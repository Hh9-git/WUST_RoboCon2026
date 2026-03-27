#include "BSP.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

//----------------------BSP_UART----------------------//
#ifdef HAL_UART_MODULE_ENABLED

char UART_TxBuf[128];

// 串口打印
int UART_Print(char *fmt, ...)
{
    int ret;
    va_list ap;
    va_start(ap, fmt);
    ret = vsprintf(UART_TxBuf, fmt, ap);
    va_end(ap);
    if (ret > 0)
    {
        assert_param(HAL_UART_Transmit(&PRINT_UART, (uint8_t *)UART_TxBuf, ret, WAIT_DELAY) == HAL_OK);
    }
    return ret;
}

#endif /* HAL_UART_MODULE_ENABLED */

//----------------------BSP_USB----------------------//
#ifdef HAL_PCD_MODULE_ENABLED

char USB_TxBuf[128];

// USB串口打印
int USB_Print(char *fmt, ...)
{
    int ret;
    va_list ap;
    va_start(ap, fmt);
    ret = vsprintf(USB_TxBuf, fmt, ap);
    va_end(ap);
    if (ret > 0)
    {
        assert_param(CDC_Transmit_FS((uint8_t *)USB_TxBuf, ret) == USBD_OK);
    }
    return ret;
}

/* USB接收回调函数, 须在usbd_cdc_if.c中的CDC_Receive_FS函数中调用 */
__weak void USB_Rx_Callback(uint8_t *pData, uint8_t size)
{
    UNUSED(pData);
    UNUSED(size);
}

#endif /* HAL_PCD_MODULE_ENABLED */

//----------------------BSP_CAN----------------------//
#ifdef HAL_CAN_MODULE_ENABLED

static CAN_TxHeaderTypeDef CAN_TxHeader;
static uint32_t CAN_TxMailbox = 0;

// 把can的buf中的内容发送出去, 长度为8
void CAN_Transmit(CAN_HandleTypeDef *hcan, uint16_t ID, uint8_t *Buf)
{

    if ((Buf != NULL))
    {
        CAN_TxHeader.StdId = ID;         /* 指定标准标识符，该值在0x00-0x7FF */
        CAN_TxHeader.IDE = CAN_ID_STD;   /* 指定将要传输消息的标识符类型 */
        CAN_TxHeader.RTR = CAN_RTR_DATA; /* 指定消息传输帧类型 */
        CAN_TxHeader.DLC = 8;            /* 指定将要传输的帧长度 */

        assert_param(HAL_CAN_AddTxMessage(hcan, &CAN_TxHeader, Buf, &CAN_TxMailbox) == HAL_OK);
    }
}

#endif /* HAL_CAN_MODULE_ENABLED */

//----------------------BSP_FDCAN----------------------//
#ifdef HAL_FDCAN_MODULE_ENABLED

static FDCAN_TxHeaderTypeDef FDCAN_TxHeader;

void FDCAN_Transmit(FDCAN_HandleTypeDef *hfdcan, uint16_t ID, uint8_t *Buf)
{
    if ((Buf != NULL))
    {
        FDCAN_TxHeader.Identifier = ID;                         // 目标ID
        FDCAN_TxHeader.IdType = FDCAN_STANDARD_ID;              // 标准ID
        FDCAN_TxHeader.TxFrameType = FDCAN_DATA_FRAME;          // 数据帧格式
        FDCAN_TxHeader.DataLength = FDCAN_DLC_BYTES_8;          // 数据长度
        FDCAN_TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;  //
        FDCAN_TxHeader.BitRateSwitch = FDCAN_BRS_OFF;           // 关闭速率切换
        FDCAN_TxHeader.FDFormat = FDCAN_CLASSIC_CAN;            // 传统CAN模式
        FDCAN_TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS; // 无发送事件
        FDCAN_TxHeader.MessageMarker = 0;                       //

        assert_param(HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &FDCAN_TxHeader, Buf) == HAL_OK);
    }
}
#endif /* HAL_FDCAN_MODULE_ENABLED */

//----------------------BSP_Interrupt----------------------//

//--------------------------------------------------------------------------------------------------------------------
// GPIO Interrupt
#ifdef HAL_GPIO_MODULE_ENABLED

static uint8_t GPIO_Function_Count = 0;       // 函数数量
static GPIO_Interrupt_t *GPIO_ItSource_Array; // GPIO中断回调函数结构体数组指针

/**
 * 联接GPIO中断源和中断回调函数
 * 应输入GPIO引脚, 回调函数个数以及回调函数的地址
 * 联接后自动开启中断
 */
void AttachInterrupt_GPIO(uint16_t GPIO_Pin, void (*GPIO_Callback)(void))
{
    GPIO_ItSource_Array = (GPIO_Interrupt_t *)realloc(GPIO_ItSource_Array, (GPIO_Function_Count + 1) * sizeof(GPIO_Interrupt_t));
    GPIO_ItSource_Array[GPIO_Function_Count].GPIO_Pin = GPIO_Pin;
    GPIO_ItSource_Array[GPIO_Function_Count].GPIO_Callback = GPIO_Callback;
    GPIO_Function_Count++;
}

// GPIO中断回调
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    for (uint8_t i = 0; i < GPIO_Function_Count; i++)
    {
        if (GPIO_ItSource_Array[i].GPIO_Pin == GPIO_Pin)
        {
            GPIO_ItSource_Array[i].GPIO_Callback();
        }
    }
}

#endif /* HAL_GPIO_MODULE_ENABLED */

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
    assert_param(HAL_UARTEx_ReceiveToIdle_IT(huart, UART_ItSource_Array[UART_Function_Count].UART_RxBuf, RxBuf_Size) == HAL_OK);
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
            assert_param(HAL_UARTEx_ReceiveToIdle_IT(huart, UART_ItSource_Array[i].UART_RxBuf, UART_ItSource_Array[i].UART_RxBuf_Size) == HAL_OK);
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
    assert_param(HAL_UART_Receive_DMA(huart, UART_DMA_ItSource_Array[UART_DMA_Function_Count].UART_RxBuf, RxBuf_Size) == HAL_OK);
    // assert_param(HAL_UART_Receive_IT(huart, UART_DMA_ItSource_Array[UART_DMA_Function_Count].UART_RxBuf, RxBuf_Size) == HAL_OK);
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
            assert_param(HAL_UART_Receive_DMA(huart, UART_DMA_ItSource_Array[i].UART_RxBuf, UART_DMA_ItSource_Array[i].UART_RxBuf_Size) == HAL_OK);
            // assert_param(HAL_UART_Receive_IT(huart, UART_DMA_ItSource_Array[i].UART_RxBuf, UART_DMA_ItSource_Array[i].UART_RxBuf_Size) == HAL_OK);
        }
    }
}

#endif /* HAL_UART_MODULE_ENABLED */

//--------------------------------------------------------------------------------------------------------------------
// CAN Interrupt
#ifdef HAL_CAN_MODULE_ENABLED

static CAN_RxHeaderTypeDef CAN_RxHeader;
static uint8_t CAN_RxBuf[8];

static uint8_t CAN_Function_Count = 0;      // 函数数量
static CAN_Interrupt_t *CAN_ItSource_Array; // CAN中断回调函数结构体数组指针

/**
 * CAN过滤器配置
 * 如果使用了AttachInterrupt_CAN函数则无需再使用
 */
inline void CAN_FilterInit(void)
{
    CAN_FilterTypeDef CAN_FilterInitStructure;
    CAN_FilterInitStructure.FilterActivation = ENABLE;
    CAN_FilterInitStructure.FilterMode = CAN_FILTERMODE_IDMASK;
    CAN_FilterInitStructure.FilterScale = CAN_FILTERSCALE_32BIT;
    CAN_FilterInitStructure.FilterIdHigh = 0x0000;
    CAN_FilterInitStructure.FilterIdLow = 0x0000;
    CAN_FilterInitStructure.FilterMaskIdHigh = 0x0000;
    CAN_FilterInitStructure.FilterMaskIdLow = 0x0000;
    CAN_FilterInitStructure.FilterFIFOAssignment = CAN_RX_FIFO0;
    CAN_FilterInitStructure.FilterBank = 0;
    HAL_CAN_ConfigFilter(&hcan1, &CAN_FilterInitStructure);
    assert_param(HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) == HAL_OK);
    assert_param(HAL_CAN_Start(&hcan1) == HAL_OK);

    CAN_FilterInitStructure.FilterBank = 0;
    CAN_FilterInitStructure.SlaveStartFilterBank = 0;
    HAL_CAN_ConfigFilter(&hcan2, &CAN_FilterInitStructure);
    assert_param(HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING) == HAL_OK);
    assert_param(HAL_CAN_Start(&hcan2) == HAL_OK);
}

/**
 * 联接CAN中断源和中断回调函数
 * 应输入CAN源, 回调函数个数以及回调函数的地址
 * 联接后自动开启中断
 */
void AttachInterrupt_CAN(CAN_HandleTypeDef *hcan, void (*CAN_Callback)(CAN_RxHeaderTypeDef *pHeader, uint8_t *pBuf))
{
    CAN_ItSource_Array = (CAN_Interrupt_t *)realloc(CAN_ItSource_Array, (CAN_Function_Count + 1) * sizeof(CAN_Interrupt_t));
    CAN_ItSource_Array[CAN_Function_Count].hcan = hcan;
    CAN_ItSource_Array[CAN_Function_Count].CAN_Callback = CAN_Callback;
    CAN_Function_Count++;
}

// CAN中断回调
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    assert_param(HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &CAN_RxHeader, CAN_RxBuf) == HAL_OK);

    for (uint8_t i = 0; i < CAN_Function_Count; i++)
    {
        if (CAN_ItSource_Array[i].hcan == hcan)
        {
            CAN_ItSource_Array[i].CAN_Callback(&CAN_RxHeader, CAN_RxBuf);
        }
    }
}

#endif /* HAL_CAN_MODULE_ENABLED */

//--------------------------------------------------------------------------------------------------------------------
// FDCAN Interrupt
#ifdef HAL_FDCAN_MODULE_ENABLED

static FDCAN_RxHeaderTypeDef FDCAN_RxHeader;
static uint8_t FDCAN_RxBuf[8];

static uint8_t FDCAN_Function_Count = 0;        // 函数数量
static FDCAN_Interrupt_t *FDCAN_ItSource_Array; // CAN中断回调函数结构体数组指针

/**
 * FDCAN过滤器配置
 * 如果使用了AttachInterrupt_FDCAN函数则无需再使用
 */
inline void FDCAN_FilterInit(FDCAN_HandleTypeDef *hfdcan)
{
    FDCAN_FilterTypeDef FDCAN_FilterInitStructure;
    FDCAN_FilterInitStructure.IdType = FDCAN_STANDARD_ID;             // 标准ID
    FDCAN_FilterInitStructure.FilterIndex = 0;                        // 过滤器索引
    FDCAN_FilterInitStructure.FilterType = FDCAN_FILTER_MASK;         // 过滤器类型
    FDCAN_FilterInitStructure.FilterConfig = FDCAN_FILTER_TO_RXFIFO0; // 过滤器0关联到FIFO0
    FDCAN_FilterInitStructure.FilterID1 = 0x000;                      // 11位ID
    FDCAN_FilterInitStructure.FilterID2 = 0x7FF;                      // 11位掩码
    HAL_FDCAN_ConfigFilter(hfdcan, &FDCAN_FilterInitStructure);
    assert_param(HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) == HAL_OK); // 使能新消息接收中断
    assert_param(HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_TX_EVT_FIFO_ELT_LOST, 0) == HAL_OK);
    assert_param(HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RAM_ACCESS_FAILURE, 0) == HAL_OK);
    assert_param(HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_DATA_PROTOCOL_ERROR, 0) == HAL_OK);
    assert_param(HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_ARB_PROTOCOL_ERROR, 0) == HAL_OK);
    assert_param(HAL_FDCAN_Start(hfdcan) == HAL_OK);
}

// 联接CAN中断源和中断回调函数
void AttachInterrupt_FDCAN(FDCAN_HandleTypeDef *hfdcan, void (*FDCAN_Callback)(FDCAN_RxHeaderTypeDef *pHeader, uint8_t *pBuf))
{
    FDCAN_ItSource_Array = (FDCAN_Interrupt_t *)realloc(FDCAN_ItSource_Array, (FDCAN_Function_Count + 1) * sizeof(FDCAN_Interrupt_t));
    FDCAN_ItSource_Array[FDCAN_Function_Count].hfdcan = hfdcan;
    FDCAN_ItSource_Array[FDCAN_Function_Count].FDCAN_Callback = FDCAN_Callback;
    FDCAN_Function_Count++;
}

// FDCAN中断回调
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET)
    {
        assert_param(HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &FDCAN_RxHeader, FDCAN_RxBuf) == HAL_OK);

        for (uint8_t i = 0; i < FDCAN_Function_Count; i++)
        {
            if (FDCAN_ItSource_Array[i].hfdcan == hfdcan)
            {
                FDCAN_ItSource_Array[i].FDCAN_Callback(&FDCAN_RxHeader, FDCAN_RxBuf);
            }
        }
    }
}

void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs)
{
    if ((RxFifo1ITs & FDCAN_IT_RX_FIFO1_NEW_MESSAGE) != RESET)
    {
        assert_param(HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &FDCAN_RxHeader, FDCAN_RxBuf) == HAL_OK);

        for (uint8_t i = 0; i < FDCAN_Function_Count; i++)
        {
            if (FDCAN_ItSource_Array[i].hfdcan == hfdcan)
            {
                FDCAN_ItSource_Array[i].FDCAN_Callback(&FDCAN_RxHeader, FDCAN_RxBuf);
            }
        }
    }
}

#endif /* HAL_FDCAN_MODULE_ENABLED */

//-------------------------------------------------------------------------------------------------------------------
// TIM Interrupt
#ifdef HAL_TIM_MODULE_ENABLED

static uint8_t TIM_Function_Count = 0;      // 函数数量
static TIM_Interrupt_t *TIM_ItSource_Array; // TIM中断回调函数结构体数组指针

/**
 * 联接TIM中断源和中断回调函数
 * 应输入TIM源, 回调函数个数以及回调函数的地址
 * 联接后会自动开启中断
 */
void AttachInterrupt_TIM(TIM_HandleTypeDef *htim, void (*TIM_Callback)(void))
{
    TIM_ItSource_Array = (TIM_Interrupt_t *)realloc(TIM_ItSource_Array, (TIM_Function_Count + 1) * sizeof(TIM_Interrupt_t));
    TIM_ItSource_Array[TIM_Function_Count].htim = htim;
    TIM_ItSource_Array[TIM_Function_Count].TIM_Callback = TIM_Callback;
    TIM_Function_Count++;
}

/**
 * TIM中断回调
 * 如果更改了系统时钟源，需要在main.c文件中调用此函数
 */
inline void BSP_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    for (uint8_t i = 0; i < TIM_Function_Count; i++)
    {
        if (TIM_ItSource_Array[i].htim == htim)
        {
            TIM_ItSource_Array[i].TIM_Callback();
        }
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    BSP_TIM_PeriodElapsedCallback(htim);
}

#endif /* HAL_TIM_MODULE_ENABLED */
