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


#endif //DVC_VOFA_H