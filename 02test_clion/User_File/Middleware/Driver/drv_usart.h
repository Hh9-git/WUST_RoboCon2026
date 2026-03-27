#ifndef DRV_USART_H
#define DRV_USART_H

#include "main.h"

// UART define
#define PRINT_UART huart3

//-----------------------------------------------------------------------------------------------------------------
// 如果启用UART功能
#ifdef HAL_UART_MODULE_ENABLED

#include "usart.h"

#define WAIT_DELAY 100 // 等待时间1ms

    typedef struct
    {
        UART_HandleTypeDef *huart;
        uint8_t *UART_RxBuf;
        uint8_t UART_RxBuf_Size;
        void (*UART_Callback)(uint8_t *pData, uint8_t size);
    } UART_Interrupt_t;

extern int UART_Print(char *fmt, ...);
extern void AttachInterrupt_UART(UART_HandleTypeDef *huart, uint8_t RxBuf_Size, void (*UART_Callback)(uint8_t *pData, uint8_t size));

typedef struct
{
    UART_HandleTypeDef *huart;
    uint8_t *UART_RxBuf;
    uint8_t UART_RxBuf_Size;
    void (*UART_Callback)(void);
} UART_DMA_Interrupt_t;
extern void AttachInterrupt_UART_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint8_t RxBuf_Size, void (*UART_Callback)(void));

#endif /* HAL_UART_MODULE_ENABLED */

#endif


