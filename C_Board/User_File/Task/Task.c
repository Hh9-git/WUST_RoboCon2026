#include "Task.h"

#include "drv_can.h"
#include "drv_tim.h"
#include "stm32f4xx_hal.h"
#include "drv_usart.h"
#include "dvc_dji_motor.h"
#include "dvc_vofa.h"
#include "drv_tim.h"
#include "dvc_remote.h"

DJ_Motor_t DJ_Motor3508[2];

/* 毫秒定时器 */
void MM_TIM_Callback(void)
{
    static uint16_t count;
    /***********0.1秒计时器***********/
    if (count++ > 100)
    {
        /***********程序运行指示灯************/
        HAL_GPIO_TogglePin(GPIOH, GPIO_PIN_10);
        count = 0;
    }
}
/*****任务初始化******/
void Task_Init(void)
{
    /*********开启定时器*********/
    AttachInterrupt_TIM(&htim7,MM_TIM_Callback);
    HAL_TIM_Base_Start_IT(&htim7);
    /*********开启VOFA*********/
    AttachInterrupt_UART_DMA(&huart1,DataBuff,200,Vofa_Callback);
    /*********开启CAN**********/
    AttachInterrupt_CAN(&hcan1, DJ_CAN_Callback);
    DJ_Init(&DJ_Motor3508[0], 1, M3508, PID_METHOD);
    DJ_SetSpeed(&DJ_Motor3508[0], 0);
    // DJ_SetAngle(&DJ_Motor3508[0], 360.0f, 1000.0f);
    // DJ_SetSpeed(&DJ_Motor3508[0], 1000.0f);
    /*********遥控测试**********/
    AttachInterrupt_UART_DMA(&huart3,Rx_buf,64,Remote_callback);
}

/*******任务执行循环*********/
void Task_loop(void)
{
    DJ_SetSpeed(&DJ_Motor3508[0],Remote_control_FS.Left_X);
    /********大疆电机运行********/
    DJ_MotorRun();
    /********VOFA绘图**********/
    justfloat_displaydata(DJ_Motor3508[0].setAngle, DJ_Motor3508[0].angle, DJ_Motor3508[0].PID_Angle.out, DJ_Motor3508[0].setSpeed, DJ_Motor3508[0].speed, DJ_Motor3508[0].PID_Speed.out);
}