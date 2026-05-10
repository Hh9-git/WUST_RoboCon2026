#include "gpio.h"

uint8_t Key_Num;

void Key_Init(void)
{
    MX_GPIO_Init();
}

uint8_t Key_GetNum(void)
{
    uint8_t Temp;
    if(Key_Num)
    {
        Temp=Key_Num;
        Key_Num=0;
        return Temp;
    }
    return 0;
}

uint8_t Key_GetState(void)
{
    if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==0)
    {
        return  1;
    }
    return 0;
}

void Key_Tick(void)
{
    static uint8_t Count;
    static uint8_t CurrState,PrevState;
    Count++;
    if(Count>=20)
    {
        Count=0;
        PrevState=CurrState;
        CurrState=Key_GetState();

        if(CurrState==0&&PrevState!=0)
        {
            Key_Num=PrevState;
        }
    }
}
