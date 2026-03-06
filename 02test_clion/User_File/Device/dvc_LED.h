/**
* @file dvc_LED.cpp
 * @author Hh999999
 * @brief A板LED驱动,LED红绿灯交替闪烁
 * @version 1.0
 * @date 2026-03-05 22:59 1.0
 */


#ifndef LED_H
#define LED_H

/*判断当前环境是否为c++编译环境。把函数写到两个{}之间，避免不识别cpp文件编译失败*/
#ifdef __cplusplus
extern "C" {
#endif
void LED_red_Toggle();

void LED_green_Toggle();

#ifdef __cplusplus
}
#endif
#endif //LED_H