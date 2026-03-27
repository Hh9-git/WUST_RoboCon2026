/**
 * 1.version:5.0 date:2023/3/30
 * 2.本文件仅适用于大疆电机CAN通信
 * 3.需要添加BSP和PID两个个文件
 */

#ifndef DJ_DRIVER_H
#define DJ_DRIVER_H

#include "PID.h"
#include "can.h"

/* 电机类型 */
typedef enum
{
    M3508 = 0,
    M2006 = 1
} DJ_Motor_Type_e;

typedef enum
{
    CURRENT_MODE = 0,
    ANGLE_MODE = 1,
    SPEED_MODE = 2,
} DJ_Motor_Mode_e;

/* 电机反馈参数 */
typedef struct
{
    int16_t setAngle;   // 设置角度
    int16_t setSpeed;   // 设置速度
    int16_t setCurrent; // 设置电流

    uint16_t angle;      // CAN读取的角度
    int16_t speed;       // CAN读取的速度
    int16_t current;     // CAN读取的电流
    uint16_t last_angle; // 上一次读取的角度

    uint16_t ID;           // 电机的ID
    DJ_Motor_Mode_e mode;  // 控制模式
    uint8_t decratio;      // 电机的减速比
    uint16_t offset_angle; // 电机初始偏移角度
    float total_angle;     // 电机总角度
    int16_t round_count;   // 电机圈数

    PID_t PID_Speed;        // 电机速度环PID
    PID_t PID_Angle;        // 电机角度环PID
    PID_t PID_SpeedOfAngle; // 电机位置控制的速度环PID
} DJ_Motor_t;
DJ_Motor_t DJ_Motor3508[2];
DJ_Motor_t DJ_Motor2006[2];

extern DJ_Motor_t DJ_Motor3508[2];
extern DJ_Motor_t DJ_Motor2006[2];

void DJ_CAN_Callback(CAN_RxHeaderTypeDef *pHeader, uint8_t *pBuf);
void DJ_Init(DJ_Motor_t *motor, uint8_t Motor_ID, DJ_Motor_Type_e Motor_Type);
void DJ_MotorRun(void);
void DJ_SetAngle(DJ_Motor_t *motor, int16_t angle, uint16_t maxSpeed);
void DJ_SetSpeed(DJ_Motor_t *motor, int16_t speed);
void DJ_ClearAngle(DJ_Motor_t *motor);

#endif
