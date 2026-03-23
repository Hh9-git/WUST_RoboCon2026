//
// Created by BINXUAN on 2026/3/17.
//

#ifndef DVC_VOFA_H
#define DVC_VOFA_H

#include "stm32f4xx_hal.h"
#include "drv_usart.h"
#include <stdio.h>
#include "stdint.h"
#include <string.h>
#include <stdarg.h>
void Vofa_FireWater(const char *format, ...);
void Vofa_JustFloat(float *_data, uint8_t _num);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void justfloat_displaydata(float position_target,float position_actual,float position_out,float speed_target,float speed_actual,float speed_out);


#endif //DVC_VOFA_H