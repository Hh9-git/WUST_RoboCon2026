#ifndef BSP_KEY_H
#define BSP_KEY_H

void Key_Init(void);
uint8_t Key_GetNum(void);
uint8_t Key_GetState(void);
void Key_Tick(void);

#endif