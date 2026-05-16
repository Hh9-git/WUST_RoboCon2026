#ifndef C_BOARD_DVC_SERVO_H
#define C_BOARD_DVC_SERVO_H

#include "tim.h"

void Servo_Init(void);
void Servo_SetAngle_135(uint32_t channel, int16_t angle);
void Servo_SetPulse(uint32_t channel, uint16_t us);
#endif //C_BOARD_DVC_SERVO_H