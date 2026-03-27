#include "Remote.h"
#include "BSP.h"
#include <stdlib.h>
#include "FastMathFunctions.h"
#include "Chassis.h"
#include "Action.h"
#include "DJ_Driver.h"
int16_t pos_X, pos_y, angle, DJ_0, DJ_1;
// int16_t ActionX = -999;
// int16_t ActionY = 888;
// int16_t ActionA = 180;
/**
 * 遥控串口中断回调函数
 */
void Remote_Callback(uint8_t *pData, uint8_t size)
{
    // UART_Print("hello");
    remote.ID = pData[0];
    remote.mode = pData[1];

    /*手动操作模式*/
    if (remote.mode == 1)
    {
        HAL_UART_Transmit(&huart6, &remote.ID, 1, 0xffff);
        HAL_UART_Transmit(&huart6, &pData[1], 1, 0xffff);

        if (pData[2] || pData[3]) // 前后
        {
            if (pData[2] == 0x01)
            {
                pos_y = 800;
            }
            if (pData[3] == 0x01)
            {
                pos_y = -800;
            }
        }
        else
        {
            pos_y = 0;
        }
        if (pData[4] || pData[5]) // 左右
        {
            if (pData[4] == 0x01)
            {
                pos_X = -800;
            }

            if (pData[5] == 0x01)
            {
                pos_X = 800;
            }
        }
        else
        {
            pos_X = 0;
        }
        if (pData[6] || pData[7]) // 左右自转
        {
            if (pData[6] == 0x01)
            {
                angle = -100;
            }

            if (pData[7] == 0x01)
            {
                angle = 100;
            }
        }
        else
        {
            angle = -0;
        }
        if (pData[10] || pData[11]) // 云台左右
        {
            if (pData[10] == 0x01)
            {
                DJ_0 = DJ_0 + 5;
            }
            if (pData[11] == 0x01)
            {
                DJ_0 = DJ_0 - 5;
            }
        }
        if (pData[8] || pData[9]) // 云台俯仰
        {
            if (pData[8] == 0x01)
            {
                DJ_1 = DJ_1 + 5;
            }
            if (pData[9] == 0x01)
            {
                DJ_1 = DJ_1 - 5;
            }
        }
        if (pData[12] || pData[13]) // 传送带加减速
        {
            if (pData[12] == 0x01)
            {
                chassis.odrive_speed = chassis.odrive_speed + 2;
            }
            if (pData[13] == 0x01)
            {
                chassis.odrive_speed = chassis.odrive_speed - 2;
            }
        }
        if (pData[14] == 0x01) // 射环
        {
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, GPIO_PIN_SET);
        }
        else
        {
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, GPIO_PIN_RESET);
        }
        chassis.Vx = pos_X;
        chassis.Vy = pos_y;
        chassis.Vangle = angle;
        chassis.DJ_SetAngle[0] = DJ_0;

        chassis.DJ_SetAngle[1] = DJ_1;
    }
    /*全自动模式，1表示数据为正，0表示数据为负*/
    if (remote.mode == 0)
    {
        chassis.Px = ((int16_t)pData[3]) << 8 | pData[4]; // x坐标
        if (pData[2] == 1)
            chassis.Px = -chassis.Px;
        chassis.Py = ((int16_t)pData[5]) << 8 | pData[6]; // y坐标
        chassis.yaw = ((int16_t)pData[8]);                // 角度
        if (pData[7] == 1)
            chassis.yaw = -((int16_t)pData[8]);
        chassis.DJ_SetAngle[0] = (int16_t)pData[10]; // 云台偏角
        if (pData[9] == 1)
            chassis.DJ_SetAngle[0] = -(int16_t)pData[10];
        chassis.DJ_SetAngle[1] = (int16_t)pData[11]; // 云台俯仰角
        chassis.odrive_speed = pData[12];            // 传送带速度

        HAL_UART_Transmit(&huart6, &remote.ID, 1, 0xffff);
        HAL_UART_Transmit(&huart6, &pData[1], 1, 0xffff);

        // UART_Print("%d %d %d %d %d %d", chassis.Px, chassis.Py, chassis.yaw, chassis.DJ_SetAngle[0], chassis.DJ_SetAngle[1], chassis.odrive_speed);
    }
    /*紧急制动模式，mode=99，1表示制动，0无操作*/
    if (remote.mode == 99)
    {
        HAL_UART_Transmit(&huart6, &remote.ID, 1, 0xffff);
        HAL_UART_Transmit(&huart6, &remote.mode, 1, 0xffff);

        chassis.odrive_speed = 0;
        DJ_ClearAngle(&DJ_Motor3508[0]);
        DJ_ClearAngle(&DJ_Motor3508[1]);
        DJ_SetAngle(&DJ_Motor3508[0], 0, 1000);
        DJ_SetAngle(&DJ_Motor3508[1], 0, 1000);
        // UART_Print("%d %d %d %d %d %d", chassis.Px, chassis.Py, chassis.yaw, chassis.DJ_SetAngle[0], chassis.DJ_SetAngle[1], chassis.odrive_speed);
    }
    /*复位模式，mode=98，1表示复位，0无操作*/
    if (remote.mode == 98)
    {
        HAL_UART_Transmit(&huart6, &remote.ID, 1, 0xffff);
        HAL_UART_Transmit(&huart6, &remote.mode, 1, 0xffff);
        if (pData[2] == 1)
            NVIC_SystemReset();
    }

    /*发射模式，mode=100，1表示制动，0无操作*/
    if (remote.mode == 100)
    {
        HAL_UART_Transmit(&huart6, &remote.ID, 1, 0xffff);
        HAL_UART_Transmit(&huart6, &remote.mode, 1, 0xffff);

        if (pData[2] == 1)
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, GPIO_PIN_SET); // 整理圆环
        if (pData[2] == 0)
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, GPIO_PIN_RESET); // 整理圆环
    }
    // HAL_UART_Transmit(&huart6, pData, 12, 0xffff);
    if (remote.mode == 10)
    {
        remote.Data[0] = remote.ID;
        remote.Data[1] = remote.mode;

        remote.pos_x = (int16_t)Action.pos_x;
        remote.pos_y = (int16_t)Action.pos_y;
        remote.angle = (int16_t)Action.yaw;

        if (remote.pos_x >= 0)
        {
            remote.Data[2] = 0;
            remote.Data[3] = (uint8_t)(remote.pos_x >> 8 & 0xFF);
            remote.Data[4] = (uint8_t)(remote.pos_x & 0xFF);
        }
        if (remote.pos_x < 0)
        {
            remote.Data[2] = 1;
            remote.pos_x = -remote.pos_x;
            remote.Data[3] = (uint8_t)(remote.pos_x >> 8 & 0xFF);
            remote.Data[4] = (uint8_t)(remote.pos_x & 0xFF);
        }

        if (remote.pos_y >= 0)
        {
            remote.Data[5] = 0;
            remote.Data[6] = (uint8_t)(remote.pos_y >> 8 & 0xFF);
            remote.Data[7] = (uint8_t)(remote.pos_y & 0xFF);
        }
        if (remote.pos_y < 0)
        {
            remote.Data[5] = 1;
            remote.pos_y = -remote.pos_y;
            remote.Data[6] = (uint8_t)(remote.pos_y >> 8 & 0xFF);
            remote.Data[7] = (uint8_t)(remote.pos_y & 0xFF);
        }

        if (remote.angle >= 0)
        {
            remote.Data[8] = 0;
            remote.Data[9] = (uint8_t)(remote.angle & 0xFF);
        }
        if (remote.angle < 0)
        {
            remote.Data[8] = 1;
            remote.angle = -remote.angle;

            remote.Data[9] = (uint8_t)(remote.angle & 0xFF);
        }
        HAL_UART_Transmit(&huart1, remote.Data, 10, 0xFFFF);
    }
    if (remote.mode == 102) // 拾取操作
    {
        HAL_UART_Transmit(&huart6, &remote.ID, 1, 0xffff);
        HAL_UART_Transmit(&huart6, &remote.mode, 1, 0xffff);
        if (pData[2] == 0) // 下降到最低位置
        {
            DJ_SetAngle(&DJ_Motor2006[0], -1200, 1000);
            DJ_SetAngle(&DJ_Motor2006[1], 1200, 1000);
            DJ_ClearAngle(&DJ_Motor2006[0]);
            DJ_ClearAngle(&DJ_Motor2006[1]);
        }
        if (pData[2] == 1) // 拾取夹
        {
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, GPIO_PIN_SET);
        }
        if (pData[2] == 2) // 拾取张开
        {
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, GPIO_PIN_RESET);
        }
        if (pData[2] == 3) // 抬到最高位置
        {
            DJ_SetAngle(&DJ_Motor2006[0], 120, 1000);
            DJ_SetAngle(&DJ_Motor2006[1], -120, 1000);
            DJ_ClearAngle(&DJ_Motor2006[0]);
            DJ_ClearAngle(&DJ_Motor2006[1]);
        }
        if (pData[2] == 4) // 上升一个环大小
        {
            DJ_SetAngle(&DJ_Motor2006[0], 120, 1000);
            DJ_SetAngle(&DJ_Motor2006[1], -120, 1000);
            DJ_ClearAngle(&DJ_Motor2006[0]);
            DJ_ClearAngle(&DJ_Motor2006[1]);
        }

        if (pData[2] == 5) // 整体降
        {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
        }

        if (pData[2] == 6) // 整体升
        {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
        }
    }
    if (remote.mode == 103) // 上膛操作
    {
        HAL_UART_Transmit(&huart6, &remote.ID, 1, 0xffff);
        HAL_UART_Transmit(&huart6, &remote.mode, 1, 0xffff);
        if (pData[2] == 0) // 收回
        {
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_SET);
        }
        if (pData[2] == 1) // 出
        {
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_RESET);
        }
        if (pData[2] == 2) // 发射不夹
        {
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, GPIO_PIN_SET);
        }
        if (pData[2] == 3) // 发射夹住
        {
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, GPIO_PIN_RESET);
        }
    }
}
