#ifndef __ODRIVE_H
#define __ODRIVE_H

#include "BSP.h"

#define ODRIVE_AXIS0_ID 4
#define ODRIVE_AXIS1_ID 5
#define ODRIVE_CAN hcan1

typedef enum
{
    MSG_CO_NMT_CTRL = 0x000, // CANOpen NMT Message REC
    MSG_ODRIVE_HEARTBEAT,
    MSG_ODRIVE_ESTOP,
    MSG_GET_MOTOR_ERROR,          // Errors
    MSG_GET_ENCODER_ERROR,        //...
    MSG_GET_SENSORLESS_ERROR,     //...
    MSG_SET_AXIS_NODE_ID,         // 获取ID
    MSG_SET_AXIS_REQUESTED_STATE, // 命令axis0进入某个状态
    MSG_SET_AXIS_STARTUP_CONFIG,  //...
    MSG_GET_ENCODER_ESTIMATES,    // 当前预测到的位置值(单位:turn)和当前估算转速(单位:turn/s)
    MSG_GET_ENCODER_COUNT,        //...同上,单位为count和count/s
    MSG_SET_CONTROLLER_MODES,     // 设置控制模式和控制信号输入模式--
    MSG_SET_INPUT_POS = 0x00C,    // 输入的电机目标位置
    MSG_SET_INPUT_VEL = 0x00D,    // 输入的电机目标转速-------###
    MSG_SET_INPUT_CURRENT,        // 输入的电机输出的力矩大小
    MSG_SET_VEL_LIMIT,            // 设置速度和电流限制
    MSG_START_ANTICOGGING,        // 执行anticogging校准
    MSG_SET_TRAJ_VEL_LIMIT,       // 轨迹控制最大速度(即匀速阶段的转速)
    MSG_SET_TRAJ_ACCEL_LIMITS,    // 轨迹控制最大加速度和最大反向加速度 turn/s^2
    MSG_SET_TRAJ_A_PER_CSS,       // 轨迹控制负载惯量大小,默认为0
    MSG_GET_IQ,                   // 电流环控制输入的目标交轴电流值和通过电流采样获取的交轴电流值
    MSG_GET_SENSORLESS_ESTIMATES, // 得到当前MOSFET温度和电机温度
    MSG_RESET_ODRIVE,             // 重启ODrive硬件,同odrv0.reboot()
    MSG_GET_VBUS_VOLTAGE,         // 获取当前DC总线上的电压和电流
    MSG_CLEAR_ERRORS = 0x018,     // 清除错误
    MSG_CO_HEARTBEAT_CMD = 0x700, // CANOpen NMT Heartbeat SEND
} Odrive_Commond;

void odrive_set_speed(float speed1, float speed2);

#endif
