/**
 * 1.version:1.0 data:2023/6/7
 * 2.LDS30B激光测距
 */

#ifndef LDS30B_H
#define LDS30B_H

#include "BSP.h"

typedef struct
{
    uint8_t ID;
    float distance;
} LDS30B_t;
LDS30B_t lds1;
LDS30B_t lds2;
void LDS30B_Init(LDS30B_t *lds, UART_HandleTypeDef *huart, uint8_t ID);

#endif
