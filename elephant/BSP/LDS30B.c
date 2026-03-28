#include "LDS30B.h"
#include <stdlib.h>

static uint8_t lds_count;    // 激光测距数量
static LDS30B_t **lds_array; // 激光测距数组指针

void LDS30B_Callback(uint8_t *pData, uint8_t size)
{
    if (pData[1] == 0x03 && pData[2] == 0x04)
    {
        for (uint8_t i = 0; i < lds_count; i++)
        {
            if (lds_array[i]->ID == pData[0])
            {
                lds_array[i]->distance = ((uint32_t)(pData[3] << 24 | pData[4] << 16 | pData[5] << 8 | pData[6])) / 10.0f;
            }
        }
    }
}

void LDS30B_Init(LDS30B_t *lds, UART_HandleTypeDef *huart, uint8_t ID)
{
    lds_array = (LDS30B_t **)realloc(lds_array, (lds_count + 1) * sizeof(LDS30B_t *));
    lds_array[lds_count] = lds;
    lds_array[lds_count]->ID = ID;
    lds_count++;
    AttachInterrupt_UART(huart, 32, LDS30B_Callback);
}
