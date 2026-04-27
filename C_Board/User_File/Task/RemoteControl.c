#include "RemoteControl.h"

float MaxSpeedX = 10000;
float MaxSpeedY = 10000;
float MaxSpeedW = 10.0;


inline void Remote_Init()
{
    SBUS_Init(Remote_Callback);
}


void Remote_Callback()
{
        if (Remote_control_FS.SWA > 1000) // 遥控器开关SWA控制底盘启停
        {
            // HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_SET);
            chassis.ctrlMode = VELOCITY_MODE;

            /*通道0，前后，最上353 中间1024 最下1695*/
            chassis.setVx = (Remote_control_FS.Right_Y - 1024) / 671.0f * MaxSpeedX;
            /*通道1，左右，最左353 中间1024 最右1695*/
            chassis.setVy = -(Remote_control_FS.Right_X-1024) / 671.0f * MaxSpeedY;
            /*通道2，自转，最左1695 中间1024 最右353*/
            chassis.setVw = -(Remote_control_FS.Left_X-1024) / 671.0f * MaxSpeedW;
            Chassis_Run();
            /********大疆电机运行********/
            DJ_MotorRun();
        }
        else
        {
            DJ_SetSpeed(&DJ_Motor3508[0], 0);
            DJ_SetSpeed(&DJ_Motor3508[1], 0);
            DJ_SetSpeed(&DJ_Motor3508[2], 0);
            DJ_SetSpeed(&DJ_Motor3508[3], 0);
            DJ_MotorRun();
        }
        if (Remote_control_FS.SWB>1000)
        {
            DJ_SetAngle(&DJ_Motor2006[0],(Remote_control_FS.Left_X-1024) / 671.0f * 100,1000);
            DJ_MotorRun();
        }
        else
        {
            DJ_SetSpeed(&DJ_Motor2006[0], 0);
            DJ_MotorRun();
        }
        if (Remote_control_FS.SWD>1000)
        {
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_SET);
        }
        else
        {
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_RESET);
        }

}