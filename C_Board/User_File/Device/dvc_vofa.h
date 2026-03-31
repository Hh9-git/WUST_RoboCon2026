
#ifndef DVC_VOFA_H
#define DVC_VOFA_H


#include "main.h"
#include "drv_usart.h"
//FireWater  0
//JustFloat  1
#define VOFA_DATA_FORMAT 1

#define commandlength 200
#define framehead '='
#define frametail '!'

typedef struct{
    float speed_kp;
    float speed_ki;
    float speed_kd;
    float position_kp;
    float position_ki;
    float position_kd;
}pid_para_t;

float Get_Data(void);
void USART_PID_Adjust(uint8_t Motor_n);

void firewater_displaydata(float position_target,float position_actual,float position_out,float speed_target,float speed_actual,float speed_out);
void Float_to_Byte(float f,unsigned char byte[]);
void JustFloat_test(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void justfloat_displaydata(float position_target,float position_actual,float position_out,float speed_target,float speed_actual,float speed_out);

#endif //DVC_VOFA_H