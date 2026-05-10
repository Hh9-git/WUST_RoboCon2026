#ifndef DVC_KEY_H
#define DVC_KEY_H

void Key_Init(void);
uint8_t Key_GetNum(void);
uint8_t Key_GetState(void);
void Key_Tick(void);

#endif //DVC_KEY_H