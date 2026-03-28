#ifndef REMOTE_H
#define REMOTE_H

#include "BSP.h"
#include "Global.h"
#include "Chassis.h"
#include "string.h"
#include "SBUS.h"
#include "math.h"

#define REMOTE_UART huart8

#define MaxSpeedUD 15000.0f
#define PUSH_UP 192
#define PUSH_MEDIUM 992
#define PUSH_DOWN 1792
#define Shoot_UP_DOWN 552.5    // 需要调试成最佳值 632
#define shoot_distance 11240 // 11560

#define IN_RANGE(val, target, range) ((val > target - range) && (val < target + range))

// 通道7发射的状态数组
typedef enum State
{
    DOWN = 1,
    MEDIUM = 2,
    UP = 3,
} State_t;

extern float speedUD;
extern volatile uint8_t flag;
extern int Dis;

void Remote_Init(void);
void Remote_Callback(uint8_t *pData, uint8_t size);

#endif
