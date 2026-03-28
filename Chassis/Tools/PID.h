/**
 * 1.version:1.2 date:2023/2/28
 * 2.用于PID
 */

#ifndef __PID_H
#define __PID_H

#include "stdint.h"

typedef struct
{
    float Kp;
    float Ki;
    float Kd;

    float max_out;  // 最大输出
    float max_iout; // 最大积分输出
    float err[2];   // 误差及上一次误差

    float set;
    float fdb;

    float out;
    float Pout;
    float Iout;
    float Dout;
} PID_t;

void PID_Init(PID_t *pid, float kp, float ki, float kd, float max_out, float max_iout);
float PID_Calc(PID_t *pid, float fdb, float set);

#endif
