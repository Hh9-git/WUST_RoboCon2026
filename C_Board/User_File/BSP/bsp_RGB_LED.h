#ifndef BSP_RGB_LED_H
#define BSP_RGB_LED_H

#include "main.h"
#include "tim.h"

extern TIM_HandleTypeDef htim5;

/**
  * @brief          aRGB show
  * @param[in]      aRGB: 0xaaRRGGBB, 'aa' is alpha, 'RR' is red, 'GG' is green, 'BB' is blue
  * @retval         none
  */
/**
  * @brief          显示RGB
  * @param[in]      aRGB:0xaaRRGGBB,'aa' 是透明度,'RR'是红色,'GG'是绿色,'BB'是蓝色
  * @retval         none
  */
extern void aRGB_led_show(uint32_t aRGB);
void RGB_LED_Init(void);
void RGB_LED_off();
void RGB_64LED_ON();
void RGB_64LEDS_Init();
#endif