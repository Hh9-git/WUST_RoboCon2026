#include "drv_usart.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>


#ifdef HAL_UART_MODULE_ENABLED

char UART_TxBuf[128];

// 串口打印 UART_Print("123456\r\n");格式需要加\r
int UART_Print(char *fmt, ...)
{
    int ret;
    va_list ap;
    va_start(ap, fmt);
    ret = vsprintf(UART_TxBuf, fmt, ap);
    va_end(ap);
    // if (ret > 0)
    // {
    //     assert_param(HAL_UART_Transmit(&PRINT_UART, (uint8_t *)UART_TxBuf, ret, WAIT_DELAY) == HAL_OK);
    // }
    HAL_UART_Transmit(&PRINT_UART, (uint8_t *)UART_TxBuf, ret, WAIT_DELAY);
    return ret;
}

#endif /* HAL_UART_MODULE_ENABLED */


//--------------------------------------------------------------------------------------------------------------------
// UART Interrupt
#ifdef HAL_UART_MODULE_ENABLED

static uint8_t UART_Function_Count = 0;       // 函数数量
static UART_Interrupt_t *UART_ItSource_Array; // UART中断回调函数结构体数组指针


/**
 * 联接UART中断源和中断回调函数
 * 应输入UART源, 回调函数个数以及回调函数的地址
 * 联接后自动开启中断
 */
void AttachInterrupt_UART(UART_HandleTypeDef *huart, uint8_t RxBuf_Size, void (*UART_Callback)(uint8_t *pData, uint8_t size))
{
    UART_ItSource_Array = (UART_Interrupt_t *)realloc(UART_ItSource_Array, (UART_Function_Count + 1) * sizeof(UART_Interrupt_t));
    UART_ItSource_Array[UART_Function_Count].UART_RxBuf_Size = RxBuf_Size;
    UART_ItSource_Array[UART_Function_Count].UART_RxBuf = (uint8_t *)malloc(RxBuf_Size * sizeof(uint8_t));
    UART_ItSource_Array[UART_Function_Count].huart = huart;
    UART_ItSource_Array[UART_Function_Count].UART_Callback = UART_Callback;
    // assert_param(HAL_UARTEx_ReceiveToIdle_IT(huart, UART_ItSource_Array[UART_Function_Count].UART_RxBuf, RxBuf_Size) == HAL_OK);
    HAL_UARTEx_ReceiveToIdle_IT(huart, UART_ItSource_Array[UART_Function_Count].UART_RxBuf, RxBuf_Size);
    UART_Function_Count++;
}

// UART空闲中断回调
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    for (uint8_t i = 0; i < UART_Function_Count; i++)
    {
        if (UART_ItSource_Array[i].huart == huart)
        {
            UART_ItSource_Array[i].UART_Callback(UART_ItSource_Array[i].UART_RxBuf, Size);
            HAL_UARTEx_ReceiveToIdle_IT(huart, UART_ItSource_Array[i].UART_RxBuf, UART_ItSource_Array[i].UART_RxBuf_Size);
        }
    }
}



static uint8_t UART_DMA_Function_Count = 0;           // DMA函数数量
static UART_DMA_Interrupt_t *UART_DMA_ItSource_Array; // UART中断回调函数结构体数组指针

/**
 * 联接UART DMA和中断回调函数
 * 应输入UART源, 回调函数个数以及回调函数的地址
 * 联接后自动开启DMA
 */
void AttachInterrupt_UART_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint8_t RxBuf_Size, void (*UART_Callback)(void))
{
    UART_DMA_ItSource_Array = (UART_DMA_Interrupt_t *)realloc(UART_DMA_ItSource_Array, (UART_DMA_Function_Count + 1) * sizeof(UART_DMA_Interrupt_t));
    UART_DMA_ItSource_Array[UART_DMA_Function_Count].huart = huart;
    UART_DMA_ItSource_Array[UART_DMA_Function_Count].UART_RxBuf = pData;
    UART_DMA_ItSource_Array[UART_DMA_Function_Count].UART_RxBuf_Size = RxBuf_Size;
    UART_DMA_ItSource_Array[UART_DMA_Function_Count].UART_Callback = UART_Callback;
    // assert_param(HAL_UART_Receive_DMA(huart, UART_DMA_ItSource_Array[UART_DMA_Function_Count].UART_RxBuf, RxBuf_Size) == HAL_OK);
    // assert_param(HAL_UART_Receive_IT(huart, UART_DMA_ItSource_Array[UART_DMA_Function_Count].UART_RxBuf, RxBuf_Size) == HAL_OK);
    HAL_UART_Receive_DMA(huart, UART_DMA_ItSource_Array[UART_DMA_Function_Count].UART_RxBuf, RxBuf_Size);
    UART_DMA_Function_Count++;
}

// UART完成中断
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    for (uint8_t i = 0; i < UART_DMA_Function_Count; i++)
    {
        if (UART_DMA_ItSource_Array[i].huart == huart)
        {
            UART_DMA_ItSource_Array[i].UART_Callback();
            // assert_param(HAL_UART_Receive_DMA(huart, UART_DMA_ItSource_Array[i].UART_RxBuf, UART_DMA_ItSource_Array[i].UART_RxBuf_Size) == HAL_OK);
            // assert_param(HAL_UART_Receive_IT(huart, UART_DMA_ItSource_Array[i].UART_RxBuf, UART_DMA_ItSource_Array[i].UART_RxBuf_Size) == HAL_OK);
            HAL_UART_Receive_DMA(huart, UART_DMA_ItSource_Array[i].UART_RxBuf, UART_DMA_ItSource_Array[i].UART_RxBuf_Size);
        }
    }
}


#endif /* HAL_UART_MODULE_ENABLED */
