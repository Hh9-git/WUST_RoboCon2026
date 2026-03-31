#include "Task.h"

#include "drv_can.h"
#include "stm32f4xx_hal.h"
#include "drv_usart.h"
#include "dvc_dji_motor.h"
#include "dvc_vofa.h"




uint8_t UART_RxBuf[128]; // Example buffer for UART reception
uint8_t UART_Txdata[8]={10,20,30,40,50,60,70,80};

// uint8_t CAN_Txdata[8]={10,20,30,40,50,60,70,80};

DJ_Motor_t DJ_Motor3508[2];
void Task_Init(void)
{
    // AttachInterrupt_UART(&huart1, 8, NULL); // Example: Attach a UART interrupt with a buffer size of 128 bytes and no callback function
    // Initialize task-related resources here
    // AttachInterrupt_CAN(&hcan1, NULL); // Example: Attach a CAN interrupt with no callback function
    AttachInterrupt_CAN(&hcan1, DJ_CAN_Callback);
    // DJ_Init(&DJ_Motor3508[1], 1, M3508);
    DJ_Init(&DJ_Motor3508[0], 1, M3508, PID_METHOD);
    DJ_SetSpeed(&DJ_Motor3508[0], 3000.0f);
}

void Task_loop(void)
{

    justfloat_displaydata(DJ_Motor3508[0].setAngle, DJ_Motor3508[0].total_angle, DJ_Motor3508[0].PID_Angle.out, DJ_Motor3508[0].setSpeed, DJ_Motor3508[0].speed, DJ_Motor3508[0].PID_SpeedOfAngle.out);
    DJ_MotorRun();

    // firewater_displaydata(DJ_Motor3508[0].setAngle, DJ_Motor3508[0].total_angle, DJ_Motor3508[0].PID_Angle.out, DJ_Motor3508[0].setSpeed, DJ_Motor3508[0].speed, DJ_Motor3508[0].PID_SpeedOfAngle.out);
    // UART_Print("%f,%f,%f,%f,%f,%f\r\n", DJ_Motor3508[0].setAngle, DJ_Motor3508[0].total_angle,DJ_Motor3508[0].PID_Angle.out,DJ_Motor3508[0].setSpeed,DJ_Motor3508[0].speed, DJ_Motor3508[0].PID_SpeedOfAngle.out);

    // UART_Print("Task loop is running...\r\n");

}