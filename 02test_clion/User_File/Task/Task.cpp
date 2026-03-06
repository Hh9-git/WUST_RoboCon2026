/**
 * @file Task.cpp
 * @author Hh999999
 * @brief 任务函数，放在一个单独的文件里，main函数里调用Task_Loop()，
 * 把任务函数和main函数分开，main函数只负责调用任务函数，任务函数负责具体的任务逻辑，这样代码结构更清晰，易于维护和扩展
 * @version 1.0
 * @date 2026-03-05 23:00 1.0
 *
 * /

/* Includes ------------------------------------------------------------------*/
#include "Task.h"
#include "gpio.h"
#include "../Device/dvc_LED.h"

/* Exported function declarations --------------------------------------------*/

void Task_Loop()
{
    while (1)
    {
        LED_green_Toggle();
        HAL_Delay(500);
        LED_red_Toggle();
        HAL_Delay(500);
    }
}
