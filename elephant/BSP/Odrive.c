#include "odrive.h"

typedef union
{
    uint8_t raw[8];
    float value[2];
} float_to_uint8_t;

/* ODRIVE设置速度 */
void odrive_set_speed(float speed1, float speed2)
{
    float_to_uint8_t pack; // 共用体

    pack.value[0] = speed1; // odrive_set_axis0.input_vel;单位为turn/s
    pack.value[1] = 0;
    while (HAL_CAN_GetTxMailboxesFreeLevel(&ODRIVE_CAN) == 0)
        continue;
    CAN_Transmit(&ODRIVE_CAN, (ODRIVE_AXIS0_ID << 5) + MSG_SET_INPUT_VEL, pack.raw);

    pack.value[0] = speed2; // odrive_set_axis0.input_vel;单位为turn/s
    pack.value[1] = 0;
    while (HAL_CAN_GetTxMailboxesFreeLevel(&ODRIVE_CAN) == 0)
        continue;
    CAN_Transmit(&ODRIVE_CAN, (ODRIVE_AXIS1_ID << 5) + MSG_SET_INPUT_VEL, pack.raw);
}
