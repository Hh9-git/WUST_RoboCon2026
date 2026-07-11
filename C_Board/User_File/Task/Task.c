#include "Task.h"

/* 海泰电机调试索引 */
#define HT_A  0
#define HT_B  2
#define HT_C  5

#define MS_TIM &htim7

DJ_Motor_t DJ_Motor3508[8];
DJ_Motor_t DJ_Motor2006[2];
HT_motor_struct HT_Motors[8];

uint8_t KeyNum;
float kp1 = 0, kd1 = 0, tf1 = 0;
float gyro[3], accel[3], temp;

void MS_TIM_Callback(void)
{
    static uint16_t count;
    count++;

    Key_Tick();

    if (count % 40 == 0)
    {
        OLED_refresh_gram();
    }

    if (count % 100 == 0)
        aRGB_led_show(0x7F123456);

    if (count >= 100) count = 0;
}

void Task_Init(void)
{

    BMI088_init();
    Servo_Init();
    PWM_Init();
    Buzzer_Init();
    RGB_LED_Init();
    RGB_64LEDS_Init();
    OLED_init();

    AttachInterrupt_TIM(MS_TIM, MS_TIM_Callback);
    HAL_TIM_Base_Start_IT(MS_TIM);
    AttachInterrupt_UART_DMA(VOFA_UART, DataBuff, 200, Vofa_Callback);
    AttachInterrupt_CAN(HT_MOTOR_CAN, HT_CAN_Callback);
    AttachInterrupt_CAN(DJI_MOTOR_CAN, DJ_CAN_Callback);
    DJ_Init(&DJ_Motor3508[4], 5, M3508,PID_METHOD);//升降台底座3508电机
    DJ_Init(&DJ_Motor3508[5], 6, M3508,PID_METHOD);
    DJ_Init(&DJ_Motor2006[0], 7, M2006,PID_METHOD);

    HT_Motor_Init(&HT_Motors[7],8,&hcan2);
    Chassis_Init();
    Remote_Init();
}  
void Task_loop(void)
{
    OLED_printf(0, 0, "2set:%-4d act:%-4d", (int)DJ_Motor2006[0].setAngle, (int)DJ_Motor2006[0].angle);
    OLED_printf(1, 0, "LX:%-4d LY:%-4d", (int)Remote_control_FS.Left_X, (int)Remote_control_FS.Left_Y);
    OLED_printf(2, 0, "SWA:%-4d SWB:%-4d", (int)Remote_control_FS.SWA, (int)Remote_control_FS.SWB);
    OLED_printf(3, 0, "SWC:%-4d SWD:%-4d", (int)Remote_control_FS.SWC, (int)Remote_control_FS.SWD);
    OLED_printf(4, 0, "VA:%-4d VB:%-4d", (int)Remote_control_FS.VRA, (int)Remote_control_FS.VRB);
    HAL_Delay(2);
}