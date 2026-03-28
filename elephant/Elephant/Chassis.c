#include "Chassis.h"
#include "FastMathFunctions.h"
#include "Remote.h"
#include "DJ_Driver.h"
/* 底盘初始化 */
inline void Chassis_Init(void)
{
    AttachInterrupt_CAN(&hcan1, RMDS_CAN_Callback); // 连接迈克逊电机CAN中断回调
    // can1中断连接3508,2006电机
    AttachInterrupt_CAN(&hcan1, DJ_CAN_Callback);
    /* 码盘初始化 */
    Action_Init();

    /* 激光初始化 */
    // LDS30B_Init(&lds1, &huart7, 1);
    LDS30B_Init(&lds2, &huart8, 1);

    /* 初始化RMDS电机驱动器 */
    RMDS_Init(&chassis.motor1, 1);
    RMDS_Init(&chassis.motor2, 2);
    RMDS_Init(&chassis.motor3, 3);
#if USE_CAN
    RMDS_ResetAll(); // 没有设置模式就会叫
    HAL_Delay(200);
    RMDS_SetModeAll(SPEED);
#endif
    // 3508、2006初始化
    DJ_Init(&DJ_Motor3508[0], 1, M3508, PID_METHOD);
    DJ_Init(&DJ_Motor3508[1], 2, M3508, PID_METHOD);
    DJ_Init(&DJ_Motor2006[0], 3, M2006, PID_METHOD);
    DJ_Init(&DJ_Motor2006[1], 4, M2006, PID_METHOD);
    HAL_Delay(2);
    /* 底盘x坐标pid初始化 */
    TDPID_Init(&chassis.PID_X, 4.0, 0.1f, 1.0f, 5000.0f, 3.0f, 0.003f, 1500.0f);
    /* 底盘y坐标pid初始化 */
    TDPID_Init(&chassis.PID_Y, 3.0, 0.1f, 1.0f, 5000.0f, 3.0f, 0.003f, 1000.0f);
    /* 底盘角度pid初始化 */
    TDPID_Init(&chassis.PID_Angle, 3.0, 0.01f, 3.0f, 2.0f, 0.1f, 0.003f, 1000.0f);

    /* 激光y坐标初始化 */
    chassis.lds_y = lds2.distance;
}

/**
 * 底盘运行
 * 需要循环运行
 */
inline void Chassis_Run(void)
{
    chassis.Px = Action.pos_x - OFFSET_Y * fast_sin(-RAD(Action.yaw)) + OFFSET_X * (fast_cos(RAD(Action.yaw)) - 1);
    chassis.Py = Action.pos_y - OFFSET_Y * (fast_cos(RAD(Action.yaw)) - 1) - OFFSET_X * fast_sin(-RAD(Action.yaw)) - chassis.offset_y;

    if (chassis.ctrlMode == VELOCITY_MODE)
    {
        /* 底盘电机速度计算 */
        chassis.Motor1_Speed = (-(chassis.setVx) * fast_cos(-Action.yaw * PI / 180) + (chassis.setVy) * fast_sin(-Action.yaw * PI / 180) + chassis.setVw * RR) * 60 / (PI * RADIUS);
        chassis.Motor2_Speed = ((chassis.setVx) * fast_cos(PI / 3 - Action.yaw * PI / 180) - (chassis.setVy) * fast_sin(PI / 3 - Action.yaw * PI / 180) + chassis.setVw * RR) * 60 / (PI * RADIUS);
        chassis.Motor3_Speed = ((chassis.setVx) * fast_cos(PI / 3 + Action.yaw * PI / 180) + (chassis.setVy) * fast_sin(PI / 3 + Action.yaw * PI / 180) + chassis.setVw * RR) * 60 / (PI * RADIUS);
    }
    else if (chassis.ctrlMode == POSITION_MODE)
    {
        /* 位置PID计算x轴速度, y轴速度, 自转角速度 */
        // TDPID_Calc(&chassis.PID_X, Action.pos_x, chassis.setPx);
        // TDPID_Calc(&chassis.PID_Y, Action.pos_y, chassis.setPy);
        TDPID_Calc(&chassis.PID_X, chassis.Px, chassis.setPx);
        TDPID_Calc(&chassis.PID_Y, chassis.Py, chassis.setPy);
        TDPID_Calc(&chassis.PID_Angle, Action.yaw * PI / 180, chassis.setYaw);
        /* 底盘速度计算 */
        chassis.Motor1_Speed = (-(chassis.PID_X.out) * fast_cos(-Action.yaw * PI / 180) + (chassis.PID_Y.out) * fast_sin(-Action.yaw * PI / 180) + chassis.PID_Angle.out * RR) * 60 / (PI * RADIUS);
        chassis.Motor2_Speed = ((chassis.PID_X.out) * fast_cos(PI / 3 - Action.yaw * PI / 180) - (chassis.PID_Y.out) * fast_sin(PI / 3 - Action.yaw * PI / 180) + chassis.PID_Angle.out * RR) * 60 / (PI * RADIUS);
        chassis.Motor3_Speed = ((chassis.PID_X.out) * fast_cos(PI / 3 + Action.yaw * PI / 180) + (chassis.PID_Y.out) * fast_sin(PI / 3 + Action.yaw * PI / 180) + chassis.PID_Angle.out * RR) * 60 / (PI * RADIUS);
    }

#if USE_CAN
    /* 设置电机速度 */
    RMDS_SetSpeed(&chassis.motor1, chassis.Motor1_Speed);
    RMDS_SetSpeed(&chassis.motor2, chassis.Motor2_Speed);
    RMDS_SetSpeed(&chassis.motor3, chassis.Motor3_Speed);
#endif
}

/**
 * 设置底盘速度
 * 单位:mm/s
 * 单位:rad/s
 *
 */
inline void Chassis_SetVel(int16_t Vx, int16_t Vy, int16_t Vw)
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
