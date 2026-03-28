
#ifndef __RMDS_DRIVER_H
#define __RMDS_DRIVER_H

#include "main.h"
#include "BSP.h"

#define RMDS_CAN hcan1

#define ENCODER_COUNT 2000.0f // 编码器分辨率
#define REDUC_RATIO 12.25f    // 减速比

typedef enum
{
    OPENLOOP = 1,               // 开环模式
    CURRENT = 2,                // 电流模式
    SPEED = 3,                  // 速度模式
    POSITION = 4,               // 位置模式
    SPEED_POSITION = 5,         // 速度位置模式
    CURRENT_SPEED = 6,          // 电流速度模式
    CURRENT_POSITION = 7,       // 电流位置模式
    CURRENT_SPEED_POSITION = 8, // 电流速度位置模式
} RMDS_e;

typedef struct
{
    uint8_t ID;
    int16_t current;
    float speed;
    float position;
    uint8_t IsBlocking; // 是否堵转

} RMDS_t;

void RMDS_CAN_Callback(CAN_RxHeaderTypeDef *pHeader, uint8_t *pBuf);
void RMDS_Init(RMDS_t *motor, uint8_t number);
void RMDS_Reset(RMDS_t *motor);
void RMDS_ResetAll(void);
void RMDS_SetMode(RMDS_t *motor, RMDS_e mode);
void RMDS_SetModeAll(RMDS_e mode);
void RMDS_SetCurrent(RMDS_t *motor, int16_t current);
void RMDS_SetSpeed(RMDS_t *motor, float speed);
void RMDS_SetPosition(RMDS_t *motor, float angle);
void RMDS_SetSpeedPosition(RMDS_t *motor, float speed, float angle);
void RMDS_SetCurrentSpeed(RMDS_t *motor, uint16_t current, float speed);
void RMDS_SetFreeback(RMDS_t *motor, uint8_t time_ms);
void RMDS_SetFreebackAll(uint8_t time_ms);

#endif
