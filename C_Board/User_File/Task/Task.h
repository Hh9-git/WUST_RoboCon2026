
#ifndef TASK_H
#define TASK_H
#include "drv_can.h"
#include "drv_tim.h"
#include "stm32f4xx_hal.h"
#include "drv_usart.h"
#include "dvc_dji_motor.h"
#include "dvc_vofa.h"
#include "drv_tim.h"
#include "dvc_remote.h"
#include "Chassis.h"
#include "dvc_HT_motor.h"
#include "dvc_action.h"
#include "RemoteControl.h"
#include "dvc_oled.h"
#include "../BSP/bsp_key.h"
#include "bsp_RGB_LED.h"
#include "../BSP/bsp_buzzer.h"
#include "drv_PWM.h"
#include "dvc_servo.h"
#include "BMI088driver.h"


void Task_Init(void);

void Task_loop(void);



#endif //TASK_H