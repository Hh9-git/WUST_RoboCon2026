#include "Chassis.h"
#include "FastMathFunctions.h"
#include "Remote.h"

chassis_t chassis;

/* 底盘初始化 */
inline void Chassis_Init(void)
{
    AttachInterrupt_CAN(&hcan1, DJ_CAN_Callback); // 连接大疆电机CAN中断回调

    /* 码盘初始化 */
    Action_Init();

    /* 初始化RMDS电机驱动器 */
    for (uint8_t i = 0; i < 4; i++)
    {
        DJ_Init(&chassis.ChassisMotors[i], i + 1, M3508, PID_METHOD);
    }
    for (uint8_t i = 4; i < 8; i++)
    {
        DJ_Init(&chassis.ChassisMotors[i], i + 1, M2006, PID_METHOD);
    }
    // /* 底盘x坐标pid初始化 */
    // TDPID_Init(&chassis.PID_X, 4.0, 0.01f, 0.9f, 2000.0f, 3.0f, 0.003f, 1000.0f);
    // /* 底盘y坐标pid初始化 */
    // TDPID_Init(&chassis.PID_Y, 4.0, 0.01f, 0.9f, 2000.0f, 3.0f, 0.003f, 1000.0f);
    // /* 底盘角度pid初始化 */
    // TDPID_Init(&chassis.PID_Angle, 2.0, 0.01f, 3.0f, 3.0f, 0.1f, 0.003f, 10.0f);
    /* 底盘x坐标pid初始化 */
    PID_Init(&chassis.PID_x, 4.0, 0.01f, 0.9f, 1500.0f, 3.0f);
    /* 底盘y坐标pid初始化 */
    PID_Init(&chassis.PID_y, 4.0, 0.01f, 0.9f, 1500.0f, 3.0f);
    /* 底盘角度pid初始化 */
    PID_Init(&chassis.PID_angle, 2.0, 0.01f, 3.0f, 3.0f, 0.1f);
}

/**
 * 底盘运行
 * 需要循环运行
 */
inline void Chassis_Run(void)
{
    chassis.Px = Action.pos_x - OFFSET * fast_sin(-RAD(Action.yaw));
    chassis.Py = Action.pos_y - OFFSET * (fast_cos(RAD(Action.yaw)) - 1);

    if (chassis.ctrlMode == VELOCITY_MODE)
    {
        /* 合速度大小 */
        float V = sqrtf(chassis.setVx * chassis.setVx + chassis.setVy * chassis.setVy);
        /* 合速度与Y轴夹角 */
        float angle = atan2f(chassis.setVx, chassis.setVy) + Action.yaw * PI / 180;
        /* Vx */
        float Vx = V * fast_sin(angle);
        /* Vy */
        float Vy = V * fast_cos(angle);
        /* 底盘电机速度计算 */
        chassis.Motors_Speed[0] = (-Vx + Vy - chassis.setVw * LEGNTH_WIDTH) * COEFFICIENT;
        chassis.Motors_Speed[1] = (-Vx - Vy - chassis.setVw * LEGNTH_WIDTH) * COEFFICIENT;
        chassis.Motors_Speed[2] = (Vx - Vy - chassis.setVw * LEGNTH_WIDTH) * COEFFICIENT;
        chassis.Motors_Speed[3] = (Vx + Vy - chassis.setVw * LEGNTH_WIDTH) * COEFFICIENT;
        for (uint8_t i = 0; i < 4; i++)
        {
            DJ_SetSpeed(&chassis.ChassisMotors[i], chassis.Motors_Speed[i]);
        }
    }
    else if (chassis.ctrlMode == POSITION_MODE)
    {

        /* 位置PID计算x轴速度, y轴速度, 自转角速度 */
        PID_Calc(&chassis.PID_x, chassis.Px, chassis.setPx);
        PID_Calc(&chassis.PID_y, chassis.Py, chassis.setPy);
        PID_Calc(&chassis.PID_angle, RAD(Action.yaw), chassis.setYaw);

        /* 合速度大小 */
        float V = sqrtf(chassis.PID_x.out * chassis.PID_x.out + chassis.PID_y.out * chassis.PID_y.out);
        /* 合速度与Y轴夹角 */
        float angle = atan2f(chassis.PID_x.out, chassis.PID_y.out) + Action.yaw * PI / 180;

        /* Vx */
        float Vx = V * fast_sin(angle);
        /* Vy */
        float Vy = V * fast_cos(angle);
        /* 底盘电机速度计算 */
        chassis.Motors_Speed[0] = (-Vx + Vy - chassis.PID_angle.out * LEGNTH_WIDTH) * COEFFICIENT;
        chassis.Motors_Speed[1] = (-Vx - Vy - chassis.PID_angle.out * LEGNTH_WIDTH) * COEFFICIENT;
        chassis.Motors_Speed[2] = (Vx - Vy - chassis.PID_angle.out * LEGNTH_WIDTH) * COEFFICIENT;
        chassis.Motors_Speed[3] = (Vx + Vy - chassis.PID_angle.out * LEGNTH_WIDTH) * COEFFICIENT;
        for (uint8_t i = 0; i < 4; i++)
        {
            DJ_SetSpeed(&chassis.ChassisMotors[i], chassis.Motors_Speed[i]);
        }
    }
#if USE_CAN
    /* 大疆电机运行 */
    DJ_MotorRun();
#endif
}

/**
 * 设置底盘速度
 * 单位:mm/s
 * 单位:rad/s
 */
inline void Chassis_SetVel(float Vx, float Vy, float Vw)
{
    chassis.ctrlMode = VELOCITY_MODE;
    chassis.setVx = Vx;
    chassis.setVy = Vy;
    chassis.setVw = Vw;
}

/**
 * 设置底盘位置
 * 单位:mm
 * 单位:rad
 */
inline void Chassis_SetPos(float Px, float Py, float Yaw)
{
    chassis.ctrlMode = POSITION_MODE;
    chassis.setPx = Px;
    chassis.setPy = Py;
    chassis.setYaw = Yaw;
}

/**
 * 设置夹取角度
 * 单位:度
 */
void cliping(float angle)
{
    DJ_SetAngle(&chassis.ChassisMotors[4], angle, 2000.0f);
    DJ_SetAngle(&chassis.ChassisMotors[5], -(angle), 2000.0);
}

/**
 * 设置丝杆角度 取环时下降 取完后上升到发射位置
 * 单位:度
 */
void launch(float angle)
{
    DJ_SetAngle(&chassis.ChassisMotors[6], angle, 17000.0f);
    DJ_SetAngle(&chassis.ChassisMotors[7], angle, 17000.0f);
}

void lifting(float angle)
{
    DJ_ClearAngle(&chassis.ChassisMotors[6]);
    DJ_ClearAngle(&chassis.ChassisMotors[7]);
    if (angle > 0)
    {
        DJ_SetAngle(&chassis.ChassisMotors[6], angle, 17000.0f);
        DJ_SetAngle(&chassis.ChassisMotors[7], angle, 17000.0f);
    }
    else
    {
        DJ_SetAngle(&chassis.ChassisMotors[6], angle, 17000.0f);
        DJ_SetAngle(&chassis.ChassisMotors[7], angle, 17000.0f);
    }
}

void Chassis_clear(chassis_t chassis)
{
    chassis.PID_x.err[0] = chassis.PID_x.err[1] = 0;
    chassis.PID_y.err[0] = chassis.PID_y.err[1] = 0;
    chassis.PID_angle.err[0] = chassis.PID_angle.err[1] = 0;
    chassis.PID_x.Iout = chassis.PID_x.out = 0;
    chassis.PID_y.Iout = chassis.PID_y.out = 0;
    chassis.PID_angle.Iout = chassis.PID_angle.out = 0;
}