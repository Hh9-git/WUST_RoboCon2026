#include "dvc_buzzer.h"

#define MAX_PSC                 1000

#define MAX_BUZZER_PWM      20000
#define MIN_BUZZER_PWM      10000

uint16_t psc = 0;
uint16_t pwm = MIN_BUZZER_PWM;


extern TIM_HandleTypeDef htim4;
void Buzzer_Init(void)
{
    //start tim
    HAL_TIM_Base_Start(&htim4);
    //start pwm channel
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
}

void Buzzer_on(uint16_t psc, uint16_t pwm)
{
    __HAL_TIM_PRESCALER(&htim4, psc);
    __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_3, pwm);

}
void Buzzer_off(void)
{
    __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_3, 0);
}