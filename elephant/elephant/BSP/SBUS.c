#include "SBUS.h"

#define SBUS_START 0x0F
#define SBUS_END 0x00
#define SBUS_SIZE 25
#define SBUS_MAXDELAY 1000

/* 程序从DTCM开始执行，但是不支持DMA访问，所以需要在AXI SRAM分配内存 */
uint8_t receiveData[SBUS_SIZE] __attribute__((section(".dma_data")));
uint16_t sbus_channels[16];
void (*RemoteFunction)(void);

void SBUS_Callback(void)
{
    if (receiveData[0] == SBUS_START && receiveData[24] == SBUS_END)
    {
        sbus_channels[0] = (uint16_t)((receiveData[1] | receiveData[2] << 8) & 0x07FF);
        sbus_channels[1] = (uint16_t)((receiveData[2] >> 3 | receiveData[3] << 5) & 0x07FF);
        sbus_channels[2] = (uint16_t)((receiveData[3] >> 6 | receiveData[4] << 2 | receiveData[5] << 10) & 0x07FF);
        sbus_channels[3] = (uint16_t)((receiveData[5] >> 1 | receiveData[6] << 7) & 0x07FF);
        sbus_channels[4] = (uint16_t)((receiveData[6] >> 4 | receiveData[7] << 4) & 0x07FF);
        sbus_channels[5] = (uint16_t)((receiveData[7] >> 7 | receiveData[8] << 1 | receiveData[9] << 9) & 0x07FF);
        sbus_channels[6] = (uint16_t)((receiveData[9] >> 2 | receiveData[10] << 6) & 0x07FF);
        sbus_channels[7] = (uint16_t)((receiveData[10] >> 5 | receiveData[11] << 3) & 0x07FF);
        sbus_channels[8] = (uint16_t)((receiveData[12] | receiveData[13] << 8) & 0x07FF);
        sbus_channels[9] = (uint16_t)((receiveData[13] >> 3 | receiveData[14] << 5) & 0x07FF);
        sbus_channels[10] = (uint16_t)((receiveData[14] >> 6 | receiveData[15] << 2 | receiveData[16] << 10) & 0x07FF);
        sbus_channels[11] = (uint16_t)((receiveData[16] >> 1 | receiveData[17] << 7) & 0x07FF);
        sbus_channels[12] = (uint16_t)((receiveData[17] >> 4 | receiveData[18] << 4) & 0x07FF);
        sbus_channels[13] = (uint16_t)((receiveData[18] >> 7 | receiveData[19] << 1 | receiveData[20] << 9) & 0x07FF);
        sbus_channels[14] = (uint16_t)((receiveData[20] >> 2 | receiveData[21] << 6) & 0x07FF);
        sbus_channels[15] = (uint16_t)((receiveData[21] >> 5 | receiveData[22] << 3) & 0x07FF);
    }
    else // 调整错误帧
    {
        for (uint8_t i = 1; i < SBUS_SIZE; i++)
        {
            if (receiveData[i] == SBUS_START && receiveData[i - 1] == SBUS_END)
            {
                assert_param(HAL_UART_Receive(&SBUS_UART, receiveData, i, SBUS_MAXDELAY) == HAL_OK);
                break;
            }
        }
    }

    RemoteFunction();
}

void SBUS_Init(void (*SBUS_Function)(void))
{
    RemoteFunction = SBUS_Function;

    assert_param(HAL_UART_Receive(&SBUS_UART, receiveData, SBUS_SIZE, SBUS_MAXDELAY) == HAL_OK);
    if (receiveData[0] != SBUS_START || receiveData[24] != SBUS_END)
    {
        for (uint8_t i = 1; i < SBUS_SIZE; i++)
        {
            if (receiveData[i] == SBUS_START && receiveData[i - 1] == SBUS_END)
            {
                assert_param(HAL_UART_Receive(&SBUS_UART, receiveData, i, SBUS_MAXDELAY) == HAL_OK);
                break;
            }
        }
    }

    AttachInterrupt_UART_DMA(&SBUS_UART, receiveData, SBUS_SIZE, SBUS_Callback);
}
