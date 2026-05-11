#ifndef DVC_BUZZER_H
#define DVC_BUZZER_H

#include "main.h"
void Buzzer_Init(void);
void Buzzer_on(uint16_t psc, uint16_t pwm);
void Buzzer_off(void);

#endif //DVC_BUZZER_H