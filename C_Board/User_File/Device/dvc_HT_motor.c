#include "dvc_HT_motor.h"
#include "stdlib.h"
#include "dvc_vofa.h"

/* HT电机模式选择 */
enum HT_Commands
{
    CMD_ENTER_MOTOR_MODE = 0xFC,
    CMD_EXIT_MOTOR_MODE = 0xFD,
    CMD_SET_MOTOR_ZEROPOSITION = 0xFE
};

static uint8_t cmd[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00};

/**
 * 映射设置的float参数到HT电机适配参数
 * 16 bit position command
 * 12 bit velocity command
 * 12 bit Kp
 * 12 bit Kd
 * 12 bit Feed-Forward Current 前馈电流
 */
static inline uint16_t float_to_uint(float x, float x_min, float x_max, uint8_t bits)
{
    return (uint16_t)((x - x_min) * ((float)((1 << bits) - 1)) / (x_max - x_min));
}

/**
 * CAN接收数据映射到float类型
 */
static inline float uint_to_float(int x_int, float x_min, float x_max, int bits)
{
    return ((float)x_int) * (x_max - x_min) / ((float)((1 << bits) - 1)) + x_min;
}




/**
 * @brief 海泰电机使能
 * @param can
 * @param HT_MOTOR_ID
 */
void HT_Motor_Enable(CAN_HandleTypeDef *can,uint8_t HT_MOTOR_ID)
{
    cmd[7]=CMD_ENTER_MOTOR_MODE;
    CAN_Transmit(can,HT_MOTOR_ID,cmd);
}

/**
 * @brief 海泰电机失能
 * @param can
 * @param HT_MOTOR_ID
 */
void HT_Motor_Disable(CAN_HandleTypeDef *can,uint8_t HT_MOTOR_ID)
{
    cmd[7]=CMD_EXIT_MOTOR_MODE;
    CAN_Transmit(can,HT_MOTOR_ID,cmd);
}

/**
 * @brief 海泰电机设置0位置
 * @param can
 * @param HT_MOTOR_ID
 */
void HT_Motor_SetZeroPosition(CAN_HandleTypeDef *can,uint8_t HT_MOTOR_ID)
{
    cmd[7]=CMD_SET_MOTOR_ZEROPOSITION;
    CAN_Transmit(can,HT_MOTOR_ID,cmd);
}


static HT_motor_struct **MotorsArray;  // 电机指针数组
static uint8_t MotorsCount; // 电机数量

// CAN中断回调函数
void HT_CAN_Callback(CAN_RxHeaderTypeDef *pHeader, uint8_t *pBuf)
{
    if (pHeader->StdId == 0x00)
    {
        for (uint8_t i = 0; i < MotorsCount; i++)
        {
            if (MotorsArray[i]->ID == pBuf[0])
            {
                // 接收数据转换
                MotorsArray[i]->Position = uint_to_float((uint16_t)(pBuf[1] << 8 | pBuf[2]), P_MIN, P_MAX, 16);
                MotorsArray[i]->Velocity = uint_to_float((uint16_t)(pBuf[3] << 4 | pBuf[4] >> 4), V_MIN, V_MAX, 12);
                MotorsArray[i]->Torque = uint_to_float((uint16_t)((pBuf[4] & 0x0F) << 8 | pBuf[5]), T_MIN, T_MAX, 12);
            }
        }
    }
}

// 电机初始化,不可以重复初始化
void HT_Motor_Init(HT_motor_struct *motor, uint8_t ID, CAN_HandleTypeDef *can)
{
    motor->can = can;
    MotorsCount++; // 电机数+1
    MotorsArray = (HT_motor_struct **)realloc(MotorsArray, MotorsCount * sizeof(HT_motor_struct *));
    MotorsArray[MotorsCount - 1] = motor;
    motor->ID= ID;
    // 进入电机模式
    cmd[7] = CMD_ENTER_MOTOR_MODE;
    CAN_Transmit(can, ID, cmd);
    HAL_Delay(1);

    // 设置零参数
    HT_Motor_Set_MIT(motor, 0, 0, 0, 0, 0);
    HAL_Delay(1);

    // 设置电机零位置
    cmd[7] = CMD_SET_MOTOR_ZEROPOSITION;
    CAN_Transmit(can, ID, cmd);
    HAL_Delay(1);
}

// 设置电机参数
inline void HT_Motor_Set_MIT(HT_motor_struct *motor ,float torque, float speed, float angle, float Kp, float Kd)
{
    uint16_t p, v, kp, kd, t;
    uint8_t buf[8];

    // LIMIT_MIN_MAX(angle, -PI, PI); // 限制最大角度和最小角度

    /* 根据协议，对float参数进行转换 */
    p = float_to_uint(angle, P_MIN, P_MAX, 16);
    v = float_to_uint(speed, V_MIN, V_MAX, 12);
    kp = float_to_uint(Kp, KP_MIN, KP_MAX, 12);
    kd = float_to_uint(Kd, KD_MIN, KD_MAX, 12);
    t = float_to_uint(torque, T_MIN, T_MAX, 12);

    /* 根据传输协议，把数据转换为CAN命令数据字段 */
    buf[0] = p >> 8;
    buf[1] = p & 0xFF;
    buf[2] = v >> 4;
    buf[3] = ((v & 0xF) << 4) | (kp >> 8);
    buf[4] = kp & 0xFF;
    buf[5] = kd >> 4;
    buf[6] = ((kd & 0xF) << 4) | (t >> 8);
    buf[7] = t & 0xff;

    CAN_Transmit(motor->can,motor->ID, buf);

}


// 设置位置模式, 顺时针为正, rad
inline void HT_SetPosition(HT_motor_struct *motor, float angle, float kp, float kd, float torque)
{
    motor->setTorque = torque;
    motor->setVelocity = 0;
    motor->setPosition = angle;
    motor->setKp = kp;
    motor->setKd= kd;
}

// 设置力矩模式, N*M
inline void HT_SetTorque(HT_motor_struct *motor, float torque)
{
    // HT_SendParameter(motor, torque, 0, 0, 0, 0);
    motor->setTorque = torque;
    motor->setVelocity = 0;
    motor->setPosition = 0;
    motor->setKp = 0;
    motor->setKd = 0;
}

inline void HT_Run(HT_motor_struct *motor)
{
    HT_Motor_Set_MIT(motor, motor->setTorque, motor->setVelocity, motor->setPosition, motor->setKp, motor->setKd);
}

/****检测是否到达位置*****/
inline uint8_t HT_ArrivalPos(HT_motor_struct *motor, float per)
{
    float temp = motor->Position - motor->setPosition;

    /* 取绝对值 */
    if (temp < 0)
    {
        temp = -temp;
    }

    if (temp < per)
    {
        return 1;
    }

    return 0;
}
