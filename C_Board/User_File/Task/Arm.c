#include "Arm.h"
#include <math.h>

// ===================== 可修改参数 =====================
#define L1          0.4f   // 大臂长度 m
#define L2          0.3f   // 小臂长度 m
#define Kp          10.0f  // 刚度
#define Kd          1.0f    // 阻尼

#define MAX_TAU1    3.0f    // 关节1最大力矩 N·m（根据海泰8108调）
#define MAX_TAU2    3.0f    // 关节2最大力矩 N·m
//=======================================================

/**
 * @brief  带安全力矩限幅的 2自由度 阻抗力控计算（只输出力矩）
 * @param  xd, yd:   目标末端位置 (m)
 * @param  theta1,theta2: 当前关节角度 (rad)
 * @param  omega1,omega2: 当前关节速度 (rad/s)
 * @param  tau1_out, tau2_out: 输出限幅后的安全力矩
 */
void ForceControl_Calculate_Safe(
    float xd, float yd,
    float theta1, float theta2,
    float omega1, float omega2,
    float *tau1_out, float *tau2_out)
{
    // 1. 正运动学
    float x = L1 * cosf(theta1) + L2 * cosf(theta1 + theta2);
    float y = L1 * sinf(theta1) + L2 * sinf(theta1 + theta2);

    // 2. 阻抗控制：计算末端期望力
    float Fx = Kp * (xd - x) - Kd * omega1;
    float Fy = Kp * (yd - y) - Kd * omega2;

    // 3. 雅可比矩阵
    float s1  = sinf(theta1);
    float s12 = sinf(theta1 + theta2);
    float c1  = cosf(theta1);
    float c12 = cosf(theta1 + theta2);

    float J11 = -L1*s1 - L2*s12;
    float J12 = -L2*s12;
    float J21 =  L1*c1 + L2*c12;
    float J22 =  L2*c12;

    // 4. 计算原始关节力矩
    float tau1_raw = J11 * Fx + J21 * Fy;
    float tau2_raw = J12 * Fx + J22 * Fy;

    // ===================== 安全力矩限幅（核心）=====================
    // 关节1
    if (tau1_raw > MAX_TAU1)      *tau1_out = MAX_TAU1;
    else if (tau1_raw < -MAX_TAU1)*tau1_out = -MAX_TAU1;
    else                          *tau1_out = tau1_raw;

    // 关节2
    if (tau2_raw > MAX_TAU2)      *tau2_out = MAX_TAU2;
    else if (tau2_raw < -MAX_TAU2)*tau2_out = -MAX_TAU2;
    else                          *tau2_out = tau2_raw;
}