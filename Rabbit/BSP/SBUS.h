/**
 * SBUS协议解析
 * 波特率设置100000，需要反向电平，stm32 uart数据位设置9位
 */

#ifndef SBUS_H
#define SBUS_H

#include "BSP.h"

#define SBUS_UART huart1

extern volatile uint16_t sbus_channels[16];

void SBUS_Init(void (*SBUS_Function)(void));

#endif
