/**
* @file dvc_LED.c
 * @author Hh999999
 * @brief A板LED驱动,LED红绿灯交替闪烁
 * @version 1.0
 * @date 2026-03-05 22:59 1.0
 */

/* Includes ------------------------------------------------------------------*/
#include "dvc_LED.h"
#include "GPIO.h"

void LED_red_Toggle()
{
    HAL_GPIO_TogglePin(LED_red_GPIO_Port, LED_red_Pin);
}

void LED_green_Toggle()
{
    HAL_GPIO_TogglePin(LED_green_GPIO_Port, LED_green_Pin);
}