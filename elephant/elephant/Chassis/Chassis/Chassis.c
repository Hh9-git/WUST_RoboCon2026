#include "Chassis.h"
#include "BSP.h"
#include "Remote.h"
#include "FastMathFunctions.h"
#include "Action.h"
#include "DJ_Driver.h"

void SetChassis()
{
    if (remote.mode == 0)
    {
        chassis.Kpm = 500.0;
        chassis.Kp = 0.50;

        chassis.Distence = (chassis.Px - Action.pos_x) * (chassis.Px - Action.pos_x) + (chassis.Py - Action.pos_y) * (chassis.Py - Action.pos_y); // 当前点与目标点距离的平方
        if (chassis.Distence < 1000000)
        {
            chassis.Kpm = 500.0;
            chassis.Kp = 0.5;
        }
        // chassis.Taget = (chassis.Px - chassis.CurrrentX) * (chassis.Px - chassis.CurrrentX) + (chassis.Py - chassis.CurrrentY) * (chassis.Py - chassis.CurrrentY); // 起点与终点的距离
        // chassis.precentage = chassis.Distence / chassis.Taget;
        // if (chassis.precentage >= 0.6 && chassis.precentage <= 0.8)
        // {
        //     chassis.Kpm = (1 - chassis.precentage) * 2000.0;
        // }
        // if (chassis.precentage < 0.6)
        // {
        //     chassis.Kpm = 2000.0;
        // }
        // if (chassis.precentage > 0.8)
        // {
        //     chassis.Kpm = 400.0;
        // }
        /* 底盘x坐标pid初始化 */
        PID_Init(&chassis.PID_X, 0.5, 0.08f, 1.0f, chassis.Kpm, 6.0f);
        /* 底盘y坐标pid初始化 */
        PID_Init(&chassis.PID_Y, 0.5, 0.08f, 1.0f, chassis.Kpm, 6.0f);
        /* 底盘角度pid初始化 */
        PID_Init(&chassis.PID_Angle, 3.0, 0.01f, 2.0f, 1.0, 0.02f);
        /* PID计算x轴速度, y轴速度, 自转角速度 */
        PID_Calc(&chassis.PID_X, Action.pos_x, chassis.Px);
        PID_Calc(&chassis.PID_Y, Action.pos_y, chassis.Py);
        PID_Calc(&chassis.PID_Angle, Action.yaw * PI / 180, chassis.yaw * PI / 180);
        /* 计算电机速度 定点*/
        chassis.Motor1_Speed = (-(chassis.PID_X.out) * fast_cos(-Action.yaw * PI / 180) + (chassis.PID_Y.out) * fast_sin(-Action.yaw * PI / 180) + (chassis.PID_Angle.out) * RR) * 60 / (PI * RADIUS);
        chassis.Motor2_Speed = ((chassis.PID_X.out) * fast_cos(PI / 3 - Action.yaw * PI / 180) - (chassis.PID_Y.out) * fast_sin(PI / 3 - Action.yaw * PI / 180) + (chassis.PID_Angle.out) * RR) * 60 / (PI * RADIUS);
        chassis.Motor3_Speed = ((chassis.PID_X.out) * fast_cos(PI / 3 + Action.yaw * PI / 180) + (chassis.PID_Y.out) * fast_sin(PI / 3 + Action.yaw * PI / 180) + (chassis.PID_Angle.out) * RR) * 60 / (PI * RADIUS);
    }
    if (remote.mode == 1)
    { /* 计算电机速度 遥控*/
        chassis.Kp = 3.0;
        chassis.Motor1_Speed = (-(chassis.Vx) * fast_cos(-Action.yaw * PI / 180) + (chassis.Vy) * fast_sin(-Action.yaw * PI / 180) + (chassis.Vangle) * RR) * 60 / (PI * RADIUS);
        chassis.Motor2_Speed = ((chassis.Vx) * fast_cos(PI / 3 - Action.yaw * PI / 180) - (chassis.Vy) * fast_sin(PI / 3 - Action.yaw * PI / 180) + (chassis.Vangle) * RR) * 60 / (PI * RADIUS);
        chassis.Motor3_Speed = ((chassis.Vx) * fast_cos(PI / 3 + Action.yaw * PI / 180) + (chassis.Vy) * fast_sin(PI / 3 + Action.yaw * PI / 180) + (chassis.Vangle) * RR) * 60 / (PI * RADIUS);
    }
    if (remote.mode == 99 || remote.mode == 102 || remote.mode == 103 || remote.mode == 10)
    {
        chassis.Motor1_Speed = 0.0;
        chassis.Motor2_Speed = 0.0;
        chassis.Motor3_Speed = 0.0;
    }

    /* 设置电机速度 */
    RMDS_SetSpeed(&chassis.motor1, chassis.Motor1_Speed);
    RMDS_SetSpeed(&chassis.motor2, chassis.Motor2_Speed);
    RMDS_SetSpeed(&chassis.motor3, chassis.Motor3_Speed);

    HAL_Delay(2);
}

/* 底盘初始化 */
void Chassis_Init(void)
{
    // can1中断连接麦克逊电机
    AttachInterrupt_CAN(&hcan1, RMDS_CAN_Callback);
    // can2中断连接3508电机
    AttachInterrupt_CAN(&hcan1, DJ_CAN_Callback);
    /* 码盘初始化 */
    AttachInterrupt_UART(&REMOTE_UART, 64, Remote_Callback);

    Action_Init();
    /* 初始化RMDS电机驱动器 */
    RMDS_Init(&chassis.motor1, 1);
    RMDS_Init(&chassis.motor2, 2);
    RMDS_Init(&chassis.motor3, 3);
    HAL_Delay(2);

    RMDS_ResetAll();
    HAL_Delay(200);
    RMDS_SetModeAll(SPEED);
    // 3508、2006初始化
    DJ_Init(&DJ_Motor3508[0], 1, M3508);
    DJ_Init(&DJ_Motor3508[1], 2, M3508);
    DJ_Init(&DJ_Motor2006[0], 3, M2006);
    DJ_Init(&DJ_Motor2006[1], 4, M2006);

    HAL_Delay(200);
    /* 底盘x坐标pid初始化 */
    PID_Init(&chassis.PID_X, 1.0, 0.08f, 1.0f, 1000.0f, 6.0f);
    /* 底盘y坐标pid初始化 */
    PID_Init(&chassis.PID_Y, 1.0, 0.08f, 1.0f, 1000.0f, 6.0f);
    /* 底盘角度pid初始化 */
    PID_Init(&chassis.PID_Angle, 6.0, 0.05f, 0.8f, 1.0f, 0.02f);
    HAL_Delay(3000);
}
