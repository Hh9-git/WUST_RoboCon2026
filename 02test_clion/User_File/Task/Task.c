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
#include "gpio.h"
#include "usart.h"
#include "../Device/dvc_LED.h"
#include "drv_can.h"
#include "drv_usart.h"
#include "dvc_vofa.h"
#include "dvc_remote.h"
#include "stdio.h"
#include "stm32f4xx_hal.h"
#include <math.h>

// uint8_t Uart_Rxdata[128];
/* 程序从DTCM开始执行，但是不支持DMA访问，所以需要在AXI SRAM分配内存 */
uint8_t Uart_Rxdata[128] __attribute__((section(".dma_data")));

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

void Uart2_callback(void)
{
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

    AttachInterrupt_UART_DMA(&huart2, Uart_Rxdata, 128, Uart2_callback);
    // AttachInterrupt_UART(&huart3, 128, Serial_callback);
}


void Task_Loop()
{
    // LED_green_Toggle();
    // HAL_Delay(500);
    // UART_Print("123456\r\n");
    HAL_UART_Transmit(&huart2, (uint8_t *)"123456\r\n", 8, 100);


}
