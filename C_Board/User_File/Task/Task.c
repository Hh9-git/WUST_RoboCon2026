#include "Task.h"

#define HT_TEST_A 0
#define HT_TEST_B 2
#define HT_TEST_C 3


#define MS_TIM &htim7

DJ_Motor_t DJ_Motor3508[4];
DJ_Motor_t DJ_Motor2006[2];

HT_motor_struct HT_Motors[8];

uint8_t CAN_TX_data[8]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};

uint8_t KeyNum;

float kp1=0,kd1=0,tf1=0;

float  gyro[3], accel[3], temp;

/* 毫秒定时器 */
void MS_TIM_Callback(void)
{
    static uint16_t count;
    count++;
    /*非阻塞式按键检测*/
    Key_Tick();
    if (count%40==0)
    {
        /*******刷新OLED显示屏********/
        OLED_refresh_gram();
        justfloat_displaydata(HT_Motors[HT_TEST_A].Position,HT_Motors[HT_TEST_A].Torque,HT_Motors[HT_TEST_B].Position,HT_Motors[HT_TEST_B].Torque,HT_Motors[HT_TEST_C].Position,HT_Motors[HT_TEST_C].Torque);
        // justfloat_displaydata(HT_Motors[HT_TEST_A].setPosition,HT_Motors[HT_TEST_A].Position,HT_Motors[HT_TEST_A].Torque,0,0,0);
    }
    /***********0.1秒计时器***********/
    if (count%100==0)
    {
        /***********程序运行指示灯************/
        aRGB_led_show(0x7F123456);
    }
    if (count>100)count=0;
}


/*****任务初始化******/
void Task_Init(void)
{
    /**********IMU初始化*********/
    BMI088_init();
    /*********舵机初始化********/
    Servo_Init();
    /*********PWM初始化********/
    // PWM_Init();
    /*********蜂鸣器初始化********/
    Buzzer_Init();
    /*******RGB彩灯初始化********/
    RGB_LED_Init();
    /******OLED初始化*********/
    OLED_init();
    /*********开启定时器*********/
    AttachInterrupt_TIM(MS_TIM,MS_TIM_Callback);
    HAL_TIM_Base_Start_IT(MS_TIM);
    /*********开启VOFA*********/
    AttachInterrupt_UART_DMA(VOFA_UART,DataBuff,200,Vofa_Callback);
    /*********开启CAN**********/
    AttachInterrupt_CAN(HT_MOTOR_CAN, HT_CAN_Callback);
    AttachInterrupt_CAN(DJI_MOTOR_CAN, DJ_CAN_Callback);

    Remote_Init();

    /**大疆电机测试**/
    // DJ_Init(&DJ_Motor2006[0],5,M2006,PID_METHOD);
    // DJ_SetAngleInc(&DJ_Motor3508[0],-90);
    // DJ_SetAngle(&DJ_Motor3508[0],90,1000);
    // DJ_Init(&DJ_Motor3508[0],1,M3508,IMPEDANCE_METHOD);
    // DJ_SetImpAngle(&DJ_Motor3508[0],10,1,180,1);
    DJ_Init(&DJ_Motor3508[0],4,M3508,PID_METHOD);

    /**海泰电机测试**/
    // HT_Motor_Init(&HT_Motors[HT_TEST_A],(HT_TEST_A+1),&hcan2);
    // HT_Motor_Init(&HT_Motors[HT_TEST_B],(HT_TEST_B+1),&hcan2);
    // HT_Motor_Init(&HT_Motors[HT_TEST_C],(HT_TEST_C+1),&hcan2);
    // HT_SetTorque(&HT_Motors[HT_TEST_A],0.5);
    // HT_SetTorque(&HT_Motors[HT_TEST_B],0.3);
    // HT_SetTorque(&HT_Motors[HT_TEST_C],0.2);
    // HT_SetPosition(&HT_Motors[HT_TEST_A],RAD(180),0.5,0,0);

    // HT_Motors[HT_TEST_A].setTorque = kp1*(HT_Motors[HT_TEST_A].setPosition- HT_Motors[HT_TEST_A].Position)
    //        + kd1*(HT_Motors[HT_TEST_A].setVelocity - HT_Motors[HT_TEST_A].Velocity)
    //        + tf1;
    // HT_SetTorque(&HT_Motors[HT_TEST_A],HT_Motors[HT_TEST_A].setTorque);

    // HT_Motor_Init(&HT_Motors[HT_TEST_A],(HT_TEST_A+1),&hcan2);
    // HT_Motor_Init(&HT_Motors[HT_TEST_B],(HT_TEST_B+1),&hcan2);
    HT_Motor_Init(&HT_Motors[HT_TEST_C],(HT_TEST_C+1),&hcan2);
    // HT_SetTorque(&HT_Motors[HT_TEST_A],0.5);
    // HT_SetTorque(&HT_Motors[HT_TEST_B],0.3);
    // HT_SetTorque(&HT_Motors[HT_TEST_C],0);
    HT_SetPosition(&HT_Motors[HT_TEST_C],-0.4,3,0,3);
}


/*******任务执行循环*********/
void Task_loop(void)
{


    BMI088_read(gyro, accel, &temp);
    HAL_Delay(10);
    OLED_printf(0,0,"g1:%.3f",gyro[0]);
    OLED_printf(1,0,"g2:%.3f",gyro[1]);
    OLED_printf(2,0,"g3:%.3f",gyro[2]);
    OLED_printf(0,12,"a1:%.3f",accel[0]);
    OLED_printf(1,12,"a2:%.3f",accel[1]);
    OLED_printf(2,12,"a3:%.3f",accel[2]);
    OLED_printf(3,0,"t1:%.3f",temp);
    /*********非阻塞式按键*********/
    KeyNum=Key_GetNum();
    if (KeyNum==1)
    {
        Buzzer_LongBeep();
        kp1+=45;
        while (kp1>135){kp1=0;}
        Servo_SetAngle_135(TIM_CHANNEL_1,kp1);
        Servo_SetAngle_135(TIM_CHANNEL_2,kp1);
        /***********刷新oled显示屏***********/
        OLED_printf(0,0,"KeyNum:%.1f",kp1);
    }
    else
    {
        Buzzer_off();
        RGB_LED_off();
    }
    // justfloat_displaydata(DJ_Motor3508[0].setSpeed,DJ_Motor3508[0].speed,DJ_Motor3508[0].setAngle,DJ_Motor3508[0].total_angle,0,0);
    // UART_Print("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",sbus_channels[0],sbus_channels[1],sbus_channels[2],sbus_channels[3],sbus_channels[4],sbus_channels[5],sbus_channels[6],sbus_channels[7],sbus_channels[8],sbus_channels[9]);

    // HT_Run(&HT_Motors[HT_TEST_A]);
    // HT_Run(&HT_Motors[HT_TEST_B]);
    // HT_Run(&HT_Motors[HT_TEST_C]);
    //
    // OLED_printf(1,0,"pos1:%f",HT_Motors[HT_TEST_A].Position);
    // OLED_printf(2,0,"pos2:%f",HT_Motors[HT_TEST_B].Position);
    // OLED_printf(2,0,"Pos3:%f",HT_Motors[HT_TEST_C].setPosition);
    // OLED_printf(3,0,"Pos3:%f",HT_Motors[HT_TEST_C].Position);
    // OLED_printf(4,0,"Tor3:%f",HT_Motors[HT_TEST_C].Torque);
    // justfloat_displaydata(HT_Motors[HT_TEST_A].Position,HT_Motors[HT_TEST_A].Torque,HT_Motors[HT_TEST_B].Position,HT_Motors[HT_TEST_B].Torque,HT_Motors[HT_TEST_C].Position,HT_Motors[HT_TEST_C].Torque);
}