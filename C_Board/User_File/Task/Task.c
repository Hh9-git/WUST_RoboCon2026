#include "Task.h"

/* ==================== 调试模式选择（只开一个）==================== */
#define DEBUG_IMU       0
#define DEBUG_SERVO     0
#define DEBUG_DJ_MOTOR  0
#define DEBUG_HT_MOTOR  1
#define DEBUG_REMOTE    0
/* ============================================================== */

/* 海泰电机调试索引 */
#define HT_A  0
#define HT_B  2
#define HT_C  5

#define MS_TIM &htim7

DJ_Motor_t DJ_Motor3508[4];
DJ_Motor_t DJ_Motor2006[2];
HT_motor_struct HT_Motors[8];

uint8_t KeyNum;
float kp1 = 0, kd1 = 0, tf1 = 0;
float gyro[3], accel[3], temp;

static uint8_t oled_refresh_flag = 0;
static uint8_t vofa_send_flag = 0;

void MS_TIM_Callback(void)
{
    static uint16_t count;
    count++;

    Key_Tick();

    if (count % 40 == 0)
    {
        oled_refresh_flag = 1;
        vofa_send_flag = 1;
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
    OLED_init();

    AttachInterrupt_TIM(MS_TIM, MS_TIM_Callback);
    HAL_TIM_Base_Start_IT(MS_TIM);
    AttachInterrupt_UART_DMA(VOFA_UART, DataBuff, 200, Vofa_Callback);
    AttachInterrupt_CAN(HT_MOTOR_CAN, HT_CAN_Callback);
    AttachInterrupt_CAN(DJI_MOTOR_CAN, DJ_CAN_Callback);
    Remote_Init();

#if DEBUG_DJ_MOTOR
    DJ_Init(&DJ_Motor3508[0], 1, M3508, PID_METHOD);
#endif

#if DEBUG_HT_MOTOR
    HT_Motor_Init(&HT_Motors[HT_A], (HT_A + 1), &hcan2);
    HT_Motor_Init(&HT_Motors[HT_B], (HT_B + 1), &hcan2);
    HT_Motor_Init(&HT_Motors[HT_C], (HT_C + 1), &hcan2);
    HT_SetPosition(&HT_Motors[HT_A], RAD(180), 0.5f, 0, 0);
    HT_SetPosition(&HT_Motors[HT_C], 0, 0, 0, 0.5f);
#endif
}

void Task_loop(void)
{
#if DEBUG_IMU
    BMI088_read(gyro, accel, &temp);
    OLED_printf(0, 0, "g:%.2f %.2f %.2f", gyro[0], gyro[1], gyro[2]);
    OLED_printf(1, 0, "a:%.2f %.2f %.2f", accel[0], accel[1], accel[2]);
    OLED_printf(2, 0, "t:%.2f", temp);
    HAL_Delay(10);
#endif

#if DEBUG_SERVO
    KeyNum = Key_GetNum();
    if (KeyNum == 1)
    {
        Buzzer_LongBeep();
        kp1 += 45;
        if (kp1 > 135) kp1 = 0;
        Servo_SetAngle_135(TIM_CHANNEL_1, kp1);
        Servo_SetAngle_135(TIM_CHANNEL_2, kp1);
        OLED_printf(0, 0, "Angle:%.1f", kp1);
    }
    else
    {
        Buzzer_off();
        RGB_LED_off();
    }
#endif

#if DEBUG_REMOTE
    UART_Print("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
        sbus_channels[0], sbus_channels[1], sbus_channels[2], sbus_channels[3],
        sbus_channels[4], sbus_channels[5], sbus_channels[6], sbus_channels[7],
        sbus_channels[8], sbus_channels[9]);
#endif

#if DEBUG_DJ_MOTOR
    DJ_MotorRun();
#endif

#if DEBUG_HT_MOTOR
    HT_Run(&HT_Motors[HT_A]);
    HT_Run(&HT_Motors[HT_B]);
    HT_Run(&HT_Motors[HT_C]);
    OLED_printf(0, 0, "A pos:%.3f", HT_Motors[HT_A].Position);
    OLED_printf(1, 0, "B pos:%.3f", HT_Motors[HT_B].Position);
    OLED_printf(2, 0, "C pos:%.3f", HT_Motors[HT_C].Position);
    OLED_printf(3, 0, "C tor:%.3f", HT_Motors[HT_C].Torque);
#endif

    if (oled_refresh_flag)
    {
        oled_refresh_flag = 0;
        OLED_refresh_gram();
    }
    if (vofa_send_flag)
    {
        vofa_send_flag = 0;
#if DEBUG_HT_MOTOR
        justfloat_displaydata(
            HT_Motors[HT_A].Position, HT_Motors[HT_A].Torque,
            HT_Motors[HT_B].Position, HT_Motors[HT_B].Torque,
            HT_Motors[HT_C].Position, HT_Motors[HT_C].Torque);
#endif
#if DEBUG_DJ_MOTOR
        justfloat_displaydata(
            DJ_Motor3508[0].setSpeed, DJ_Motor3508[0].speed,
            DJ_Motor3508[0].setAngle, DJ_Motor3508[0].total_angle, 0, 0);
#endif
    }
}
