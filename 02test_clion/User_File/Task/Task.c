/**
 * @file Task.c
 * @author Hh999999
 * @brief 任务函数，放在一个单独的文件里，main函数里调用Task_Loop()，
 * 把任务函数和main函数分开，main函数只负责调用任务函数，任务函数负责具体的任务逻辑，这样代码结构更清晰，易于维护和扩展
 * @version 1.0
 * @date 2026-03-05 23:00 1.0
 *
 * /

/* Includes ------------------------------------------------------------------*/
#include "Task.h"
#include "gpio.h"
#include "usart.h"
#include "../Device/dvc_LED.h"
#include "drv_can.h"


uint8_t Tx_data[5]="Hello";
uint8_t Rx_data[5];

 uint8_t CAN1_0x200_Tx_Data[8]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};


void Task_Init()
{
    CAN_Init(&hcan1, NULL);
    // HAL_UARTEx_ReceiveToIdle_DMA(&huart2, Rx_data, 5);



}

void Task_Loop()
{
    while (1)
    {

        // HAL_UART_Transmit_DMA(&huart2, Tx_data, 5);
        // HAL_Delay(500);
        CAN_Send_Data(&hcan1, 0x200, CAN1_0x200_Tx_Data, 8);
        HAL_Delay(500);


    }
}

//
// void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
// {
//     if (huart->Instance == USART2)
//     {
//         // 接收完成后再次开启接收中断
//         HAL_UARTEx_ReceiveToIdle_DMA(&huart2, Rx_data, 5);
//
//         if (Rx_data[0]=='1')
//         {
//             LED_red_Toggle();
//             Rx_data[0]='0';
//         }
//         else if (Rx_data[0]=='2')
//         {
//             LED_green_Toggle();
//             Rx_data[0]='0';
//         }
//     }
// }