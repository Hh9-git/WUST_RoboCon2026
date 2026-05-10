#ifndef C_BOARD_DVC_RGB_LED_H
#define C_BOARD_DVC_RGB_LED_H

#include "main.h"

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

#endif //C_BOARD_DVC_RGB_LED_H