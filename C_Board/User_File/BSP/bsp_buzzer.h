#ifndef BSP_BUZZER_H
#define BSP_BUZZER_H

#include "main.h"

extern TIM_HandleTypeDef htim4;
#define Buzzer_TIM  &htim4
#define Buzzer_CHANNEL TIM_CHANNEL_3

// 常用音阶频率（Hz）
#define NOTE_C4  262  // 中音Do
#define NOTE_D4  294  // 中音Re
#define NOTE_E4  330  // 中音Mi
#define NOTE_F4  349  // 中音Fa
#define NOTE_G4  392  // 中音Sol
#define NOTE_A4  440  // 中音La
#define NOTE_B4  494  // 中音Si
#define NOTE_C5  523  // 高音Do

void Buzzer_Init(void);
void Buzzer_SetTone(uint16_t freq);
void Buzzer_on(uint16_t psc, uint16_t pwm);
void Buzzer_off(void);
void Buzzer_Beep(uint16_t freq, uint16_t time_ms);
void Buzzer_ShortBeep(void);
void Buzzer_LongBeep(void);
void Buzzer_DoubleBeep(void);

#endif