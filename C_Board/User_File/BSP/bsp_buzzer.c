#include "bsp_buzzer.h"

// 8 个中音音阶：Do Re Mi Fa Sol La Si Do
uint16_t buzzer[8] = {
    NOTE_C4,   // 0：Do
    NOTE_D4,   // 1：Re
    NOTE_E4,   // 2：Mi
    NOTE_F4,   // 3：Fa
    NOTE_G4,   // 4：Sol
    NOTE_A4,   // 5：La
    NOTE_B4,   // 6：Si
    NOTE_C5    // 7：高音Do
};

void Buzzer_Init(void)
{
    //start tim
    HAL_TIM_Base_Start(Buzzer_TIM);
    //start pwm channel
    HAL_TIM_PWM_Start(Buzzer_TIM, TIM_CHANNEL_3);
}

// 设置蜂鸣器音调（freq=0停止发声）
void Buzzer_SetTone(uint16_t freq)
{
    // 功能：设置蜂鸣器音调
    // freq：频率值  100~20000Hz 都可以
    uint8_t PSC=84;
    if(freq == 0)
    {
        __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, 0); // 静音
        return;
    }

    uint32_t ARR = 84000000/84/freq;  // 计算重装载值2w122q
    __HAL_TIM_SET_PRESCALER(&htim4, PSC - 1);
    __HAL_TIM_SET_AUTORELOAD(&htim4, ARR-1);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, ARR/2); // 50%占空比

}

void Buzzer_off(void)
{
    __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_3, 0);
}

void Buzzer_Beep(uint16_t freq, uint16_t time_ms)
{
    Buzzer_SetTone(freq);
    HAL_Delay(time_ms);
    Buzzer_SetTone(0);
    HAL_Delay(50); // 间隔，防止粘连
}


void Buzzer_ShortBeep(void)
{
    static uint8_t i=0;
    Buzzer_Beep(buzzer[i], 100);
    i++;
    if (i>=8) i=0;
}

// 长滴一声
void Buzzer_LongBeep(void)
{
    Buzzer_Beep(NOTE_A4, 300);
}

// 双滴提示音
void Buzzer_DoubleBeep(void)
{
    Buzzer_Beep(NOTE_A4, 100);
    Buzzer_Beep(NOTE_A4, 100);
}

/* 播放旋律 */
void Buzzer_PlayMelody(const Buzzer_Note_t *notes, uint16_t count, uint16_t gap_ms)
{
    for (uint16_t i = 0; i < count; i++)
    {
        if (notes[i].freq == NOTE_REST)
        {
            Buzzer_SetTone(0);
            HAL_Delay(notes[i].duration_ms);
        }
        else
        {
            Buzzer_SetTone(notes[i].freq);
            HAL_Delay(notes[i].duration_ms);
            Buzzer_SetTone(0);
        }
        HAL_Delay(gap_ms);
    }
    Buzzer_SetTone(0);
}

/* 开机提示音: 上行琶音 C4-E4-G4-C5, 轻快明亮 */
void Buzzer_Startup(void)
{
    const Buzzer_Note_t melody[] = {
        {NOTE_C4,  80},
        {NOTE_E4,  80},
        {NOTE_G4,  80},
        {NOTE_C5, 200},
    };
    Buzzer_PlayMelody(melody, sizeof(melody) / sizeof(melody[0]), 30);
}

/* 警告提示音: 三连短促高音 */
void Buzzer_Alert(void)
{
    const Buzzer_Note_t melody[] = {
        {NOTE_G5, 100},
        {NOTE_REST, 50},
        {NOTE_G5, 100},
        {NOTE_REST, 50},
        {NOTE_G5, 100},
    };
    Buzzer_PlayMelody(melody, sizeof(melody) / sizeof(melody[0]), 0);
}

/* 成功提示音: 上行音阶, 愉快收尾 */
void Buzzer_Success(void)
{
    const Buzzer_Note_t melody[] = {
        {NOTE_C4,  100},
        {NOTE_E4,  100},
        {NOTE_G4,  100},
        {NOTE_E4,  100},
        {NOTE_C5,  250},
    };
    Buzzer_PlayMelody(melody, sizeof(melody) / sizeof(melody[0]), 20);
}