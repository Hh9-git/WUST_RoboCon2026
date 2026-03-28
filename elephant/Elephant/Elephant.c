#include "Elephant.h"
#include "Chassis.h"
#include "Remote.h"
#include "math.h"
#include "DJ_Driver.h"
#include "odrive.h"
#include "SBUS.h"
#include "Action.h"
#include "LDS30B.h"
uint8_t falg = 0;
uint8_t MM_flag = 0;
uint8_t shoot_flag = 0;
uint8_t stand_tem = 0;
/**
 * UART1 SBUS
 * UART6 ACTION
 * UART7
 * UART8 PC
 * CAN1 DJ
 * CAN1 RMDS
 * TIM7 MM
 */
/* 秒定时器 */

void MM_TIM2_Callback(void)
{
    MM_flag++;
    if (MM_flag == 1)
    {
        HAL_GPIO_WritePin(POWER2_GPIO_Port, POWER2_Pin, GPIO_PIN_SET);
    }
    else if (MM_flag == 2)
    {
        HAL_GPIO_WritePin(POWER2_GPIO_Port, POWER2_Pin, GPIO_PIN_RESET);
    }
    else
    {
        MM_flag = 0;
        HAL_TIM_Base_Stop_IT(&htim2);
    }
}

void MM_TIM3_Callback(void)
{
    static uint16_t count;

    switch (global.step)
    {
    case 1:
        if (count++ == 1000)
        {
            count = 0;
            HAL_GPIO_WritePin(POWER3_GPIO_Port, POWER3_Pin, GPIO_PIN_SET);
            global.step++;
        }
        break;

    case 2:
        if (count++ == 1000)
        {
            count = 0;
            DJ_SetAngleInc(&DJ_Motor2006[0], -130);
            DJ_SetAngleInc(&DJ_Motor2006[1], -130);
            global.step++;
            HAL_TIM_Base_Stop_IT(&htim3);
        }

        break;

    default:
        break;
    }
}
/* 毫秒定时器 */
void MM_TIM_Callback(void)
{

    static uint16_t count;
    if (count++ > 100)
    {
        count = 0;
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
        UART_Print("%f %f %f\n", chassis.offset_y, chassis.Py, lds2.distance);
    }
}

/* 大象初始化 */
inline void Elephant_Init(void)
{
    // HAL_GPIO_WritePin(POWER1_GPIO_Port, POWER1_Pin, GPIO_PIN_SET);
    // HAL_GPIO_WritePin(POWER2_GPIO_Port, POWER2_Pin, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(POWER3_GPIO_Port, POWER3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(POWER4_GPIO_Port, POWER4_Pin, GPIO_PIN_SET);
    AttachInterrupt_TIM(&htim7, MM_TIM_Callback);
    AttachInterrupt_TIM(&htim2, MM_TIM2_Callback);
    AttachInterrupt_TIM(&htim3, MM_TIM3_Callback);
    CAN_FilterInit(); // CAN过滤配置
    Chassis_Init();   // 底盘初始化
    Remote_Init();    // 遥控初始化
    HAL_TIM_Base_Start_IT(&htim2);

    /************* 初始化完成 ***********/
    // UART_Print("Initialization complete\n");
    HAL_TIM_Base_Start_IT(&htim7);
    // HAL_Delay(3000);
}

/* 大象运行循环 */
inline void Elephant_Run(void)
{
    Dis = sqrt((chassis.setPx - Action.pos_x) * (chassis.setPx - Action.pos_x) + (chassis.setPy - Action.pos_y) * (chassis.setPy - Action.pos_y));
    // Elephant_Point();
    Chassis_Run();

    // DJ_SetAngle(&DJ_Motor3508[0], chassis.DJ_SetAngle[0], 1000);
    DJ_SetAngle(&DJ_Motor3508[1], -chassis.DJ_SetAngle[1], 1000);
    odrive_set_speed(chassis.odrive_speed, -chassis.odrive_speed);
    DJ_MotorRun();
}

/*自动射环*/
void Elephant_shoot(void)
{
    HAL_TIM_Base_Start_IT(&htim2);
    shoot_flag = 1;
    DJ_SetAngleInc(&DJ_Motor2006[0], -130);
    DJ_SetAngleInc(&DJ_Motor2006[1], -130);
}

/*大象全自动*/
void Elephant_Point(void)
{
    switch (global.step)
    {
    case 0: // 取环第一个点
        chassis.PID_X.max_out = 5000;
        chassis.PID_Y.max_out = 5000;
        Chassis_SetPos(-4692, 508, 0);
        Dis = sqrt((chassis.setPx - Action.pos_x) * (chassis.setPx - Action.pos_x) + (chassis.setPy - Action.pos_y) * (chassis.setPy - Action.pos_y));

        if (Dis <= 50)
        {
            DJ_SetAngleInc(&DJ_Motor2006[0], 130);
            DJ_SetAngleInc(&DJ_Motor2006[1], 130);
            global.step++;
        }
        break;

    case 1: // 取环第二个点s
        chassis.PID_X.max_out = 1000;
        chassis.PID_Y.max_out = 1000;
        Chassis_SetPos(-5176, -100, -0.684268);
        Dis = sqrt((chassis.setPx - Action.pos_x) * (chassis.setPx - Action.pos_x) + (chassis.setPy - Action.pos_y) * (chassis.setPy - Action.pos_y));

        if (Dis <= 50)
        {
            HAL_TIM_Base_Start_IT(&htim3);
        }
        break;

    case 2: //  自动夹环
        /* 空等待 */
        break;

    case 3: // 一型柱1
        chassis.PID_X.max_out = 5000;
        chassis.PID_Y.max_out = 5000;
        Chassis_SetPos(-2982, 785, 0);

        Dis = sqrt((chassis.setPx - Action.pos_x) * (chassis.setPx - Action.pos_x) + (chassis.setPy - Action.pos_y) * (chassis.setPy - Action.pos_y));
        chassis.odrive_speed = 20.231;
        chassis.DJ_SetAngle[1] = 22;
        if (Dis <= 10 && stand_tem == 0)
        {
            Chassis_SetPos(-2982, 785 + lds2.distance - (1630 + 785), 0);
            stand_tem = 1;
        }
        if (Dis <= 10)
        {
            if (shoot_flag == 0)
            {
                HAL_TIM_Base_Start_IT(&htim2);
                shoot_flag = 1;
            }
            if (MM_flag == 2)
            {
                global.step++;
                shoot_flag = 0;
            }
        }
        break;

    case 4: // 一型柱2
        chassis.PID_X.max_out = 5000;
        chassis.PID_Y.max_out = 5000;
        Chassis_SetPos(235, 785, 0);
        Dis = sqrt((chassis.setPx - Action.pos_x) * (chassis.setPx - Action.pos_x) + (chassis.setPy - Action.pos_y) * (chassis.setPy - Action.pos_y));
        chassis.odrive_speed = 20.231;
        chassis.DJ_SetAngle[1] = 22;
        if (Dis <= 500 && stand_tem == 0)
        {
            Chassis_SetPos(-2982, 785 + lds2.distance - (1630 - 785), 0);
            stand_tem = 1;
        }
        if (Dis <= 10)
        {

            if (shoot_flag == 0)
                Elephant_shoot();
            if (MM_flag == 2)
            {
                global.step++;
                shoot_flag = 0;
            }
        }
        break;

    case 5: // 一型柱3
        chassis.PID_X.max_out = 5000;
        chassis.PID_Y.max_out = 5000;
        Chassis_SetPos(3400, 785, 0);
        Dis = sqrt((chassis.setPx - Action.pos_x) * (chassis.setPx - Action.pos_x) + (chassis.setPy - Action.pos_y) * (chassis.setPy - Action.pos_y));
        chassis.odrive_speed = 20.231;
        chassis.DJ_SetAngle[1] = 22;
        if (Dis <= 500 && stand_tem == 0)
        {
            Chassis_SetPos(-2982, 785 + lds2.distance - (1630 - 785), 0);
            stand_tem = 1;
        }
        if (Dis <= 10)
        {

            if (shoot_flag == 0)
                Elephant_shoot();
            if (MM_flag == 2)
            {
                global.step++;
                shoot_flag = 0;
            }
        }
        break;

    case 6: // 二型柱2
        chassis.PID_X.max_out = 5000;
        chassis.PID_Y.max_out = 5000;
        Chassis_SetPos(1473, 785, 0);
        Dis = sqrt((chassis.setPx - Action.pos_x) * (chassis.setPx - Action.pos_x) + (chassis.setPy - Action.pos_y) * (chassis.setPy - Action.pos_y));
        chassis.odrive_speed = 32.577;
        chassis.DJ_SetAngle[1] = 47;
        if (Dis <= 500 && stand_tem == 0)
        {
            Chassis_SetPos(-2982, 785 + lds2.distance - (1630 - 785), 0);
            stand_tem = 1;
        }
        if (Dis <= 10)
        {

            if (shoot_flag == 0)
                Elephant_shoot();
            if (MM_flag == 2)
            {
                global.step++;
                shoot_flag = 0;
            }
        }
        break;

    case 7: // 二型柱1
        chassis.PID_X.max_out = 5000;
        chassis.PID_Y.max_out = 5000;
        Chassis_SetPos(-1069.4, 785, 0);
        Dis = sqrt((chassis.setPx - Action.pos_x) * (chassis.setPx - Action.pos_x) + (chassis.setPy - Action.pos_y) * (chassis.setPy - Action.pos_y));
        chassis.odrive_speed = 32.557;
        chassis.DJ_SetAngle[1] = 47;
        if (Dis <= 500 && stand_tem == 0)
        {
            Chassis_SetPos(-2982, 785 + lds2.distance - (1630 - 785), 0);
            stand_tem = 1;
        }
        if (Dis <= 10)
        {
            if (shoot_flag == 0)
                Elephant_shoot();
            if (MM_flag == 2)
            {
                global.step++;
                shoot_flag = 0;
            }
        }
        break;

    case 8: // 三型柱1
        chassis.PID_X.max_out = 5000;
        chassis.PID_Y.max_out = 5000;
        Chassis_SetPos(235, 785, 0);
        Dis = sqrt((chassis.setPx - Action.pos_x) * (chassis.setPx - Action.pos_x) + (chassis.setPy - Action.pos_y) * (chassis.setPy - Action.pos_y));
        chassis.odrive_speed = 45.385;
        chassis.DJ_SetAngle[1] = 37;
        if (Dis <= 500 && stand_tem == 0)
        {
            Chassis_SetPos(-2982, 785 + lds2.distance - (1630 - 785), 0);
            stand_tem = 1;
        }
        if (Dis <= 10)
        {

            if (shoot_flag == 0)
                Elephant_shoot();
            if (MM_flag == 2)
            {
                global.step++;
                shoot_flag = 0;
            }
        }
        break;

    case 9: // 回到起点
        chassis.PID_X.max_out = 1000;
        chassis.PID_Y.max_out = 1000;
        Chassis_SetPos(0, 0, 0);
        Dis = sqrt((chassis.setPx - Action.pos_x) * (chassis.setPx - Action.pos_x) + (chassis.setPy - Action.pos_y) * (chassis.setPy - Action.pos_y));
        chassis.odrive_speed = 0;
        chassis.DJ_SetAngle[1] = 0;
        if (Dis <= 10)
        {
            global.step++;
        }
        break;
    default:
        break;
    }
}
