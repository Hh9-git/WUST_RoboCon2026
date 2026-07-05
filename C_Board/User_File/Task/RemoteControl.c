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
    if (Remote_control_FS.SWA > 1024)
    {
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

        //VRA旋钮控制云台旋转角度
        float HT_angle = (Remote_control_FS.VRA - 267) / 1540.0f * 360.0f;
        HT_SetPosition(&HT_Motors[7],RAD(HT_angle),0.5,0.2,0);
        HT_Run(&HT_Motors[7]);
        //Left_Y通道控制大疆3508电机上升下降
        float DJ_bottom_angle = -(Remote_control_FS.Left_Y - 1024) / 671.0f * 960.0f;
        DJ_SetAngle(&DJ_Motor3508[5], DJ_bottom_angle,5000);
        DJ_MotorRun();
        if (Remote_control_FS.SWC<1024)
        {
            //VRB控制大疆2006电机旋转角度
            float DJ_gripper_angle = (Remote_control_FS.VRB - 267) / 1540.0f * 180.0f;
            // float DJ_gripper_angle = (Remote_control_FS.VRB - 403) / 1144.0f * 180.0f;
            DJ_SetAngle(&DJ_Motor2006[0], DJ_gripper_angle,5000);
            DJ_MotorRun();
            HAL_GPIO_WritePin(GPIOI,GPIO_PIN_6,GPIO_PIN_RESET);
        }
        else if (Remote_control_FS.SWC==1024)
        {
            //VRB控制大疆2006电机旋转角度
            float DJ_gripper_angle = (Remote_control_FS.VRB - 267) / 1540.0f * 180.0f;
            DJ_SetAngle(&DJ_Motor2006[0], DJ_gripper_angle,1000);
            DJ_MotorRun();
            HAL_GPIO_WritePin(GPIOI,GPIO_PIN_6,GPIO_PIN_SET);
        }
        if (Remote_control_FS.SWB < 1024)
        {
            //SWD默认向上继电器（高电平触发）关闭，一位一通电磁阀断开，气动夹爪张开
            HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOI,GPIO_PIN_7,GPIO_PIN_RESET);
        }
        else if (Remote_control_FS.SWB >1024)
        {
            //SWD拨杆向下继电器（高电平触发）打开，一位一通电磁阀接通，气动夹爪夹紧武器杆
            //TIM8_channel1,PC6
            HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET);
        }
    }
    else if (Remote_control_FS.SWA < 1024)
    {
        DJ_SetSpeed(&DJ_Motor3508[0], 0);
        DJ_SetSpeed(&DJ_Motor3508[1], 0);
        DJ_SetSpeed(&DJ_Motor3508[2], 0);
        DJ_SetSpeed(&DJ_Motor3508[3], 0);
        DJ_MotorRun();
    }
}