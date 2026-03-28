/**
 * 1.version:3.1 data:2022/11/25
 * 2.本文件可实现UART配置、CAN配置、中断与中断回调函数自由绑定
 */

#ifndef __BSP_H
#define __BSP_H

#include "main.h"

// UART define
#define PRINT_UART huart7

//-----------------------------------------------------------------
// 如果启用GPIO中断功能
#ifdef HAL_GPIO_MODULE_ENABLED

typedef struct
{
    uint16_t GPIO_Pin;
    void (*GPIO_Callback)(void);

} GPIO_Interrupt_t;

/**
 * 联接GPIO中断源和中断回调函数
 * 应输入GPIO引脚, 回调函数个数以及回调函数的地址
 * 联接后自动开启中断
 */
void AttachInterrupt_GPIO(uint16_t GPIO_Pin, void (*GPIO_EXTI)(void));
#endif /* HAL_GPIO_MODULE_ENABLED */

//-----------------------------------------------------------------
// 如果启用UART功能
#ifdef HAL_UART_MODULE_ENABLED
#include "usart.h"

typedef struct
{
    UART_HandleTypeDef *huart;
    uint8_t *UART_RxBuf;
    uint8_t UART_RxBuf_Size;
    void (*UART_Callback)(uint8_t *pData, uint8_t size);

} UART_Interrupt_t;

// 串口打印
int UART_Print(char *fmt, ...);
/**
 * 联接UART中断源和中断回调函数
 * 应输入UART源, 回调函数个数以及回调函数的地址
 * 联接后自动开启中断
 */
void AttachInterrupt_UART(UART_HandleTypeDef *huart, uint8_t RxBuf_Size, void (*UART_Callback)(uint8_t *pData, uint8_t size));
#endif /* HAL_UART_MODULE_ENABLED */

//-----------------------------------------------------------------
// 如果启用CAN功能
#ifdef HAL_CAN_MODULE_ENABLED
#include "can.h"

typedef struct
{
    CAN_HandleTypeDef *hcan;
    void (*CAN_Callback)(CAN_RxHeaderTypeDef *pHeader, uint8_t *pBuf);

} CAN_Interrupt_t;

// 把can的buf中的内容发送出去, 长度为8
void CAN_Transmit(CAN_HandleTypeDef *hcan, uint16_t ID, uint8_t *Buf);
/**
 * 联接CAN中断源和中断回调函数
 * 应输入CAN源, 回调函数个数以及回调函数的地址
 * 联接后自动开启中断
 */
void AttachInterrupt_CAN(CAN_HandleTypeDef *hcan, void (*CAN_Callback)(CAN_RxHeaderTypeDef *pHeader, uint8_t *pBuf));
#endif /* HAL_CAN_MODULE_ENABLED */

//-----------------------------------------------------------------
// 如果启用CAN功能
#ifdef HAL_FDCAN_MODULE_ENABLED
#include "fdcan.h"

typedef struct
{
    FDCAN_HandleTypeDef *hfdcan;
    void (*FDCAN_Callback)(FDCAN_RxHeaderTypeDef *pHeader, uint8_t *pBuf);

} FDCAN_Interrupt_t;

// 把fdcan的buf中的内容发送出去, 长度为8
void FDCAN_Transmit(FDCAN_HandleTypeDef *hfdcan, uint16_t ID, uint8_t *Buf);
/**
 * 联接CAN中断源和中断回调函数
 * 应输入CAN源, 回调函数个数以及回调函数的地址
 * 联接后自动开启中断
 */
void AttachInterrupt_FDCAN(FDCAN_HandleTypeDef *hfdcan, void (*FDCAN_Callback)(FDCAN_RxHeaderTypeDef *pHeader, uint8_t *pBuf));
#endif /* HAL_FDCAN_MODULE_ENABLED */

//-----------------------------------------------------------------
// 如果启用TIM base功能
#ifdef HAL_TIM_MODULE_ENABLED

typedef struct
{
    TIM_HandleTypeDef *htim;
    void (*TIM_Callback)(void);

} TIM_Interrupt_t;

/**
 * 联接TIM中断源和中断回调函数
 * 应输入TIM源, 回调函数个数以及回调函数的地址
 * 联接后自动开启中断
 */
void AttachInterrupt_TIM(TIM_HandleTypeDef *htim, void (*TIM_Callback)(void));
#endif /* HAL_TIM_MODULE_ENABLED */

#endif
