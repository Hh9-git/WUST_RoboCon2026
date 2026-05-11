#include "drv_PWM.h"


void PWM_Init(void)
{
    HAL_TIM_Base_Start(&htim1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
    HAL_TIM_Base_Start(&htim8);
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3);
}

void PWM_SetDuty(void)
{
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, 500);
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, 500);
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_3, 500);
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_4, 500);
    __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_1, 500);
    __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_2, 500);
    __HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_3, 500);
}