/**
 * 1.version:4.8 data:2023/5/23
 * 2.本文件可实现UART配置、CAN配置、GPIO中断与TIM中断回调函数自由绑定
 */

#ifndef BSP_H
#define BSP_H

#ifdef __cplusplus
extern "C "
{
#endif

#include "main.h"

#define _PI 3.14159265358979f
/* 角度转弧度 */
#define RAD(deg) ((deg)*_PI / 180.0f)
/* 弧度转角度 */
#define DEG(rad) ((rad)*180.0f / _PI)

// UART define
#define PRINT_UART huart8

//----------------------------------------------------------------------------------------------------------------
// 如果启用GPIO中断功能
#ifdef HAL_GPIO_MODULE_ENABLED

    typedef struct
    {
        uint16_t GPIO_Pin;
        void (*GPIO_Callback)(void);
    } GPIO_Interrupt_t;

    extern void AttachInterrupt_GPIO(uint16_t GPIO_Pin, void (*GPIO_EXTI)(void));

#endif /* HAL_GPIO_MODULE_ENABLED */

//-----------------------------------------------------------------------------------------------------------------
// 如果启用UART功能
#ifdef HAL_UART_MODULE_ENABLED

#include "usart.h"

#define WAIT_DELAY 10000 // 等待时间100ms

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

//-----------------------------------------------------------------------------------------------------------------
// 如果启用UART功能
#ifdef HAL_PCD_MODULE_ENABLED

#include "usbd_cdc_if.h"

    extern int USB_Print(char *fmt, ...);
    extern void USB_Rx_Callback(uint8_t *pData, uint8_t size);

#endif /* HAL_PCD_MODULE_ENABLED */

//-------------------------------------------------------------------------------------------------------------------
// 如果启用CAN功能
#ifdef HAL_CAN_MODULE_ENABLED

#include "can.h"

    typedef struct
    {
        CAN_HandleTypeDef *hcan;
        void (*CAN_Callback)(CAN_RxHeaderTypeDef *pHeader, uint8_t *pBuf);
    } CAN_Interrupt_t;

    extern void CAN_FilterInit(void);
    extern void CAN_Transmit(CAN_HandleTypeDef *hcan, uint16_t ID, uint8_t *Buf);
    extern void AttachInterrupt_CAN(CAN_HandleTypeDef *hcan, void (*CAN_Callback)(CAN_RxHeaderTypeDef *pHeader, uint8_t *pBuf));

#endif /* HAL_CAN_MODULE_ENABLED */

//---------------------------------------------------------------------------------------------------------------------
// 如果启用FDCAN功能
#ifdef HAL_FDCAN_MODULE_ENABLED
#include "fdcan.h"

    typedef struct
    {
        FDCAN_HandleTypeDef *hfdcan;
        void (*FDCAN_Callback)(FDCAN_RxHeaderTypeDef *pHeader, uint8_t *pBuf);

    } FDCAN_Interrupt_t;

    extern void FDCAN_FilterInit(FDCAN_HandleTypeDef *hfdcan);
    extern void FDCAN_Transmit(FDCAN_HandleTypeDef *hfdcan, uint16_t ID, uint8_t *Buf);
    extern void AttachInterrupt_FDCAN(FDCAN_HandleTypeDef *hfdcan, void (*FDCAN_Callback)(FDCAN_RxHeaderTypeDef *pHeader, uint8_t *pBuf));
#endif /* HAL_FDCAN_MODULE_ENABLED */

//-----------------------------------------------------------------------------------------------------------------------
// 如果启用TIM base功能
#ifdef HAL_TIM_MODULE_ENABLED
#include "tim.h"

    typedef struct
    {
        TIM_HandleTypeDef *htim;
        void (*TIM_Callback)(void);
    } TIM_Interrupt_t;

    extern void AttachInterrupt_TIM(TIM_HandleTypeDef *htim, void (*TIM_Callback)(void));
    extern void BSP_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#endif /* HAL_TIM_MODULE_ENABLED */

#ifdef __cplusplus
}
#endif

#endif
