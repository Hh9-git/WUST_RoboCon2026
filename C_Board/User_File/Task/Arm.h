#ifndef C_BOARD_ARM_H
#define C_BOARD_ARM_H

#include "alg_fastmath.h"

void ForceControl_Calculate_Safe(
    float xd, float yd,
    float theta1, float theta2,
    float omega1, float omega2,
    float *tau1_out, float *tau2_out);
#endif //C_BOARD_ARM_H