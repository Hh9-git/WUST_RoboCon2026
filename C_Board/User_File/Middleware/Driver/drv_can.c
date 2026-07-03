#include "drv_can.h"

//----------------------BSP_CAN----------------------//
#ifdef HAL_CAN_MODULE_ENABLED

static CAN_TxHeaderTypeDef CAN_TxHeader;

void CAN_Transmit(CAN_HandleTypeDef *hcan, uint16_t ID, uint8_t *Buf)
{
    if (Buf != NULL)
    {
        CAN_TxHeader.StdId = ID;
        CAN_TxHeader.IDE = CAN_ID_STD;
        CAN_TxHeader.RTR = CAN_RTR_DATA;
        CAN_TxHeader.DLC = 8;

        if (HAL_CAN_AddTxMessage(hcan, &CAN_TxHeader, Buf, (uint32_t *)CAN_TX_MAILBOX0) != HAL_OK)
        {
            if (HAL_CAN_AddTxMessage(hcan, &CAN_TxHeader, Buf, (uint32_t *)CAN_TX_MAILBOX1) != HAL_OK)
            {
                HAL_CAN_AddTxMessage(hcan, &CAN_TxHeader, Buf, (uint32_t *)CAN_TX_MAILBOX2);
            }
        }
    }
}
#endif /* HAL_CAN_MODULE_ENABLED */

//--------------------------------------------------------------------------------------------------------------------
// CAN Interrupt
#ifdef HAL_CAN_MODULE_ENABLED

static uint8_t CAN_Function_Count = 0;      // 函数数量
static CAN_Interrupt_t *CAN_ItSource_Array; // CAN中断回调函数结构体数组指针

void AttachInterrupt_CAN(CAN_HandleTypeDef *hcan, void (*CAN_Callback)(CAN_RxHeaderTypeDef *pHeader, uint8_t *pBuf))
{
    CAN_ItSource_Array = (CAN_Interrupt_t *)realloc(CAN_ItSource_Array, (CAN_Function_Count + 1) * sizeof(CAN_Interrupt_t));

    CAN_ItSource_Array[CAN_Function_Count].hcan = hcan;
    CAN_ItSource_Array[CAN_Function_Count].CAN_Callback = CAN_Callback;
    CAN_Function_Count++;

    CAN_FilterTypeDef CAN_FilterInitStructure;
    CAN_FilterInitStructure.FilterActivation = ENABLE;
    CAN_FilterInitStructure.FilterMode = CAN_FILTERMODE_IDMASK;
    CAN_FilterInitStructure.FilterScale = CAN_FILTERSCALE_32BIT;
    CAN_FilterInitStructure.FilterIdHigh = 0x0000;
    CAN_FilterInitStructure.FilterIdLow = 0x0000;
    CAN_FilterInitStructure.FilterMaskIdHigh = 0x0000;
    CAN_FilterInitStructure.FilterMaskIdLow = 0x0000;
    CAN_FilterInitStructure.FilterBank = (hcan->Instance == CAN1) ? 0 : 14;
    CAN_FilterInitStructure.SlaveStartFilterBank = 14;
    CAN_FilterInitStructure.FilterFIFOAssignment = CAN_RX_FIFO0;
    HAL_CAN_ConfigFilter(hcan, &CAN_FilterInitStructure);
    HAL_CAN_Start(hcan);
    HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
}

// CAN中断回调
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef CAN_RxHeader;
    uint8_t CAN_RxBuf[8];
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &CAN_RxHeader, CAN_RxBuf);

    for (uint8_t i = 0; i < CAN_Function_Count; i++)
    {
        if (CAN_ItSource_Array[i].hcan == hcan)
        {
            CAN_ItSource_Array[i].CAN_Callback(&CAN_RxHeader, CAN_RxBuf);
        }
    }
}
#endif /* HAL_CAN_MODULE_ENABLED */