
#ifndef DRV_TIM_H
#define DRV_TIM_H

#include "main.h"
// 如果启用TIM base功能
#ifdef HAL_TIM_MODULE_ENABLED

typedef struct
{
    TIM_HandleTypeDef *htim;
    void (*TIM_Callback)(void);

} TIM_Interrupt_t;

/**
 * 联接TIM中断源和中断回调函数
 * 应输入TIM源, 回调函数个数以及回调函数的地址
 * 联接后自动开启中断
 */
void AttachInterrupt_TIM(TIM_HandleTypeDef *htim, void (*TIM_Callback)(void));
#endif /* HAL_TIM_MODULE_ENABLED */

#endif //DRV_TIM_H