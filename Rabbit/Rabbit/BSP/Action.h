/**
 * 1.version:1.4 data:2023/5/23
 * 2.Action正前方为X轴, 正右方为Y轴
 */

#ifndef ACTION_H
#define ACTION_H

#include "BSP.h"

#define ACTION_UART huart6 // 串口重定义

/* 码盘姿态 */
typedef struct
{
    float yaw;
    float pos_x;
    float pos_y;
    float offset_x;
    float offset_y;
    float offset_yaw;
} posture_t;

extern posture_t Action;

// Action码盘初始化, 上电后需要等待十几秒, Action码盘才会正常工作
void Action_Init(void);
// Action数据清零
void Action_ClearData(void);

#endif
