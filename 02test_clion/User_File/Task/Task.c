/**
 * @file Task.c
 * @author Hh999999
 * @brief 任务函数，放在一个单独的文件里，main函数里调用Task_Loop()，
 * 把任务函数和main函数分开，main函数只负责调用任务函数，任务函数负责具体的任务逻辑，这样代码结构更清晰，易于维护和扩展
 * @version 1.0
 * @date 2026-03-05 23:00 1.0
 *
 */


#include "Task.h"
#include "usart.h"
#include "../Device/dvc_LED.h"
#include "drv_can.h"
#include "drv_usart.h"
#include "dvc_vofa.h"
#include "dvc_dji_motor.h"

uint8_t Uart_Rxdata[128];
uint8_t CAN_Txdata[8]={10,20,30,40,50,60,70,80};
DJ_Motor_t DJ_Motor3508[2];

void Serial_callback(uint8_t *pData, uint8_t size)
{
    
}


void Task_Init()
{
    AttachInterrupt_UART(&huart3, 128, Serial_callback);
    AttachInterrupt_CAN(&hcan1, DJ_CAN_Callback);
    // DJ_Init(&DJ_Motor3508[1], 1, M3508);
    DJ_Init(&DJ_Motor3508[0], 1, M3508, PID_METHOD);
    DJ_SetSpeed(&DJ_Motor3508[0], 3000.0f);
    // DJ_SetAngle(&DJ_Motor3508[0], 720.0f, 1000.0f);



}

void Task_Loop()
{
    // justfloat_displaydata(DJ_Motor3508[0].setAngle, DJ_Motor3508[0].total_angle, DJ_Motor3508[0].PID_Angle.out, DJ_Motor3508[0].setSpeed, DJ_Motor3508[0].speed, DJ_Motor3508[0].PID_SpeedOfAngle.out);
    // firewater_displaydata(DJ_Motor3508[0].setAngle, DJ_Motor3508[0].total_angle, DJ_Motor3508[0].PID_Angle.out, DJ_Motor3508[0].setSpeed, DJ_Motor3508[0].speed, DJ_Motor3508[0].PID_SpeedOfAngle.out);
    UART_Print("%f,%f,%f,%f,%f,%f\r\n", DJ_Motor3508[0].setAngle, DJ_Motor3508[0].total_angle,DJ_Motor3508[0].PID_Angle.out,DJ_Motor3508[0].setSpeed,DJ_Motor3508[0].speed, DJ_Motor3508[0].PID_SpeedOfAngle.out);
    DJ_MotorRun();
}
