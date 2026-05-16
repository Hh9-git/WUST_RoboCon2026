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
        if (Remote_control_FS.SWB>1000&&Remote_control_FS.SWA > 1000)
        {
            HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET);

            int16_t angle =-(Remote_control_FS.Left_Y- 1024) / 671 * 135;
            Servo_SetAngle_135(TIM_CHANNEL_1,angle);
            Servo_SetAngle_135(TIM_CHANNEL_2,angle);
            OLED_printf(1,0,"Angle:%+d",angle);
        }
        else
        {
            HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_RESET);
            Servo_SetAngle_135(TIM_CHANNEL_1,0);
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