/**
* @file Task.cpp
 * @author Hh999999
 * @brief 任务函数，放在一个单独的文件里，main函数里调用Task_Loop()，
 * 把任务函数和main函数分开，main函数只负责调用任务函数，任务函数负责具体的任务逻辑，这样代码结构更清晰，易于维护和扩展
 * @version 1.0
 * @date 2026-03-05 23:00 1.0
 *
 * /

/* Includes ------------------------------------------------------------------*/
#ifndef TASK_H
#define TASK_H

/*判断当前环境是否为c++编译环境。把函数写到两个{}之间，避免不识别cpp文件编译失败*/
#ifdef __cplusplus
extern "C" {
#endif


void Task_Loop();

#ifdef __cplusplus
}
#endif

#endif //TASK_H