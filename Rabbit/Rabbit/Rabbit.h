#ifndef RABBIT_H
#define RABBIT_H

#include "BSP.h"
#include "Global.h"
#include "Remote.h"

void Rabbit_Init(void);
void Rabbit_Run(void);
extern uint8_t MM_flag;
extern uint8_t FF_flag;
extern float speedW;
extern uint16_t up_flag;
extern uint16_t down_flag;
extern float speedX;
extern float speedY;
extern float speedW;
extern float speedUD;
extern uint8_t shoot_up;

#endif
