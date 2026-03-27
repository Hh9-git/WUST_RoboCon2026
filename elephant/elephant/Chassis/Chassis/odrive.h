#ifndef __ODRIVE_H
#define __ODRIVE_H

#include "main.h"
#include "stdio.h"
#include "string.h"

#define AXIS0_NODE_ID (4 << 5) // odrive ID
#define AXIS1_NODE_ID (5 << 5)

typedef union _float_to_uint8_t
{
  uint8_t raw[8];
  float value[2];
  uint32_t u32_data[2];
  int32_t int32_data[2];
} float_to_uint8_t;

typedef enum
{
  AXIS_0 = 0,
  AXIS_1 = 1
} Axis_t;

typedef enum
{
  AXIS_STATE_UNDEFINED = 0,                  //<! will fall through to idle
  AXIS_STATE_IDLE = 1,                       //<! disable PWM and do nothing
  AXIS_STATE_STARTUP_SEQUENCE = 2,           //<! the actual sequence is defined by the config.startup_... flags
  AXIS_STATE_FULL_CALIBRATION_SEQUENCE = 3,  //<! run all calibration procedures, then idle
  AXIS_STATE_MOTOR_CALIBRATION = 4,          //<! run motor calibration
  AXIS_STATE_SENSORLESS_CONTROL = 5,         //<! run sensorless control
  AXIS_STATE_ENCODER_INDEX_SEARCH = 6,       //<! run encoder index search
  AXIS_STATE_ENCODER_OFFSET_CALIBRATION = 7, //<! run encoder offset calibration
  AXIS_STATE_CLOSED_LOOP_CONTROL = 8,        //<! run closed loop control
  AXIS_STATE_LOCKIN_SPIN = 9,                //<! run lockin spin
  AXIS_STATE_ENCODER_DIR_FIND = 10,
  AXIS_STATE_HOMING = 11, //<! run axis homing function
} State_t;

typedef struct
{
  uint32_t axis_error;
  uint32_t axis_current_state;
  uint32_t motor_error;
  uint32_t encoder_error;
  uint32_t sensorless_error;
  float encoder_pos_estimate;
  float encoder_vel_estimate;
  int32_t encoder_shadow_count;
  int32_t encoder_cpr_count;
  float iq_setpoint;
  float iq_measured;
  float sensorless_pos_estimate;
  float sensorless_vel_estimate;
  float vbus_voltage;
} OdriveAxisGetState_t;

typedef struct
{
  uint16_t axis_node_id;
  uint32_t requested_state;
  int32_t control_mode;
  int32_t input_mode;
  int16_t vel_ff;
  int16_t current_ff;
  int32_t input_pos;
  // int32_t input_vel;
  float input_vel;
  float torque_vel;
  int32_t input_current;
  float vel_limit;
  float traj_vel_limit; //????(traj)
  float traj_accel_limit;
  float traj_decel_limit;
  float traj_a_per_css;
} OdriveAxisSetState_t;

typedef enum
{
  MSG_CO_NMT_CTRL = 0x000, // CANOpen NMT Message REC
  MSG_ODRIVE_HEARTBEAT,
  MSG_ODRIVE_ESTOP,
  MSG_GET_MOTOR_ERROR,          // Errors
  MSG_GET_ENCODER_ERROR,        //...
  MSG_GET_SENSORLESS_ERROR,     //...
  MSG_SET_AXIS_NODE_ID,         // ??ID
  MSG_SET_AXIS_REQUESTED_STATE, // ??axis0??????
  MSG_SET_AXIS_STARTUP_CONFIG,  //...
  MSG_GET_ENCODER_ESTIMATES,    // ?????????(??:turn)???????(??:turn/s)
  MSG_GET_ENCODER_COUNT,        //...??,???count?count/s
  MSG_SET_CONTROLLER_MODES,     // ???????????????--
  MSG_SET_INPUT_POS = 0x00C,    // ?????????
  MSG_SET_INPUT_VEL = 0x00D,    // ?????????-------###
  MSG_SET_INPUT_CURRENT,        // ????????????
  MSG_SET_VEL_LIMIT,            // ?????????
  MSG_START_ANTICOGGING,        // ??anticogging??
  MSG_SET_TRAJ_VEL_LIMIT,       // ????????(????????)
  MSG_SET_TRAJ_ACCEL_LIMITS,    // ????????????????? turn/s^2
  MSG_SET_TRAJ_A_PER_CSS,       // ??????????,???0
  MSG_GET_IQ,                   // ??????????????????????????????
  MSG_GET_SENSORLESS_ESTIMATES, // ????MOSFET???????
  MSG_RESET_ODRIVE,             // ??ODrive??,?odrv0.reboot()
  MSG_GET_VBUS_VOLTAGE,         // ????DC?????????
  MSG_CLEAR_ERRORS = 0x018,     // ????
  MSG_CO_HEARTBEAT_CMD = 0x700, // CANOpen NMT Heartbeat SEND
} Odrive_Commond;

void odrive_set_speed(float speed); // TODO: check
uint8_t odrv_write_msg(Axis_t axis, Odrive_Commond cmd);

#endif
