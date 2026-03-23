
#ifndef DVC_REMOTE_H
#define DVC_REMOTE_H

#include <stdint.h>
#include "usart.h"

//结构体定义

typedef struct{
    int16_t X_L;
    int16_t Y_L;
    int16_t X_R;
    int16_t Y_R;
    int16_t key_a;
    int16_t key_b;
    int16_t key_c;
    int16_t key_d;
    int16_t adc_a;
    int16_t adc_b;
}remote_t;



//函数定义
void __remote_sbus_deserialize(const uint8_t *buf);
void __remote_debug(void);
void remote_init(remote_t *ctrl);
void remote_task(void);


#endif //DVC_REMOTE_H