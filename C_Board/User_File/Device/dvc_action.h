/**
* 1.version:1.4 data:2023/5/23
 * 2.Action正前方为X轴, 正右方为Y轴
 */

#ifndef DVC_ACTION_H
#define DVC_ACTION_H
#include "drv_usart.h"

#define ACTION_UART huart1 // 串口重定义

/* 码盘姿态 */
typedef struct
{
    float yaw;
    float pos_x;
    float pos_y;

    float offset_x;
    float offset_y;
}posture_t;

extern posture_t Action;
extern uint8_t receiveData[28];
// Action码盘初始化, 上电后需要等待十几秒, Action码盘才会正常工作
void Action_Init(void);
// Action数据清零
void Action_ClearData(void);

void Action_Callback(uint8_t *pData, uint16_t size);

void Action_Correct(void);

#endif //DVC_ACTION_H