#include "RemoteControl.h"
#include "dvc_remote.h"
#include "stm32f407xx.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal_gpio.h"
#include <stdint.h>

float MaxSpeedX = 10000;
float MaxSpeedY = 10000;
float MaxSpeedW = 10.0;


uint8_t Pusher_Flag=0;//0 is retract and 1 is extend
uint8_t Griper_Flag=0;//0 is open and 1 is close
uint8_t Suction_Flag=0;//0 is pick and 1 is release

inline void Remote_Init()
{
    SBUS_Init(Remote_Callback);
}

void Remote_Callback()
{
    /*遥控器拨杆向上，运行所有电机*/
    if (Remote_control_FS.SWA > 1024)
    {
        //Chassic run all time
        chassis.ctrlMode = VELOCITY_MODE;
        /*通道0，前后，最上353 中间1024 最下1695*/
        chassis.setVx = (Remote_control_FS.Right_X - 1024) / 671.0f * MaxSpeedX;
        /*通道1，左右，最左353 中间1024 最右1695*/
        chassis.setVy = (Remote_control_FS.Right_Y - 1024) / 671.0f * MaxSpeedY;
        /*通道2，自转，最左1695 中间1024 最右353*/
        chassis.setVw = -(Remote_control_FS.Left_X-1024) / 671.0f * MaxSpeedW;
        Chassis_Run();
        /********大疆电机运行********/
        DJ_MotorRun();


        //Left_Y通道控制大疆3508电机上升下降
        float DJ_Y_angle = (Remote_control_FS.VRB - 1024) / 671.0f * 540.0f;
        DJ_SetAngle(&DJ_Motor3508[5], DJ_Y_angle,5000);
        DJ_MotorRun();

        //Lever SWC up: control pusher extend/retract.
        if (Remote_control_FS.SWC<1024)
        {
            //Lever SWD up: pusher off by default (retracted).
            if(Remote_control_FS.SWD<1024)
            {
                 Pusher_Flag=0;
                //  HAL_GPIO_WritePin(GPIOI,GPIO_PIN_6,GPIO_PIN_RESET);
            }
            //Lever SWD down: pneumatic pusher on by default (extended)
            else if(Remote_control_FS.SWD>1024)
            {
                 Pusher_Flag=1;
                //  HAL_GPIO_WritePin(GPIOI,GPIO_PIN_6,GPIO_PIN_SET);
            }
        }
        //Lever SWC neutral: control gripper open/close.
        else if (Remote_control_FS.SWC==1024)
        {
        
            //Lever SWD up: gripper open by default
            if(Remote_control_FS.SWD<1024)
            {
                Griper_Flag=0;
                //  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_RESET);
            }
            //Lever SWD down: gripper close by default
            else if(Remote_control_FS.SWD>1024)
            {
                Griper_Flag=1;
                //  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET);
            }
    
        }
        //Lever SWC down: control suction cup pick/release.
        else if(Remote_control_FS.SWC>1024)
        {
            //Lever up: suction cup pick by default
            if(Remote_control_FS.SWD<1024)
            {
                Suction_Flag=0;
                //  HAL_GPIO_WritePin(GPIOI,GPIO_PIN_7,GPIO_PIN_SET);
            }
            //Lever down: suction cup release by default
            else if(Remote_control_FS.SWD>1024)
            {
                Suction_Flag=1;
                //  HAL_GPIO_WritePin(GPIOI,GPIO_PIN_7,GPIO_PIN_RESET);
            }
        }


        //SWB controls the rotational angle of the 2006 motor and servo
        if(Remote_control_FS.SWB<1024)
        {

            
            //The knob controls the rotational angle of the Haitai motor.
            float HT_angle = (Remote_control_FS.VRA - 1024) / 671.0f * 720.0f;
            HT_SetPosition(&HT_Motors[7],RAD(HT_angle),0.5,0.2,0);
            HT_Run(&HT_Motors[7]);


            float DJ_gripper_angle = (Remote_control_FS.Left_Y - 1024) / 671.0f * 360.0f; 
            DJ_SetAngle(&DJ_Motor2006[0], DJ_gripper_angle,16000);
            DJ_MotorRun();
        }
        else if(Remote_control_FS.SWB>1024)
        {
           // VRB通道控制x
            float DJ_X_angle = (Remote_control_FS.VRA - 1024) / 671.0f * 720.0f;
            DJ_SetAngle(&DJ_Motor3508[4], DJ_X_angle,5000);
            DJ_MotorRun();


            float Servo_angle = (Remote_control_FS.Left_Y - 1024) / 671.0f * 540.0f;
            Servo_SetAngle_135(TIM_CHANNEL_1, Servo_angle);       
        }

         //Pusher control
        if(Pusher_Flag==1)
        {
            HAL_GPIO_WritePin(GPIOI,GPIO_PIN_6,GPIO_PIN_SET);
        }
        else if(Pusher_Flag==0)
        {
            HAL_GPIO_WritePin(GPIOI,GPIO_PIN_6,GPIO_PIN_RESET);
        }

        //Gripper control
        if(Griper_Flag==1)
        {
            HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET);
        }
        else if(Griper_Flag==0)
        {
            HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_RESET);
        }

        //Suction cup control
        if(Suction_Flag==1)
        {
            HAL_GPIO_WritePin(GPIOI,GPIO_PIN_7,GPIO_PIN_RESET);
        }
        else if(Suction_Flag==0)
        {
            HAL_GPIO_WritePin(GPIOI,GPIO_PIN_7,GPIO_PIN_SET);
        }
    }
    /********遥控器拨杆向下，停止所有电机运行********/
    else if (Remote_control_FS.SWA < 1024)
    {
        DJ_SetSpeed(&DJ_Motor3508[0], 0);
        DJ_SetSpeed(&DJ_Motor3508[1], 0);
        DJ_SetSpeed(&DJ_Motor3508[2], 0);
        DJ_SetSpeed(&DJ_Motor3508[3], 0);
        DJ_MotorRun();
    }
}



