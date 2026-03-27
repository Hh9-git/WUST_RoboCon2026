/**
 * 1.version:1.3 data:2023/3/6
 * 2.Action正前方为X轴, 正右方为Y轴
 */

#ifndef __ACTION_H
#define __ACTION_H

#include "usart.h"
#include "main.h"

#define ACTION_UART huart1 // 串口重定义

/* 马盘姿态 */
typedef struct
{
    float yaw;
    float pos_x;
    float pos_y;
} posture_t;

extern posture_t Action;

// Action码盘初始化, 上电后需要等待十几秒, Action码盘才会正常工作
void Action_Init(void);
// Action数据清零
void Action_ClearData(void);

#endif
