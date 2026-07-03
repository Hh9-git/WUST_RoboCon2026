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
#define NOTE_D5  587  // 高音Re
#define NOTE_E5  659  // 高音Mi
#define NOTE_F5  698  // 高音Fa
#define NOTE_G5  784  // 高音Sol
#define NOTE_A5  880  // 高音La
#define NOTE_B5  988  // 高音Si
#define NOTE_C6  1047 // 超高音Do
#define NOTE_REST 0   // 休止符

/* 音符结构: 频率 + 时长 */
typedef struct
{
    uint16_t freq;
    uint16_t duration_ms;
} Buzzer_Note_t;

void Buzzer_Init(void);
void Buzzer_SetTone(uint16_t freq);
void Buzzer_on(uint16_t psc, uint16_t pwm);
void Buzzer_off(void);
void Buzzer_Beep(uint16_t freq, uint16_t time_ms);
void Buzzer_ShortBeep(void);
void Buzzer_LongBeep(void);
void Buzzer_DoubleBeep(void);

/* 播放一段旋律, notes为音符数组, count为音符数量, gap_ms为音符间隔 */
void Buzzer_PlayMelody(const Buzzer_Note_t *notes, uint16_t count, uint16_t gap_ms);

/* 预设提示音 */
void Buzzer_Startup(void);   // 开机提示音
void Buzzer_Alert(void);     // 警告提示音
void Buzzer_Success(void);   // 成功提示音

#endif