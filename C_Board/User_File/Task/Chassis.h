/**
* 四轮全向轮底盘控制
 *
 * 以码盘正方向为Y轴正方向, 右侧为X轴正方向, 顺时针旋转为正
 * -----前ID为1----- *
 * -----左ID为2----- *
 * -----右ID为3----- *
 */

#ifndef CHASSIS_H
#define CHASSIS_H

#include "alg_pid.h"
#include "alg_fastmath.h"
#include "dvc_dji_motor.h"


#define RADIUS 76.0f   // 轮子半径, 单位mm 152/2
#define DISTANCE_TO_CENTER 281.0f       // 轮子到中心的距离
#define DECRATIO 19.0f  // 电机减速比
#define OFFSET_X 49.25f // 码盘偏移
#define OFFSET_Y 14.21f

typedef enum
{
    VELOCITY_MODE,
    POSITION_MODE,
} chassis_ctrl_e;

typedef struct chassis_t
{
    /********************** 电机 ************************/

    float Motor1_Speed; // 底盘电机1速度
    float Motor2_Speed; // 底盘电机2速度
    float Motor3_Speed; // 底盘电机3速度
    float Motor4_Speed;

    /********************** 底盘参数 ************************/
    chassis_ctrl_e ctrlMode; // 控制模式

    float setVx; // 底盘设置x速度
    float setVy; // 底盘设置y速度
    float setVw; // 底盘设置角速度

    float setPx;  // 底盘设置x坐标, 单位mm
    float setPy;  // 底盘设置y坐标, 单位mm
    float setYaw; // 偏航角, 绕z轴, 单位rad

    float lds_y;
    float Px;
    float Py;
    float offset_y;

    float odrive_speed; // ordive速度

    int16_t DJ_SetAngle[2]; // 设置3508角度

    TDPID_t PID_Angle; // 角度pid
    TDPID_t PID_X;     // x坐标pid
    TDPID_t PID_Y;     // y坐标pid
}chassis_t;

extern chassis_t chassis;



void Chassis_Init(void);
void Chassis_Run(void);
void Chassis_SetVel(int16_t Vx, int16_t Vy, int16_t Vw);
void Chassis_SetPos(float Px, float Py, float Yaw);


#endif //CHASSIS_H