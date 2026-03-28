#ifndef __REMOTE_H
#define __REMOTE_H

#include "main.h"

#define REMOTE_UART huart6
// extern pos_X, pos_y, angle;
// extern ID;
struct remote_t
{

    uint8_t mode; // 模式
    uint8_t ID;   // 接收ID

    uint8_t Data[9]; // 发送上位机数据

    int16_t pos_x;
    int16_t pos_y;
    int16_t angle;

} remote;
void Remote_Callback(uint8_t *pData, uint8_t size);
void Remote_Init(void);

#endif
