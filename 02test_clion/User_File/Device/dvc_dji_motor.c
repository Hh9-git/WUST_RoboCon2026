#include "dvc_dji_motor.h"


/* 电机ID地址 */
enum CAN_Motor_ID
{
    DJ_H_ID = 0x1FF,
    DJ_L_ID = 0x200,
    DJ_M1_ID = 0x201,
    DJ_M2_ID = 0x202,
    DJ_M3_ID = 0x203,
    DJ_M4_ID = 0x204,
    DJ_M5_ID = 0x205,
    DJ_M6_ID = 0x206,
    DJ_M7_ID = 0x207,
    DJ_M8_ID = 0x208,
};


volatile static uint8_t motorCount;
static DJ_Motor_t *Motors[8]; // 电机参数结构体指针数组, ID即为索引

/* CAN中断回调函数 */
void DJ_CAN_Callback(CAN_RxHeaderTypeDef *pHeader, uint8_t *pBuf)
{
    switch (pHeader->StdId)
    {
    case DJ_M1_ID:
    case DJ_M2_ID:
    case DJ_M3_ID:
    case DJ_M4_ID:
    case DJ_M5_ID:
    case DJ_M6_ID:
    case DJ_M7_ID:
    case DJ_M8_ID:
        {
            static uint8_t i = 0;

            i = pHeader->StdId - DJ_M1_ID;

            Motors[i]->last_angle = Motors[i]->angle;
            Motors[i]->angle = (uint16_t)(pBuf[0] << 8 | pBuf[1]);
            Motors[i]->speed = (uint16_t)(pBuf[2] << 8 | pBuf[3]);
            Motors[i]->current = (uint16_t)(pBuf[4] << 8 | pBuf[5]);

            if (Motors[i]->angle - Motors[i]->last_angle > 4096)
            {
                Motors[i]->round_count--;
            }
            else if (Motors[i]->angle - Motors[i]->last_angle < -4096)
            {
                Motors[i]->round_count++;
            }
            Motors[i]->total_angle = (Motors[i]->round_count + (Motors[i]->angle - Motors[i]->offset_angle) / 8192.0f) * 360.0f / Motors[i]->decratio;
        }
    }
}

//--------------------------------------------------------------------------
// 大疆电机控制
// 需要循环执行此函数
void DJ_MotorRun(void)
{
    static uint8_t data[8];

    /* 控制电机速度 */
    for (uint8_t i = 0; i < 8; i++)
    {
        if (Motors[i]->mode == SPEED_MODE) // 如果标志为真, 开启速度控制
        {

            PID_Calc(&Motors[i]->PID_Speed, (float)(Motors[i]->speed), (float)Motors[i]->setSpeed);
            Motors[i]->setCurrent = (int16_t)Motors[i]->PID_Speed.out;
        }
    }

    /* 控制电机角度 */
    for (uint8_t i = 0; i < 8; i++)
    {
        if (Motors[i]->mode == ANGLE_MODE) // 如果标志为真, 开启角度控制
        {
            // 串级PID控制
            PID_Calc(&Motors[i]->PID_Angle, (float)(Motors[i]->total_angle), (float)Motors[i]->setAngle);
            PID_Calc(&Motors[i]->PID_SpeedOfAngle, (float)(Motors[i]->speed), Motors[i]->PID_Angle.out);
            Motors[i]->setCurrent = (int16_t)Motors[i]->PID_SpeedOfAngle.out;
        }
    }



    /* 控制电机电流 */
    data[0] = Motors[0]->setCurrent >> 8;
    data[1] = Motors[0]->setCurrent;
    data[2] = Motors[1]->setCurrent >> 8;
    data[3] = Motors[1]->setCurrent;
    data[4] = Motors[2]->setCurrent >> 8;
    data[5] = Motors[2]->setCurrent;
    data[6] = Motors[3]->setCurrent >> 8;
    data[7] = Motors[3]->setCurrent;
    CAN_Transmit(&DJ_MOTOR_CAN, DJ_L_ID, data);
    data[0] = Motors[4]->setCurrent >> 8;
    data[1] = Motors[4]->setCurrent;
    data[2] = Motors[5]->setCurrent >> 8;
    data[3] = Motors[5]->setCurrent;
    data[4] = Motors[6]->setCurrent >> 8;
    data[5] = Motors[6]->setCurrent;
    data[6] = Motors[7]->setCurrent >> 8;
    data[7] = Motors[7]->setCurrent;
    CAN_Transmit(&DJ_MOTOR_CAN, DJ_H_ID, data);
}


/**
 * 电机参数初始化
 * 不可以重复初始化同一个电机
 */
void DJ_Init(DJ_Motor_t *motor, uint8_t Motor_ID, DJ_Motor_Type_e Motor_Type)
{
    if (Motor_Type == M2006)
    {
        motor->decratio = M2006_DECRATIO;
        // 速度PID初始化
        PID_Init(&motor->PID_Speed, 5.0f, 0.5f, 0.0f, 16000.0f, 1000.0f);

        // 角度PID初始化
        PID_Init(&motor->PID_Angle, 150.0f, 0.5f, 10.0f, 18000.0f, 100.0f);
        PID_Init(&motor->PID_SpeedOfAngle, 5.0f, 0.0f, 0.0f, 16000.0f, 500.0f);
    }
    else if (Motor_Type == M3508)
    {
        motor->decratio = M3508_DECRATIO;
        // 速度PID初始化
        PID_Init(&motor->PID_Speed, 5.0f, 0.5f, 0.0f, 16000.0f, 1000.0f);

        // 角度PID初始化
        PID_Init(&motor->PID_Angle, 100.0f, 0.0f, 0.0f, 4000.0f, 30.0f);
        PID_Init(&motor->PID_SpeedOfAngle, 5.0f, 0.0f, 0.0f, 16000.0f, 200.0f);
    }

    motorCount++;
    motor->ID = DJ_L_ID + Motor_ID;
    motor->setCurrent = 0;
    motor->offset_angle = 0;
    motor->round_count = 0;
    Motors[Motor_ID - 1] = motor;
}


/**
 * 设置电机角度
 * 设置一次即可
 * 单位:度
 */
inline void DJ_SetAngle(DJ_Motor_t *motor, int16_t angle, uint16_t maxSpeed)
{
    motor->PID_Angle.max_out = maxSpeed;
    motor->setAngle = angle;
    motor->mode = ANGLE_MODE;
}

/**
 * 设置电机速度
 * 设置一次即可
 * 单位:未减速前的r/min
 */
inline void DJ_SetSpeed(DJ_Motor_t *motor, int16_t speed)
{
    motor->setSpeed = speed;
    motor->mode = SPEED_MODE;
}

/* 电机角度和圈数清零 */
inline void DJ_ClearAngle(DJ_Motor_t *motor)
{
    motor->total_angle = 0;
    motor->round_count = 0;
}

