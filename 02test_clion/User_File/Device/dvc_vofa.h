#ifndef DVC_VOFA_H
#define DVC_VOFA_H

#include <stdint.h>

// 最大支持参数数量（可修改）
#define VOFA_MAX_PARAMS 4

// 调参数据回调函数类型
typedef void (*vofa_param_callback_t)(uint8_t index, float value);

// 发送单字节回调（平台相关）
typedef void (*vofa_send_byte_t)(uint8_t byte);

// 初始化 VOFA 库
void vofa_init(vofa_send_byte_t send_func, vofa_param_callback_t param_cb);

// 多通道绘图：发送 N 个 float
void vofa_draw(float *data, uint8_t ch_num);

// 解析串口接收数据（外部传入）
void vofa_parse_rx(uint8_t *buf, uint16_t len);

void uart_send_byte(uint8_t byte);

#endif //DVC_VOFA_H