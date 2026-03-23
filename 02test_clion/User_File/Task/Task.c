/**
 * @file Task.c
 * @author Hh999999
 * @brief 任务函数，放在一个单独的文件里，main函数里调用Task_Loop()，
 * 把任务函数和main函数分开，main函数只负责调用任务函数，任务函数负责具体的任务逻辑，这样代码结构更清晰，易于维护和扩展
 * @version 1.0
 * @date 2026-03-05 23:00 1.0
 *
 * /

/* Includes ------------------------------------------------------------------*/
#include "Task.h"
#include "gpio.h"
#include "usart.h"
#include "../Device/dvc_LED.h"
#include "drv_can.h"
#include "drv_usart.h"
#include "dvc_vofa.h"
#include "dvc_remote.h"
#include "stdio.h"
#include "stm32f4xx_hal.h"


#define PRINT(window, fmt, args...) printf("{"#window"}"fmt"\n", ##args)


uint8_t tx_buffer[10];

uint8_t rx_buffer[10] = {0};


int16_t Encoder,Omega,Torgue,Temperture;

int a=0;

float f4[5]={0};

remote_t remote;

// float x_l=(float)remote.X_L;
// float y_l=(float)remote.Y_L;
// float x_r=(float)remote.X_R;
// float x_l=(float)remote.X_L;



void CAN_Motor_Call_Back(Struct_CAN_Rx_Buffer* Rx_Buffer)
{
    uint8_t* data = Rx_Buffer->Data;
    switch (Rx_Buffer->Header.StdId)
    {
    case 0x201:
        {
            // 处理CAN1电机数据
            // data[0] ~ data[7]是接收到的数据
            Encoder = (data[0] << 8) | data[1];
            Omega = (data[2] << 8) | data[3];
            Torgue = (data[4] << 8) | data[5];
            Temperture = data[6];
            // PRINT(motor, "torque: %d, speed: %d, temp: %d, current: %d", Encoder, Omega, Torgue, Temperture);
        }
    }
    // CAN1电机
    // 这个函数什么都不做，作为CAN_Init的参数传入，表示不使用CAN接收回调函数
}


// // 重定向 fputc → 串口发送
// int fputc(int ch, FILE* stream)
// {
//     // huart1 改成你实际用的串口：huart1 / huart2 / huart3
//     HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1, HAL_MAX_DELAY);
//     return ch;
// }


void Serial_Call_Back(uint8_t* Buffer, uint16_t Length)
{
    if (rx_buffer[0] == '1')
    {
        LED_red_Toggle();
        rx_buffer[0] = '0';
    }
    else if (rx_buffer[1] == '2')
    {
        LED_green_Toggle();
        rx_buffer[0] = '0';
    }
    // 这个函数什么都不做，作为UART_Init的参数传入，表示不使用UART接收回调函数
}

void Task_Init()
{
    CAN_Init(&hcan1, CAN_Motor_Call_Back);


    // CAN_Init(&hcan2, CAN_Motor_Call_Back);


    // HAL_UARTEx_ReceiveToIdle_DMA(&huart2, Rx_data, 5);


    Uart_Init(&huart2, rx_buffer, 10, Serial_Call_Back);
    remote_init(&remote);

}



void Task_Loop()
{
    while (1)
    {





        LED_red_Toggle();
        LED_green_Toggle();
        HAL_Delay(500);
        remote_task();
        __remote_debug();

        // UART2_Tx_Data[0]=remote.X_L>>8;
        // UART2_Tx_Data[1]=remote.X_L;
        // UART_Send_Data(&huart2,UART2_Tx_Data,2);

         justfloat_displaydata((float)remote.X_L,(float)remote.X_R,(float)remote.Y_L,(float)remote.Y_R,0,0);


        // uint16_t current = 3;
        // uint16_t voltage = 6;
        //
        //                                                                                                  N
        // PRINT(plotter, "%d,%d", current, voltage);
        // PRINT(power, "the power is %d W", current * voltage);
        //
        // Vofa_FireWater("%f,%f,%f\r\n", f1, f2, f3);
        // (f1 > 20) ? (f1 = 11.4) : (f1 += 0.5);
        // (f2 < 0) ? (f2 = 51.4) : (f2 -= 0.5);
        // f3 = f1 + f2;


        // f4[0]=Encoder;
        // f4[1]=Omega;
        // f4[2]=Torgue;
        // f4[3]=Temperture;
        // Vofa_JustFloat(f4, 4);

        // int16_t torque = 8000;

        // while(torque < 3000)
        // {
        //     torque += 50;
        //     CAN1_0x200_Tx_Data[0] = torque >> 8;
        //     CAN1_0x200_Tx_Data[1] = torque;
        //     CAN_Send_Data(&hcan1, 0x200, CAN1_0x200_Tx_Data, 8);
        //     HAL_Delay(50);
        // }
        // while(torque > -1000)
        // {
        //     torque -= 50;
        //     CAN1_0x200_Tx_Data[0] = torque >> 8;
        //     CAN1_0x200_Tx_Data[1] = torque;
        //     CAN_Send_Data(&hcan1, 0x200, CAN1_0x200_Tx_Data, 8);
        //     HAL_Delay(50);
        // }


        // int16_t torque = 5000;
        //
        // CAN1_0x200_Tx_Data[0] = torque >> 8;
        // CAN1_0x200_Tx_Data[1] = torque;
        // CAN_Send_Data(&hcan1, 0x200, CAN1_0x200_Tx_Data, 8);
        //
        // f4[0]=Encoder;
        // f4[1]=Omega;
        // f4[2]=Torgue;
        // f4[3]=Temperture;
        // // Vofa_JustFloat(f4, 4);
        // justfloat_displaydata(Encoder,Omega,Torgue,Temperture,0,0);
        //
        //
        // //





        // HAL_Delay(10);
        // Vofa_FireWater("%f,%f,%f,%f\r\n",Encoder, Omega, Torgue, Temperture);
        // PRINT(plotter, "%d,%d,%d,%d", Encoder, Omega, Torgue, Temperture);

        // printf("Hello, world!");
        // HAL_Delay(1);

        // JustFloat_test();

        // // LED_red_Toggle();
        // // LED_green_Toggle();
        // CAN1_0x200_Tx_Data[0]=0x01;
        // CAN1_0x200_Tx_Data[1]=0x02;
        // CAN2_0x200_Tx_Data[0]=0x11;
        // CAN2_0x200_Tx_Data[1]=0x12;

        // UART_Send_Data(&huart2, tx_buffer, 2);
        // uint8_t i;
        // for (i = 0; i < 10; i++)
        // {
        //     tx_buffer[i] = i;
        // }
        // while (tx_buffer[0] < 10)
        // {
        //     tx_buffer[0]++;
        //     HAL_Delay(100);
        // }
        //

        // static uint32_t flag;
        // // UART2_Tx_Data[0] = 0xAB;
        // if (flag == 2500)
        // {
        //     flag = 0;
        // }
        // float tmp_data;
        // tmp_data = ((float)flag / 1000.0f) * ((float)flag / 1000.0f);
        // // for (uint8_t i = 0; i < 4; i++)
        // // {
        // //     UART2_Tx_Data[i + 1] = *((char *)(&tmp_data) + i);
        // // }
        // //
        // // float led_status;
        // // led_status = !HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_1);
        // // for (uint8_t i = 0; i < 4; i++)
        // // {
        // //     UART2_Tx_Data[i + 5] = *((char *)(&led_status) + i);
        // // }
        // //
        // HAL_Delay(0);
        // flag++;
        // // UART_Send_Data(&huart2, UART2_Tx_Data, 9);
        //
        // PRINT(plotter, "%d,%d", flag, (uint16_t)tmp_data);
        // // PRINT(power, "the power is %d W", current * voltage);

        // if (rx_buffer[0]=='1')
        // {
        //     LED_red_Toggle();
        //     rx_buffer[0]='0';
        // }
        // else if (rx_buffer[0]=='2')
        // {
        //     LED_green_Toggle();
        //     rx_buffer[0]='0';
        // }
        // HAL_UART_Transmit_DMA(&huart2, Tx_data, 5);



        // CAN_Send_Data(&hcan2, 0x200, CAN2_0x200_Tx_Data, 8);
        // //

        // int16_t Rx_Encoder,Rx_Omega,Rx_Torque,Rx_Temp;
    }
}


//
// void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
// {
//     if (huart->Instance == USART2)
//     {
//         // 接收完成后再次开启接收中断
//         HAL_UARTEx_ReceiveToIdle_DMA(&huart2, Rx_data, 5);
//
//         if (Rx_data[0]=='1')
//         {
//             LED_red_Toggle();
//             Rx_data[0]='0';
//         }
//         else if (Rx_data[0]=='2')
//         {
//             LED_green_Toggle();
//             Rx_data[0]='0';
//         }
//     }
// }
