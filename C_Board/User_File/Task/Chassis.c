#include "Chassis.h"


chassis_t chassis;

/* 底盘初始化 */
inline void Chassis_Init(void)
{
    /*************链接CAN接收中断***************/
    AttachInterrupt_CAN(&hcan1, DJ_CAN_Callback);
    /***********大疆3508初始化**************/
    DJ_Init(&DJ_Motor3508[0], 1, M3508, PID_METHOD);
    DJ_Init(&DJ_Motor3508[1], 2, M3508, PID_METHOD);
    DJ_Init(&DJ_Motor3508[2], 3, M3508, PID_METHOD);
    DJ_Init(&DJ_Motor3508[3], 4, M3508, PID_METHOD);

    DJ_SetSpeed(&DJ_Motor3508[0], 0);
    DJ_SetSpeed(&DJ_Motor3508[1], 0);
    DJ_SetSpeed(&DJ_Motor3508[2], 0);
    DJ_SetSpeed(&DJ_Motor3508[3], 0);
    HAL_Delay(2);
}

/**
 * 底盘运行
 * 需要循环运行
 */
inline void Chassis_Run(void)
{
    if (chassis.ctrlMode == VELOCITY_MODE)
    {
        chassis.Motor1_Speed=(-(sqrt(2)/2.0f)*(chassis.setVx) +(sqrt(2)/2.0f)*(chassis.setVy) + chassis.setVw * DISTANCE_TO_CENTER) / (RADIUS);
        chassis.Motor2_Speed=(-(sqrt(2)/2.0f)*(chassis.setVx) -(sqrt(2)/2.0f)*(chassis.setVy) + chassis.setVw * DISTANCE_TO_CENTER) / (RADIUS);
        chassis.Motor3_Speed=( (sqrt(2)/2.0f)*(chassis.setVx) -(sqrt(2)/2.0f)*(chassis.setVy) + chassis.setVw * DISTANCE_TO_CENTER) / (RADIUS);
        chassis.Motor4_Speed=( (sqrt(2)/2.0f)*(chassis.setVx) +(sqrt(2)/2.0f)*(chassis.setVy) + chassis.setVw * DISTANCE_TO_CENTER) / (RADIUS);

        /******设置电机速度********/
        DJ_SetSpeed(&DJ_Motor3508[0], chassis.Motor1_Speed * DECRATIO);
        DJ_SetSpeed(&DJ_Motor3508[1], chassis.Motor2_Speed * DECRATIO);
        DJ_SetSpeed(&DJ_Motor3508[2], chassis.Motor3_Speed * DECRATIO);
        DJ_SetSpeed(&DJ_Motor3508[3], chassis.Motor4_Speed * DECRATIO);
    }
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

