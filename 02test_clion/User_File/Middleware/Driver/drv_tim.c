#include "drv_tim.h"

//-------------------------------------------------------------------------------------------------------------------
// TIM Interrupt
#ifdef HAL_TIM_MODULE_ENABLED

static uint8_t TIM_Function_Count = 0;      // 函数数量
static TIM_Interrupt_t *TIM_ItSource_Array; // TIM中断回调函数结构体数组指针

// 联接TIM中断源和中断回调函数
void AttachInterrupt_TIM(TIM_HandleTypeDef *htim, void (*TIM_Callback)(void))
{
    TIM_ItSource_Array = (TIM_Interrupt_t *)realloc(TIM_ItSource_Array, (TIM_Function_Count + 1) * sizeof(TIM_Interrupt_t));
    TIM_ItSource_Array[TIM_Function_Count].htim = htim;
    TIM_ItSource_Array[TIM_Function_Count].TIM_Callback = TIM_Callback;
    TIM_Function_Count++;
    HAL_TIM_Base_Start_IT(htim);
}

// TIM中断回调
__weak void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    for (uint8_t i = 0; i < TIM_Function_Count; i++)
    {
        if (TIM_ItSource_Array[i].htim == htim)
        {
            TIM_ItSource_Array[i].TIM_Callback();
        }
    }
}

#endif /* HAL_TIM_MODULE_ENABLED */