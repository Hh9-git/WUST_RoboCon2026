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

    Uart_Rxdata[0] = pData[0];
    if (Uart_Rxdata[0] == '1')
    {
        LED_green_Toggle();
    }
     if (Uart_Rxdata[0] == '2')
    {
        LED_red_Toggle();
    }

}


void Task_Init()
{
    AttachInterrupt_UART(&huart3, 128, Serial_callback);
    AttachInterrupt_CAN(&hcan1, DJ_CAN_Callback);
    // DJ_Init(&DJ_Motor3508[1], 1, M3508);
    DJ_Init(&DJ_Motor3508[0], 1, M3508);
}



// uint16_t current=3000;
void Task_Loop()
{

    DJ_SetSpeed(&DJ_Motor3508[0], 10);
    DJ_MotorRun();

    // CAN_Txdata[0]=current>>8;
    // CAN_Txdata[1]=current&0xFF;
    // LED_green_Toggle();
    // UART_Print("%d",DJ_Motor3508[0].angle);
    // CAN_Transmit(&hcan1, 0x200, (uint8_t *)CAN_Txdata);

    // HAL_UART_Transmit(&huart2, (uint8_t *)"123456\r\n", 8, 100);


}
