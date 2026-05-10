#include "Task.h"

#define HT_TEST_A 0
#define HT_TEST_B 2
#define HT_TEST_C 7
DJ_Motor_t DJ_Motor3508[4];
DJ_Motor_t DJ_Motor2006[2];

HT_motor_struct HT_Motors[8];

uint8_t CAN_TX_data[8]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};


uint8_t KeyNum;

float kp1=1,kd1=0,tf1=0;
/* 毫秒定时器 */
void MM_TIM_Callback(void)
{
    static uint16_t count;
    /*非阻塞式按键检测*/
    Key_Tick();
    /***********0.1秒计时器***********/
    if (count++ > 100)
    {
        /***********程序运行指示灯************/
        // HAL_GPIO_TogglePin(GPIOH, GPIO_PIN_10);
        count = 0;
        /*******刷新OLED显示屏********/
        OLED_refresh_gram();
        // justfloat_displaydata(HT_Motors[HT_TEST_A].Position,HT_Motors[HT_TEST_A].Torque,HT_Motors[HT_TEST_B].Position,HT_Motors[HT_TEST_B].Torque,HT_Motors[HT_TEST_C].Position,HT_Motors[HT_TEST_C].Torque);
        // justfloat_displaydata(HT_Motors[HT_TEST_A].setPosition,HT_Motors[HT_TEST_A].Position,HT_Motors[HT_TEST_A].Torque,0,0,0);
    }
}


/*****任务初始化******/
void Task_Init(void)
{

    /*******RGB彩灯初始化********/
    RGB_LED_Init();
    /******OLED初始化*********/
    OLED_init();
    /*********开启定时器*********/
    AttachInterrupt_TIM(&htim7,MM_TIM_Callback);
    HAL_TIM_Base_Start_IT(&htim7);
    /*********开启VOFA*********/
    AttachInterrupt_UART_DMA(&VOFA_UART,DataBuff,200,Vofa_Callback);
    /*********开启CAN**********/
    AttachInterrupt_CAN(&hcan2, HT_CAN_Callback);
    AttachInterrupt_CAN(&hcan1, DJ_CAN_Callback);

    Remote_Init();

    /**大疆电机测试**/
    // DJ_Init(&DJ_Motor2006[0],5,M2006,PID_METHOD);
    // DJ_SetAngleInc(&DJ_Motor3508[0],-90);
    // DJ_SetAngle(&DJ_Motor3508[0],90,1000);
    // DJ_Init(&DJ_Motor3508[0],1,M3508,IMPEDANCE_METHOD);
    // DJ_SetImpAngle(&DJ_Motor3508[0],10,1,180,1);

    /**海泰电机测试**/
    HT_Motor_Init(&HT_Motors[HT_TEST_A],(HT_TEST_A+1),&hcan2);
    HT_Motor_Init(&HT_Motors[HT_TEST_B],(HT_TEST_B+1),&hcan2);
    HT_Motor_Init(&HT_Motors[HT_TEST_C],(HT_TEST_C+1),&hcan2);
    HT_SetTorque(&HT_Motors[HT_TEST_A],0.1);
    HT_SetTorque(&HT_Motors[HT_TEST_B],0.3);
    HT_SetTorque(&HT_Motors[HT_TEST_C],0.2);
    // HT_SetPosition(&HT_Motors[HT_TEST_A],RAD(30),0,0,0);

    // HT_Motors[HT_TEST_A].setTorque = kp1*(HT_Motors[HT_TEST_A].setPosition- HT_Motors[HT_TEST_A].Position)
    //        + kd1*(HT_Motors[HT_TEST_A].setVelocity - HT_Motors[HT_TEST_A].Velocity)
    //        + tf1;
    // HT_SetTorque(&HT_Motors[HT_TEST_A],HT_Motors[HT_TEST_A].setTorque);
}


/*******任务执行循环*********/
void Task_loop(void)
{

    /*********非阻塞式按键*********/
    KeyNum=Key_GetNum();
    if (KeyNum==1)
    {
        aRGB_led_show(0x7F123456);
        OLED_LOGO();
    }
    else
    {
        RGB_LED_off();
    }
    /***********刷新oled显示屏***********/
    OLED_printf(0,0,"hello,world");
    OLED_printf(0,10,"hello,world");
    OLED_printf(1,10,"hello,world");
    OLED_printf(1,0,"KeyNum:%d",KeyNum);
    OLED_printf(2,10,"hello,world");
    OLED_printf(3,10,"hello,world");
    OLED_printf(4,10,"hello,world");

    // justfloat_displaydata(DJ_Motor3508[0].setSpeed,DJ_Motor3508[0].speed,DJ_Motor3508[0].setAngle,DJ_Motor3508[0].total_angle,0,0);
    // UART_Print("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",sbus_channels[0],sbus_channels[1],sbus_channels[2],sbus_channels[3],sbus_channels[4],sbus_channels[5],sbus_channels[6],sbus_channels[7],sbus_channels[8],sbus_channels[9]);

    HT_Run(&HT_Motors[HT_TEST_A]);
    HT_Run(&HT_Motors[HT_TEST_B]);
    HT_Run(&HT_Motors[HT_TEST_C]);

    // justfloat_displaydata(HT_Motors[HT_TEST_A].Position,HT_Motors[HT_TEST_A].Torque,HT_Motors[HT_TEST_B].Position,HT_Motors[HT_TEST_B].Torque,HT_Motors[HT_TEST_C].Position,HT_Motors[HT_TEST_C].Torque);
}