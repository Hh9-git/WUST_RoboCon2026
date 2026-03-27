#include "RMDS_Driver.h"
#include <stdlib.h>

static RMDS_t **motors;
static uint8_t motorsCount; // 电机数量

/**
 * RMDS CAN中断接收回调函数
 */
void RMDS_CAN_Callback(CAN_RxHeaderTypeDef *pHeader, uint8_t *pBuf)
{
    static uint8_t serialNum;
    static uint8_t functionNum;

    serialNum = pHeader->StdId & 0xF0;
    functionNum = pHeader->StdId & 0x0F;

    for (uint8_t i = 0; i < motorsCount; i++)
    {
        if (serialNum == motors[i]->ID)
        {
            switch (functionNum)
            {
            case 0x0B:
                motors[i]->current = (int16_t)(pBuf[0] << 8) | pBuf[1];
                motors[i]->speed = ((int16_t)(pBuf[2] << 8) | pBuf[3]) / REDUC_RATIO;
                motors[i]->position = ((int32_t)(pBuf[4] << 24) | (pBuf[5] << 16) | (pBuf[6] << 8) | pBuf[7]) / ENCODER_COUNT / REDUC_RATIO * 360;
                return;

            case 0x0D:
                motors[i]->IsBlocking = 1;
                return;
            }
        }
    }
}

/**
 * RMDS初始化
 * 输入电机结构体地址，编号
 */
void RMDS_Init(RMDS_t *motor, uint8_t number)
{
    motor->ID = number << 4;                                             // 设置ID
    motorsCount++;                                                       // 电机数+1
    motors = (RMDS_t **)realloc(motors, motorsCount * sizeof(RMDS_t *)); // 增加数组大小
    motors[motorsCount - 1] = motor;                                     // 存储电机结构体地址
}

static uint8_t data[8];

/**
 * RMDS复位
 */
void RMDS_Reset(RMDS_t *motor)
{
    data[0] = 0x55;
    data[1] = 0x55;
    data[2] = 0x55;
    data[3] = 0x55;
    data[4] = 0x55;
    data[5] = 0x55;
    data[6] = 0x55;
    data[7] = 0x55;
    CAN_Transmit(&RMDS_CAN, motor->ID, data);
}

/**
 * RMDS组内复位
 */
void RMDS_ResetAll(void)
{
    data[0] = 0x55;
    data[1] = 0x55;
    data[2] = 0x55;
    data[3] = 0x55;
    data[4] = 0x55;
    data[5] = 0x55;
    data[6] = 0x55;
    data[7] = 0x55;
    CAN_Transmit(&RMDS_CAN, 0x00, data);
}

/**
 * 设置RMDS模式
 */
void RMDS_SetMode(RMDS_t *motor, RMDS_e mode)
{
    data[0] = mode;
    data[1] = 0x55;
    data[2] = 0x55;
    data[3] = 0x55;
    data[4] = 0x55;
    data[5] = 0x55;
    data[6] = 0x55;
    data[7] = 0x55;
    CAN_Transmit(&RMDS_CAN, motor->ID + 1, data);
}

/**
 * 设置RMDS组内模式
 */
void RMDS_SetModeAll(RMDS_e mode)
{
    data[0] = mode;
    data[1] = 0x55;
    data[2] = 0x55;
    data[3] = 0x55;
    data[4] = 0x55;
    data[5] = 0x55;
    data[6] = 0x55;
    data[7] = 0x55;
    CAN_Transmit(&RMDS_CAN, 0x01, data);
}

/**
 * 设置RMDS电流
 * 电流单位mA
 */
void RMDS_SetCurrent(RMDS_t *motor, int16_t current)
{
    data[0] = 0x13;
    data[1] = 0x88;
    data[2] = (uint8_t)((current >> 8) & 0xff);
    data[3] = (uint8_t)(current & 0xff);
    data[4] = 0x55;
    data[5] = 0x55;
    data[6] = 0x55;
    data[7] = 0x55;
    CAN_Transmit(&RMDS_CAN, motor->ID + 3, data);
}

/**
 * 设置RMDS速度
 * 速度单位RPM
 */
void RMDS_SetSpeed(RMDS_t *motor, float speed)
{
    static int16_t realSpeed;
    realSpeed = (int16_t)(speed * REDUC_RATIO);

    data[0] = 0x13;
    data[1] = 0x88;
    data[2] = (uint8_t)((realSpeed >> 8) & 0xff);
    data[3] = (uint8_t)(realSpeed & 0xff);
    data[4] = 0x55;
    data[5] = 0x55;
    data[6] = 0x55;
    data[7] = 0x55;
    CAN_Transmit(&RMDS_CAN, motor->ID + 4, data);
}

/**
 * 设置RMDS位置
 * 位置单位度
 */
void RMDS_SetPosition(RMDS_t *motor, float angle)
{
    static int32_t realAngle;
    realAngle = (int32_t)(angle * ENCODER_COUNT * REDUC_RATIO / 360);

    data[0] = 0x13;
    data[1] = 0x88;
    data[2] = 0x55;
    data[3] = 0x55;
    data[4] = (uint8_t)((realAngle >> 24) & 0xff);
    data[5] = (uint8_t)((realAngle >> 16) & 0xff);
    data[6] = (uint8_t)((realAngle >> 8) & 0xff);
    data[7] = (uint8_t)(realAngle & 0xff);

    CAN_Transmit(&RMDS_CAN, motor->ID + 5, data);
}

/**
 * 设置RMDS位置以一定速度
 * 速度单位RPM
 * 位置单位
 */
void RMDS_SetSpeedPosition(RMDS_t *motor, float speed, float angle)
{
    static int16_t realSpeed;
    static int32_t realAngle;
    realSpeed = (int16_t)(speed * REDUC_RATIO);
    realAngle = (int32_t)(angle * ENCODER_COUNT * REDUC_RATIO / 360);

    data[0] = 0x13;
    data[1] = 0x88;
    data[2] = (uint8_t)((realSpeed >> 8) & 0xff);
    data[3] = (uint8_t)(realSpeed & 0xff);
    data[4] = (uint8_t)((realAngle >> 24) & 0xff);
    data[5] = (uint8_t)((realAngle >> 16) & 0xff);
    data[6] = (uint8_t)((realAngle >> 8) & 0xff);
    data[7] = (uint8_t)(realAngle & 0xff);

    CAN_Transmit(&RMDS_CAN, motor->ID + 6, data);
}

/**
 * 设置RMDS位置以一定速度
 * 电流单位mA
 * 速度单位RPM
 */
void RMDS_SetCurrentSpeed(RMDS_t *motor, uint16_t current, float speed)
{
    static int16_t realSpeed;
    realSpeed = (int16_t)(speed * REDUC_RATIO);

    data[0] = (uint8_t)((current >> 8) & 0xff);
    data[1] = (uint8_t)(current & 0xff);
    data[2] = (uint8_t)((realSpeed >> 8) & 0xff);
    data[3] = (uint8_t)(realSpeed & 0xff);
    data[4] = 0x55;
    data[5] = 0x55;
    data[6] = 0x55;
    data[7] = 0x55;

    CAN_Transmit(&RMDS_CAN, motor->ID + 7, data);
}

/**
 * RMDS开启返回信息
 */
void RMDS_SetFreeback(RMDS_t *motor, uint8_t time_ms)
{
    data[0] = time_ms;
    data[1] = 0x00;
    data[2] = 0x55;
    data[3] = 0x55;
    data[4] = 0x55;
    data[5] = 0x55;
    data[6] = 0x55;
    data[7] = 0x55;

    CAN_Transmit(&RMDS_CAN, motor->ID + 10, data);
}

/**
 * RMDS组内开启返回信息
 */
void RMDS_SetFreebackAll(uint8_t time_ms)
{
    data[0] = time_ms;
    data[1] = 0x00;
    data[2] = 0x55;
    data[3] = 0x55;
    data[4] = 0x55;
    data[5] = 0x55;
    data[6] = 0x55;
    data[7] = 0x55;

    CAN_Transmit(&RMDS_CAN, 0x0A, data);
}
