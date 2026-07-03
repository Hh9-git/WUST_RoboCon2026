#ifndef DVC_HT_MOTOR_H
#define DVC_HT_MOTOR_H
#include "drv_can.h"

#define HT_MOTOR_CAN &hcan2

// 电机极限PID
#define P_MIN -95.5f // Radians
#define P_MAX 95.5f
#define V_MIN -45.0f // Rad/s
#define V_MAX 45.0f
#define KP_MIN 0.0f // N-m/rad
#define KP_MAX 500.0f
#define KD_MIN 0.0f // N-m/rad/s
#define KD_MAX 30.0f
#define T_MIN -18.0f
#define T_MAX 18.0f


#define LIMIT_MIN_MAX(x, min, max) (x) = (((x) <= (min)) ? (min) : (((x) >= (max)) ? (max) : (x)))


#ifdef HAL_CAN_MODULE_ENABLED
#include "can.h"
#endif

#ifdef HAL_FDCAN_MODULE_ENABLED
#include "fdcan.h"
#endif

#ifndef PI
#define PI 3.14159265358979f
#endif

    /* 角度转弧度 */
#define RAD(deg) ((deg)*PI / 180)
    /* 弧度转角度 */
#define DEG(rad) ((rad)*180 / PI)

/**
 * ***HT电机参数***
 *
 * Position: 位置 rad
 * Velocity: 速度 rad/s
 * Kp: 位置增益
 * Kd: 速度增益
 * T: 力矩 N*M
 */
typedef struct
{
    CAN_HandleTypeDef *can; // 电机CAN
    uint8_t ID;     // 电机ID
    float Torque;   // 力矩
    float Velocity; // 速度
    float Position; // 位置
    float Kp;       // 刚度系数
    float Kd;       // 阻尼系数

    float setTorque;   // 设置力矩
    float setVelocity; // 设置速度
    float setPosition; // 设置位置
    float setKp;       // 设置Kp
    float setKd;       // 设置Kd
} HT_motor_struct;

extern HT_motor_struct HT_Motors[8];

void HT_Motor_Enable(CAN_HandleTypeDef *can,uint8_t HT_MOTOR_ID);
void HT_Motor_Disable(CAN_HandleTypeDef *can,uint8_t HT_MOTOR_ID);
void HT_Motor_SetZeroPosition(CAN_HandleTypeDef *can,uint8_t HT_MOTOR_ID);
void HT_Motor_Init(HT_motor_struct *motor, uint8_t ID, CAN_HandleTypeDef *can);
void HT_Run(HT_motor_struct *motor);
void HT_Motor_Set_MIT(HT_motor_struct *motor ,float torque, float speed, float angle, float Kp, float Kd);
void HT_SetPosition(HT_motor_struct *motor, float angle, float kp, float kd, float torque);
void HT_SetTorque(HT_motor_struct *motor, float torque);
void HT_CAN_Callback(CAN_RxHeaderTypeDef *pHeader, uint8_t *pBuf);
uint8_t HT_ArrivalPos(HT_motor_struct *motor, float per);

#endif //DVC_HT_MOTOR_H