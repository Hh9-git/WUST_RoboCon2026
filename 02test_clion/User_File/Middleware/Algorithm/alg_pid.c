#include "alg_pid.h"
#include "stdint.h"

// PID初始化
void PID_Init(PID_t *pid, float kp, float ki, float kd, float max_out, float max_iout)
{
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;
    pid->max_out = max_out;
    pid->max_iout = max_iout;
    pid->err[0] = pid->err[1] = 0.0f;
}

// PID计算, 输入反馈值和预期值
float PID_Calc(PID_t *pid, float fdb, float set)
{
    pid->set = set;
    pid->fdb = fdb;
    pid->err[1] = pid->err[0];
    pid->err[0] = pid->set - pid->fdb;

    pid->Pout = pid->Kp * pid->err[0];
    pid->Iout += pid->Ki * pid->err[0];
    pid->Dout = pid->Kd * pid->err[0] - pid->err[1];

    if (pid->Iout > pid->max_iout)
    {
        pid->Iout = pid->max_iout;
    }
    else if (pid->Iout < -pid->max_iout)
    {
        pid->Iout = -pid->max_iout;
    }

    pid->out = pid->Pout + pid->Iout + pid->Dout;

    if (pid->out > pid->max_out)
    {
        pid->out = pid->max_out;
    }
    else if (pid->out < -pid->max_out)
    {
        pid->out = -pid->max_out;
    }

    return pid->out;
}
