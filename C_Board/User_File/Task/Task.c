#include "Task.h"

#include "drv_can.h"
#include "drv_tim.h"
#include "stm32f4xx_hal.h"
#include "drv_usart.h"
#include "dvc_dji_motor.h"
#include "dvc_vofa.h"
#include "drv_tim.h"




uint8_t UART_RxBuf[5]; // Example buffer for UART reception
uint8_t UART_Txdata[8]={10,20,30,40,50,60,70,80};

// uint8_t CAN_Txdata[8]={10,20,30,40,50,60,70,80};

DJ_Motor_t DJ_Motor3508[2];

/* 毫秒定时器 */
void MM_TIM_Callback(void)
{

    static uint16_t count;
    if (count++ > 100)
    {
        count = 0;

        // HAL_GPIO_TogglePin(GPIOH, GPIO_PIN_10);
        // UART_Print("ordive:%.3f %.3f %.3f %.3f %d\n", chassis.odrive_speed, chassis.Px, chassis.Py, Action.yaw, chassis.DJ_SetAngle[1]);
        // UART_Print("LDS30B:%.3f\n", Dist);
        // HAL_UART_Transmit(&huart3, ucRxBuffer, sizeof(ucRxBuffer), 0xffff);

        // UART_Print("1.%d 2.%d 3.%d 4.%d 5.%d 6.%d 7.%d 8.%d 9.%d 10.%d 11.%d 12.%d 13.%d 14.%d 15.%d 16.%d\n", sbus_channels[0], sbus_channels[1], sbus_channels[2], sbus_channels[3], sbus_channels[4], sbus_channels[5], sbus_channels[6], sbus_channels[7], sbus_channels[8], sbus_channels[9], sbus_channels[10], sbus_channels[11], sbus_channels[12], sbus_channels[13], sbus_channels[14], sbus_channels[15], sbus_channels[16]);
        // UART_Print("DJ:%d\n", chassis.DJ_SetAngle[1]);
        // UART_Print("sendawei:%.3f\n",);
        // UART_Print("%f %f %f\n", Action.pos_x, Action.pos_y, Action.yaw);

        // UART_Print("%f %f %f\n", chassis.setPx, chassis.setPy, chassis.setYaw);
        // UART_Print("%.2f %.2f %.2f\n", chassis.lds_y, lds2.distance, Action.offset_y);
        // UART_Print("%f %f %f %f %f\n", chassis.Px, chassis.Py, Action.pos_x, Action.pos_y, Action.yaw);

    }
}


void LDS30B_Callback(void)
{
    // if (UART_RxBuf[0] == 1)
    // {
    //     HAL_GPIO_TogglePin(GPIOH, GPIO_PIN_11);
    //     UART_RxBuf[0] = 0;
    // }
}


void Task_Init(void)
{

    AttachInterrupt_TIM(&htim7,MM_TIM_Callback); // Example: Attach a timer interrupt with no callback function
    // AttachInterrupt_UART_DMA(&huart1,DataBuff, 200,Vofa_Callback); // Example: Attach a UART interrupt with a buffer size of 128 bytes and no callback function
    // Initialize task-related resources here
    // AttachInterrupt_CAN(&hcan1, NULL); // Example: Attach a CAN interrupt with no callback function
    // AttachInterrupt_CAN(&hcan1, DJ_CAN_Callback);
    // // DJ_Init(&DJ_Motor3508[1], 1, M3508);
    // DJ_Init(&DJ_Motor3508[0], 1, M3508, PID_METHOD);
    // DJ_SetSpeed(&DJ_Motor3508[0], 3000.0f);

    HAL_TIM_Base_Start_IT(&htim7);
    // AttachInterrupt_UART(&huart1, 32, LDS30B_Callback);
    AttachInterrupt_UART_DMA(&huart1, DataBuff, 200, Vofa_Callback);
    HAL_UART_Receive_IT(&huart1, DataBuff, 200);

}

void Task_loop(void)
{
    // AttachInterrupt_TIM(&htim7,MM_TIM_Callback); // Exa

    // HAL_GPIO_TogglePin(GPIOH, GPIO_PIN_10);
    // justfloat_displaydata(DJ_Motor3508[0].setAngle, DJ_Motor3508[0].total_angle, DJ_Motor3508[0].PID_Angle.out, DJ_Motor3508[0].setSpeed, DJ_Motor3508[0].speed, DJ_Motor3508[0].PID_SpeedOfAngle.out);
    // DJ_MotorRun();

    // firewater_displaydata(DJ_Motor3508[0].setAngle, DJ_Motor3508[0].total_angle, DJ_Motor3508[0].PID_Angle.out, DJ_Motor3508[0].setSpeed, DJ_Motor3508[0].speed, DJ_Motor3508[0].PID_SpeedOfAngle.out);
    // UART_Print("%f,%f,%f,%f,%f,%f\r\n", DJ_Motor3508[0].setAngle, DJ_Motor3508[0].total_angle,DJ_Motor3508[0].PID_Angle.out,DJ_Motor3508[0].setSpeed,DJ_Motor3508[0].speed, DJ_Motor3508[0].PID_SpeedOfAngle.out);
    //
    // UART_Print("%d\r\n",DataBuff[0]);

    // UART_Print("Task loop is running...\r\n");

}