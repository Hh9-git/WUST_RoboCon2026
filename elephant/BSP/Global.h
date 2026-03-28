#ifndef GLOBAL_H
#define GLOBAL_H

#define USE_CAN 1

struct Global
{
    uint8_t isGo; // 可以自动走
    uint8_t step; // 定点索引
    // timFlag_e timFlag; // 定时器索引
} global;

#endif
