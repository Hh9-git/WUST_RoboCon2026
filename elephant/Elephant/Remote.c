#include "Remote.h"
#include "BSP.h"
#include <stdlib.h>
#include "FastMathFunctions.h"
#include "Chassis.h"
#include "Action.h"
#include "Elephant.h"
#include "DJ_Driver.h"
#include "SBUS.h"
#include "LDS30B.h"

#define MAX_OFFSET_X 100

int16_t pos_X, pos_y, angle, DJ_0, DJ_1;
float MaxSpeedX = 1000;
float MaxSpeedY = 1000;
float MaxSpeedW = 1.0;
int sbus_chan13;
int sbus_chan3;
int sbus_chan9;
int sbus_chan11 = 0;
int sbus_chan6 = 0;
int sbus_chan15;
int sbus_chan10 = 0;

int flag = 0;

/**
 * 遥控串口中断回调函数
 */
void Remote_Callback()
{
    if (flag == 0)
    {
        sbus_chan13 = sbus_channels[13];
        sbus_chan15 = sbus_channels[15];
        flag = 1;
    }
    /*通道5，选择模式，下拉353手动，上拉1695自动*/
    if (sbus_channels[5] == 353)
    {
        chassis.ctrlMode = VELOCITY_MODE;

        /*通道0，左右，最左353 中间1024 最右1695*/
        chassis.setVx = (sbus_channels[0] - 1024) / 671.0f * MaxSpeedX;
        /*通道1，前后，最上353 中间1024 最下1695*/
        chassis.setVy = (1024 - sbus_channels[1]) / 671.0f * MaxSpeedY;
        /*通道2，自转，最左1695 中间1024 最右353*/
        chassis.setVw = -(1024 - sbus_channels[2]) / 671.0f * MaxSpeedW;
        /*通道6，发射，下拉353收回，上拉发射1695*/
        /*通道3，云台俯仰，起始左转到底1695，右转到底353*/
        // if (sbus_chan3 != sbus_channels[3])
        // {

        chassis.DJ_SetAngle[1] = sbus_chan3 + (1024 - sbus_channels[3]) / 10;
        /*通道9，传送带速度，起始下推到底1695，上推到底353*/
        chassis.odrive_speed = sbus_chan9 + ((1024 - sbus_channels[9]) / 30.0f);

        /*通道8，上膛，下拉353推出，中间1024，上拉1695收回*/
        if (sbus_channels[8] == 353)
        {
            HAL_GPIO_WritePin(POWER2_GPIO_Port, POWER2_Pin, GPIO_PIN_SET);
        }
        else if (sbus_channels[8] == 1695)
        {
            HAL_GPIO_WritePin(POWER2_GPIO_Port, POWER2_Pin, GPIO_PIN_RESET);
        }
        /*通道6，射环，下拉353推出，中间1024，上拉1695收回*/
        if (sbus_channels[6] == 353)
        {
            HAL_GPIO_WritePin(POWER1_GPIO_Port, POWER1_Pin, GPIO_PIN_RESET);
        }
        else if (sbus_channels[6] == 1695)
        {
            HAL_GPIO_WritePin(POWER1_GPIO_Port, POWER1_Pin, GPIO_PIN_SET);
        }
    }
    /*通道13，圆环升降，起始1695，上推到底353*/
    if (sbus_channels[13] > sbus_chan13) // 上升一格
    {
        DJ_SetAngleInc(&DJ_Motor2006[0], 130);
        DJ_SetAngleInc(&DJ_Motor2006[1], 130);
    }
    else if (sbus_channels[13] < sbus_chan13) // 下降一格
    {
        DJ_SetAngleInc(&DJ_Motor2006[0], -130);
        DJ_SetAngleInc(&DJ_Motor2006[1], -130);
    }
    sbus_chan13 = sbus_channels[13];

    /*通道7，夹取，下拉353推出，中间1024，上拉1695收回*/
    if (sbus_channels[7] == 353)
    {
        HAL_GPIO_WritePin(POWER3_GPIO_Port, POWER3_Pin, GPIO_PIN_SET);
    }
    else if (sbus_channels[7] == 1695)
    {
        HAL_GPIO_WritePin(POWER3_GPIO_Port, POWER3_Pin, GPIO_PIN_RESET);
    }

    /*通道5，选择模式，下拉353手动，上拉1695自动*/
    if (sbus_channels[5] == 1695)
    {
        chassis.ctrlMode = POSITION_MODE;

        /*通道5，自动模式x方向校准，最左353 中间1024 最右1695*/
        Action.offset_x = (sbus_channels[4] - 1024) / 671.0f * MAX_OFFSET_X;

        /*SD,通道15，取环，上拉353，中间1024，下拉1695*/
        if (sbus_channels[14] == 353 && sbus_channels[11] == 1695) // 左边取环第一个点
        {
            chassis.PID_X.Kp = 5.0f;
            chassis.PID_X.max_out = 3000;
            chassis.PID_Y.max_out = 3000;
            Chassis_SetPos(-4632, 606.03, 0);
        }
        else if (sbus_channels[14] == 1695 && sbus_channels[11] == 1695) // S 左边取环第二个点
        {
            chassis.PID_X.Kp = 3.0f;
            chassis.PID_X.max_out = 400;
            chassis.PID_Y.max_out = 400;
            Chassis_SetPos(-5170.298, -28.292, RAD(-41.68));
        }
        if (sbus_channels[14] == 353 && sbus_channels[11] == 353) // 右边取环第一个点
        {
            // Chassis_SetPos(-5176, -100, -0.684268);

            // if (Dis > 500)
            chassis.PID_X.Kp = 5.0f;
            chassis.PID_X.max_out = 3000;
            chassis.PID_Y.max_out = 3000;
            Chassis_SetPos(5242.68, 682.12, 0);
            if (sbus_chan11 == 0)
            {
                HAL_GPIO_WritePin(POWER3_GPIO_Port, POWER3_Pin, GPIO_PIN_RESET);
                DJ_SetAngleInc(&DJ_Motor2006[0], 1430);
                DJ_SetAngleInc(&DJ_Motor2006[1], 1430);
                sbus_chan11 = 1;
            }
            // if (Dis < 500)
            // {
            //     Chassis_SetPos(-5300, -100, -PI / 3);
            // }
        }
        else if (sbus_channels[14] == 1695 && sbus_channels[11] == 353) // 右边取环第二个点
        {
            chassis.PID_X.Kp = 3.0f;
            chassis.PID_X.max_out = 400;
            chassis.PID_Y.max_out = 400;
            Chassis_SetPos(5753, 85.74, RAD(39.64));
        }

        if (sbus_channels[6] == 353)
        {
            sbus_chan6 = 0;

            // HAL_GPIO_WritePin(POWER1_GPIO_Port, POWER1_Pin, GPIO_PIN_RESET);
        }
        else if (sbus_channels[6] == 1695)
        {
            // HAL_GPIO_WritePin(POWER1_GPIO_Port, POWER1_Pin, GPIO_PIN_SET);
            if (sbus_chan6 == 0)
            {
                DJ_SetAngleInc(&DJ_Motor2006[0], -130);
                DJ_SetAngleInc(&DJ_Motor2006[1], -130);
                // HAL_GPIO_WritePin(POWER2_GPIO_Port, POWER2_Pin, GPIO_PIN_RESET);
                HAL_TIM_Base_Start_IT(&htim2);
                sbus_chan6 = 1;
            }
            // HAL_GPIO_WritePin(POWER2_GPIO_Port, POWER2_Pin, GPIO_PIN_SET);
            // HAL_TIM_Base_Start_IT(&htim2);
        }
        /*通道8，上膛，下拉353推出，中间1024，上拉1695收回*/
        if (sbus_channels[8] == 353)
        {
            HAL_GPIO_WritePin(POWER1_GPIO_Port, POWER1_Pin, GPIO_PIN_SET);
        }
        else if (sbus_channels[8] == 1695)
        {
            HAL_GPIO_WritePin(POWER1_GPIO_Port, POWER1_Pin, GPIO_PIN_RESET);
        }
        /*SA,SB通道11，跑定点
        SA上拉，射一型柱三个点，对应SB上拉151，中间353，下拉554，对应从左往右123个柱子
        SA中间不跑任何点823,1024,1225
        SA下拉，射靠近我方二三型柱三个点，对应SB上拉1493，SB中间1695，SB下拉1896 */
        if (sbus_channels[10] == 151) // 一型柱1
        {
            chassis.PID_X.Kp = 2.0f;
            chassis.PID_X.max_out = 3000;
            chassis.PID_Y.max_out = 3000;
            Chassis_SetPos(-2891.9, 800, 0);
            Dis = sqrt((chassis.setPx - chassis.Px) * (chassis.setPx - chassis.Px) + (chassis.setPy - chassis.Py) * (chassis.setPy - chassis.Py));
            if (Dis < 100)
            {
                if (lds2.distance < 1000 && chassis.lds_y > 100)
                {
                    chassis.offset_y = chassis.Py - (chassis.lds_y - lds2.distance);
                }
            }
            chassis.odrive_speed = 20.231;
            chassis.DJ_SetAngle[1] = 22;
        }
        else if (sbus_channels[10] == 353) // 一型柱2
        {
            chassis.PID_X.Kp = 4.0f;
            chassis.PID_X.max_out = 5000;
            chassis.PID_Y.max_out = 3000;
            Chassis_SetPos(280.46, 800, 0);
            Dis = sqrt((chassis.setPx - chassis.Px) * (chassis.setPx - chassis.Px) + (chassis.setPy - chassis.Py) * (chassis.setPy - chassis.Py));
            if (Dis < 100)
            {
                if (lds2.distance < 1000 && chassis.lds_y > 100)
                {
                    chassis.offset_y = chassis.Py - (chassis.lds_y - lds2.distance);
                }
            }
            chassis.odrive_speed = 20.231;
            chassis.DJ_SetAngle[1] = 22;
        }
        else if (sbus_channels[10] == 554) // 一型柱3
        {
            chassis.PID_X.Kp = 4.0f;
            chassis.PID_X.max_out = 3000;
            chassis.PID_Y.max_out = 3000;
            Chassis_SetPos(3488.1, 800, 0);
            Dis = sqrt((chassis.setPx - chassis.Px) * (chassis.setPx - chassis.Px) + (chassis.setPy - chassis.Py) * (chassis.setPy - chassis.Py));
            if (Dis < 100)
            {
                if (lds2.distance < 1000 && chassis.lds_y > 100)
                {
                    chassis.offset_y = chassis.Py - (chassis.lds_y - lds2.distance);
                }
            }
            chassis.odrive_speed = 20.231;
            chassis.DJ_SetAngle[1] = 22;
        }
        else if (sbus_channels[10] == 1493 && sbus_channels[11] == 1695) // 二型柱1
        {
            chassis.PID_X.Kp = 4.0f;
            chassis.PID_X.max_out = 5000;
            chassis.PID_Y.max_out = 3000;
            Chassis_SetPos(-1030.26, 800, 0);
            Dis = sqrt((chassis.setPx - chassis.Px) * (chassis.setPx - chassis.Px) + (chassis.setPy - chassis.Py) * (chassis.setPy - chassis.Py));
            if (Dis < 100)
            {
                if (lds2.distance < 1000 && chassis.lds_y > 100)
                {
                    chassis.offset_y = chassis.Py - (chassis.lds_y - lds2.distance);
                }
            }
            chassis.odrive_speed = 31.338;
            chassis.DJ_SetAngle[1] = 22;
        }
        else if (sbus_channels[10] == 1896 && sbus_channels[11] == 1695) // 二型柱2
        {
            chassis.PID_X.Kp = 4.0f;
            chassis.PID_X.max_out = 5000;
            chassis.PID_Y.max_out = 3000;
            Chassis_SetPos(1525.69, 800, 0);
            Dis = sqrt((chassis.setPx - chassis.Px) * (chassis.setPx - chassis.Px) + (chassis.setPy - chassis.Py) * (chassis.setPy - chassis.Py));
            if (Dis < 100)
            {
                if (lds2.distance < 1000 && chassis.lds_y > 100)
                {
                    chassis.offset_y = chassis.Py - (chassis.lds_y - lds2.distance);
                }
            }
            chassis.odrive_speed = 31.338;
            chassis.DJ_SetAngle[1] = 22;
        }
        else if (sbus_channels[10] == 1493 && sbus_channels[11] == 353) // 二型柱3
        {
            chassis.PID_X.Kp = 4.0f;
            chassis.PID_X.max_out = 5000;
            chassis.PID_Y.max_out = 3000;
            Chassis_SetPos(-1080.26, 800, 0);
            Dis = sqrt((chassis.setPx - chassis.Px) * (chassis.setPx - chassis.Px) + (chassis.setPy - chassis.Py) * (chassis.setPy - chassis.Py));
            if (Dis < 100)
            {
                if (lds2.distance < 1000 && chassis.lds_y > 100)
                {
                    chassis.offset_y = chassis.Py - (chassis.lds_y - lds2.distance);
                }
            }
            chassis.odrive_speed = 43.0;
            chassis.DJ_SetAngle[1] = 35;
        }
        else if (sbus_channels[10] == 1896 && sbus_channels[11] == 353) // 二型柱4
        {
            chassis.PID_X.Kp = 4.0f;
            chassis.PID_X.max_out = 5000;
            chassis.PID_Y.max_out = 3000;
            Chassis_SetPos(1485.69, 800, 0);
            Dis = sqrt((chassis.setPx - chassis.Px) * (chassis.setPx - chassis.Px) + (chassis.setPy - chassis.Py) * (chassis.setPy - chassis.Py));
            if (lds2.distance < 1000 && Dis < 100)
            {
                if (chassis.lds_y > 100)
                {
                    chassis.offset_y = chassis.Py - (chassis.lds_y - lds2.distance);
                }
            }
            chassis.odrive_speed = 43.0;
            chassis.DJ_SetAngle[1] = 35;
        }
        else if (sbus_channels[10] == 1695) // 三型柱
        {
            chassis.PID_X.Kp = 4.0f;
            chassis.PID_X.max_out = 5000;
            chassis.PID_Y.max_out = 3000;
            Chassis_SetPos(280.46, 800, 0);
            Dis = sqrt((chassis.setPx - chassis.Px) * (chassis.setPx - chassis.Px) + (chassis.setPy - chassis.Py) * (chassis.setPy - chassis.Py));
            if (lds2.distance < 1000 && Dis < 100)
            {
                if (chassis.lds_y > 100)
                {
                    chassis.offset_y = chassis.Py - (chassis.lds_y - lds2.distance);
                }
            }
            chassis.odrive_speed = 41.4;
            chassis.DJ_SetAngle[1] = 25;
        }
        else if (sbus_channels[10] == 823) //
        {
            chassis.PID_X.Kp = 3.0f;
            chassis.PID_X.max_out = 1000;
            chassis.PID_Y.max_out = 1000;
            Chassis_SetPos(0, 0, 0);
        }
        // else if (sbus_channels[10] == 1225) //
        // {
        //     Chassis_SetPos(-5219.76, -14.1, RAD(-30.49));
        // }
        sbus_chan3 = chassis.DJ_SetAngle[1];
        sbus_chan9 = chassis.odrive_speed;
    }
    /*通道16，复位，下拉1695复位*/
    if (sbus_channels[15] == 1695 && sbus_chan15 != sbus_channels[15])
    {
        NVIC_SystemReset();
    }
    sbus_chan15 = sbus_channels[15];
    if (sbus_channels[12] == 353)
    {
        // Elephant_Point();
    }
    // UART_Print("1.%d 2.%d 3.%d 4.%d 5.%d 6.%d 7.%d 8.%d 9.%d 10.%d 11.%d 12.%d 13.%d 14.%d 15.%d 16.%d\n ", sbus_channels[0], sbus_channels[1], sbus_channels[2], sbus_channels[3], sbus_channels[4], sbus_channels[5], sbus_channels[6], sbus_channels[7], sbus_channels[8], sbus_channels[9], sbus_channels[10], sbus_channels[11], sbus_channels[12], sbus_channels[13], sbus_channels[14], sbus_channels[15]);
}

void Remote_Init(void)
{
    SBUS_Init(Remote_Callback);
}
