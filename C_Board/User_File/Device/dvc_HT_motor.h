/**
* 1.version:2.2 data:2023/5/20
 * 2.用于驱动HT03电机
 * 3.注意can线序问题
 * 4.电机逆时针转为正
 */
#ifndef DVC_HT_MOTOR_H
#define DVC_HT_MOTOR_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"

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

#ifdef HAL_CAN_MODULE_ENABLED
        CAN_HandleTypeDef *can; // 电机CAN
#endif

#ifdef HAL_FDCAN_MODULE_ENABLED
        FDCAN_HandleTypeDef *can; // 电机CAN
#endif

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
    } HT_t;

#ifdef HAL_CAN_MODULE_ENABLED
    void HT_CAN_Callback(CAN_RxHeaderTypeDef *pHeader, uint8_t *pBuf);
    void HT_Init(HT_t *motor, uint8_t ID, CAN_HandleTypeDef *can);
#endif

#ifdef HAL_FDCAN_MODULE_ENABLED
    void HT_FDCAN_Callback(FDCAN_RxHeaderTypeDef *pHeader, uint8_t *pBuf);
    void HT_Init(HT_t *motor, uint8_t ID, FDCAN_HandleTypeDef *can);
#endif

    void HT_Run(HT_t *motor);
    void HT_SetPosition(HT_t *motor, float angle, float Kp, float Kd, float torque);
    void HT_SetSpeed(HT_t *motor, float speed, float kd);
    void HT_SetTorque(HT_t *motor, float torque);
    void HT_SetDamp(HT_t *motor, float Kd);
    void HT_SetZeroTorque(HT_t *motor);
    void HT_SendParameter(HT_t *motor, float torque, float speed, float angle, float Kp, float Kd);
    void HT_SetZeroPosition(HT_t *motor);
    uint8_t HT_ArrivalPos(HT_t *motor, float per);

#ifdef __cplusplus
}
#endif

#endif //DVC_HT_MOTOR_H