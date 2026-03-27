#include "dvc_vofa.h"
#include "usart.h"

static vofa_send_byte_t s_send_func = NULL;
static vofa_param_callback_t s_param_cb = NULL;

// JustFloat 帧尾
static const uint8_t VOFA_TAIL[4] = {0x00, 0x00, 0x80, 0x7F};

// 协议定义
#define VOFA_FRAME_LEN    8
#define VOFA_HEAD1        0xAA
#define VOFA_HEAD2        0x55
#define VOFA_TAIL_BYTE    0xFF

// 初始化
void vofa_init(vofa_send_byte_t send_func, vofa_param_callback_t param_cb)
{
    s_send_func = send_func;
    s_param_cb = param_cb;
}

// 多通道绘图
void vofa_draw(float *data, uint8_t ch_num)
{
    if (s_send_func == NULL || data == NULL || ch_num == 0)
        return;

    uint8_t *p = (uint8_t *)data;
    for (uint16_t i = 0; i < ch_num * 4; i++)
    {
        s_send_func(p[i]);
    }

    for (uint8_t i = 0; i < 4; i++)
    {
        s_send_func(VOFA_TAIL[i]);
    }
}

// 解析调参指令：AA 55 INDEX FLOAT FF
void vofa_parse_rx(uint8_t *buf, uint16_t len)
{
    if (len != VOFA_FRAME_LEN || s_param_cb == NULL)
        return;

    if (buf[0] == VOFA_HEAD1 && buf[1] == VOFA_HEAD2 && buf[7] == VOFA_TAIL_BYTE)
    {
        uint8_t idx = buf[2];
        float val = *(float *)&buf[3];

        if (idx < VOFA_MAX_PARAMS)
            s_param_cb(idx, val);
    }
}

// 你想使用哪个串口就写哪个
void uart_send_byte(uint8_t byte)
{
    HAL_UART_Transmit(&huart2, &byte, 1, 10);
}