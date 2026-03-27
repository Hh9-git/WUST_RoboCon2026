/**
 * 三轮全向轮底盘控制
 *
 * 以码盘正方向为Y轴正方向, 右侧为X轴正方向, 顺时针旋转为正
 * -----前ID为1----- *
 * -----左ID为2----- *
 * -----右ID为3----- *
 */

#ifndef __CHASSIS_H
#define __CHASSIS_H

#include "main.h"
#include "PID.h"
#include "RMDS_Driver.h"

#define RADIUS 152.0f  // 轮子直径
#define RR 404.0f      // 轮子到中心的距离
#define DECRATIO 12.0f // 电机减速比

struct chassis_t
{
    RMDS_t motor1;
    RMDS_t motor2;
    RMDS_t motor3;

    float Motor1_Speed;
    float Motor2_Speed;
    float Motor3_Speed;

    int16_t Vx;     // 底盘设置x坐标
    int16_t Vy;     // 底盘设置y坐标
    int16_t Vangle; // 底盘设置角度

    float Distence;   // 当前与目标的距离
    float Taget;      // 目标距离
    float precentage; // 目标距离行使比例

    float CurrrentX; // 记录当前x坐标
    float CurrrentY; // 记录当前y坐标
    float CurrrentA; // 记录当前角度

    int16_t Px;  // 底盘设置x坐标
    int16_t Py;  // 底盘设置y坐标
    int16_t yaw; // 偏航角, 绕z轴, 单位rad

    uint8_t odrive_speed; // ordive速度

    int16_t DJ_SetAngle[2]; // 设置3508角度

    float Kp;
    float Kpm;

    PID_t PID_Angle; // 角度pid
    PID_t PID_X;     // x坐标pid
    PID_t PID_Y;     // y坐标pid
} chassis;
void SetChassis();
void Chassis_Init(void);

#endif
