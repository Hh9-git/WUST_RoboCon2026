#include "Rabbit.h"
#include "Chassis.h"
#include "Remote.h"
#include "math.h"
#include "DJ_Driver.h"
int Dis = 0;                // 当前点与目标点距离
volatile uint8_t flag = 10; // 默认为10
extern posture_t Action;

/*自动跑到取环位置*/
void StartAction(void)
{
    Dis = sqrt((chassis.setPx - chassis.Px) * (chassis.setPx - chassis.Px) + (chassis.setPy - chassis.Py) * (chassis.setPy - chassis.Py));
    switch (flag)
    {
    case 0:
        Chassis_SetPos(0.0, 0.0, 0.0);
        if (Dis <= 0)
        {
            flag = 0;
        }
        break;
    case 1:
        // Chassis_SetPos(0.0, 4300.0, 0.0);
        Chassis_SetPos(0.0, -4300.0, 0.0);
        // launch(5000);
        launch(-8000);
        Dis = sqrt((chassis.setPx - chassis.Px) * (chassis.setPx - chassis.Px) + (chassis.setPy - chassis.Py) * (chassis.setPy - chassis.Py));
        if (Dis <= 1500) // 默认2000 调试1000
        {
            flag++;
        }
        break;
    case 2:
        // Chassis_SetPos(-4700, 4300.0, 0.0);
        Chassis_SetPos(4700, -4300.0, 0.0);
        Dis = sqrt((chassis.setPx - chassis.Px) * (chassis.setPx - chassis.Px) + (chassis.setPy - chassis.Py) * (chassis.setPy - chassis.Py));
        if (Dis <= 700)
        {
            flag++;
        }
        break;
    case 3:
        // Chassis_SetPos(-4700.0, 1850.0, 0.0);
        Chassis_SetPos(4700.0, -1850.0, 0.0);
        Dis = sqrt((chassis.setPx - chassis.Px) * (chassis.setPx - chassis.Px) + (chassis.setPy - chassis.Py) * (chassis.setPy - chassis.Py));
        if (Dis <= 500)
        {
            flag++;
        }
        break;
    case 4:
        // Chassis_SetPos(-2700.0, 1850.0, RAD(90));
        Chassis_SetPos(2700.0, -1850.0, RAD(-90)); // 向左校正
        // launch(1500);
        launch(-11500);
        Dis = sqrt((chassis.setPx - chassis.Px) * (chassis.setPx - chassis.Px) + (chassis.setPy - chassis.Py) * (chassis.setPy - chassis.Py));
        if (Dis <= 1500)
        {
            flag++;
        }
        break;
    case 5:
        // Chassis_SetPos(-2700.0, -650.0, RAD(90));
        Chassis_SetPos(2700.0, -620.0 + 637.0 + 73.0 - 46.0, RAD(-90));
        // launch(1000);
        launch(-12000);
        Dis = sqrt((chassis.setPx - chassis.Px) * (chassis.setPx - chassis.Px) + (chassis.setPy - chassis.Py) * (chassis.setPy - chassis.Py));
        // if (Dis <= 10 && (abs(chassis.ChassisMotors[6].angle - chassis.ChassisMotors[6].setAngle) <= 10) && (abs(chassis.ChassisMotors[7].angle - chassis.ChassisMotors[7].setAngle) <= 10))
        if (Dis <= 10)
        {
            flag++;
        }
        break;
    case 6:
        // Chassis_SetPos(-3200.0, -650.0, RAD(90));
        // Chassis_SetPos(3100.0, 620.0, RAD(-90));
        // launch(0);
        launch(-12800);
        Dis = sqrt((chassis.setPx - chassis.Px) * (chassis.setPx - chassis.Px) + (chassis.setPy - chassis.Py) * (chassis.setPy - chassis.Py));
        if (Dis <= 10)
        {
            flag++;
        }
        break;
    case 7:
        // Chassis_SetPos(2700.0, 600.0, RAD(-90));
        Dis = sqrt((chassis.setPx - chassis.Px) * (chassis.setPx - chassis.Px) + (chassis.setPy - chassis.Py) * (chassis.setPy - chassis.Py));
        if (Dis <= 10)
        {
            flag++;
        }
        break;
    case 8:
        Chassis_SetPos(0, 0, 0);
        Chassis_SetVel(0, 0, 0);
        Action_ClearData();
        // chassis.Px = 0;
        // chassis.Py = 0;
        Chassis_clear(chassis);

        // Action.offset_x = chassis.Px;
        // Action.offset_y = chassis.Py;
        // Action.offset_yaw = Action.yaw;

        // chassis.PID_Y.Kp = 0.05f;
        // chassis.PID_X.Kp = 0.75f;
        // chassis.PID_Angle.Kp = 0.3;
        // chassis.PID_X.out = 0;
        // chassis.PID_Y.out = 0;
        // chassis.PID_Angle.out = 0;

        flag++;
        break;

    case 11:
        Chassis_SetPos(0, 3200, 0);
        Dis = sqrt((chassis.setPx - chassis.Px) * (chassis.setPx - chassis.Px) + (chassis.setPy - chassis.Py) * (chassis.setPy - chassis.Py));
        if (Dis <= 10)
        {
            flag++;
        }
    default:
        break;
    }
}

/**
 * UART1 SBUS
 * UART6 ACTION
 * UART7
 * UART8 PC
 * CAN1 DJ
 * CAN1 RMDS
 * TIM7 MM
 */

/* 毫秒定时器 用来打印数据*/
void MM_TIM_Callback(void)
{
    static uint16_t count;
    if (count++ > 100)
    {
        count = 0;

        UART_Print("A.x:%f A.y:%f A.angle:%f setPx:%f setPy:%f\n", Action.pos_x, Action.pos_y, Action.yaw, chassis.setPx, chassis.setPy);
        UART_Print("tt_angle : %.2f,%.2f set_angle : %.2f,%.2f flag:%d\n", chassis.ChassisMotors[6].total_angle, chassis.ChassisMotors[7].total_angle, chassis.ChassisMotors[6].setAngle, chassis.ChassisMotors[7].setAngle, shoot_up);
        // UART_Print("up:%d,down:%d,speedY:%f,channel:%f\n", up_flag, down_flag, speedY, sbus_channels[2]);
        // UART_Print("c0:%d,c1:%d,c2:%d,c3:%d\n", sbus_channels[0], sbus_channels[1], sbus_channels[2], sbus_channels[3]);
        // UART_Print("x:%f,y:%f,w:%f,ud:%f\n", speedX, speedY, speedW, speedUD);
        // UART_Print("x : %.2f,y : %.2f,angle : %.2f\n", chassis.setPx,chassis.setPy,chassis.setYaw);
        // UART_Print("Ax:%f Ay:%f Ox:%f Oy:%f flag:%d setX:%f setY:%f\n", Action.pos_x, Action.pos_y, Action.offset_x, Action.offset_y, flag, chassis.setPx, chassis.setPy);

        // UART_Print("%.3f %.3f %.3f\n", Action.pos_x, Action.pos_y, Action.yaw);
        // UART_Print("pout:%.3f iout:%.3f dout:%.3f\n", chassis.PID_x.out, chassis.PID_y.out, chassis.PID_angle.out);
        // UART_Print("%.3f %.3f %.3f %.3f\n", chassis.Motors_Speed[0], chassis.Motors_Speed[1], chassis.Motors_Speed[2], chassis.Motors_Speed[3]);

        // UART_Print("pout:%.3f iout:%.3f dout:%.3f\n", chassis.PID_X.Pout, chassis.PID_X.Iout, chassis.PID_X.Dout);
        // UART_Print("dx:%.3f, dy:%.3f, da:%.3f\n", Action.pos_x - chassis.PID_X.x1, Action.pos_y - chassis.PID_Y.x1, Action.yaw - chassis.PID_Angle.x1);
    }
}

/*
 * MM_flag 0.1s自增一次 绑定TIM2
 * 新兔子可能要换下几个GPIO
 * 开小阀->大阀推出->关小阀->大阀收回->大阀两个GPIO均置低
 */
void TIM_UP_Callback(void)
{
    MM_flag++;
    if (MM_flag == 1)
    {
        HAL_GPIO_WritePin(GPIOH, GPIO_PIN_2, RESET);
    }

    if (MM_flag == 6)
    {
        HAL_GPIO_WritePin(GPIOH, GPIO_PIN_3, SET);
        HAL_GPIO_WritePin(GPIOH, GPIO_PIN_4, RESET);
    }
    if (MM_flag == 11)
    {
        HAL_GPIO_WritePin(GPIOH, GPIO_PIN_2, SET);
        HAL_GPIO_WritePin(GPIOH, GPIO_PIN_3, RESET);
        HAL_GPIO_WritePin(GPIOH, GPIO_PIN_4, SET);
    }
    if (MM_flag == 16)
    {
        MM_flag = 0;
        HAL_GPIO_WritePin(GPIOH, GPIO_PIN_2, RESET);
        HAL_GPIO_WritePin(GPIOH, GPIO_PIN_4, RESET);
        HAL_TIM_Base_Stop_IT(&htim2);
    }
}

/*
 * FF_flag 0.1s自增一次 绑定TIM3
 * 防止高射炮破坏丝杆
 */
void TIM_DOWN_Callback(void)
{
    FF_flag++;
    if (FF_flag == 25)
    {
        Move_Off();
        FF_flag = 0;
        HAL_TIM_Base_Stop_IT(&htim3);
    }
}

/* 兔子初始化 */
void Rabbit_Init(void)
{
    AttachInterrupt_TIM(&htim7, MM_TIM_Callback);   // 定时器7连接串口打印数据
    AttachInterrupt_TIM(&htim2, TIM_UP_Callback);   // 定时器2控制气缸发射
    AttachInterrupt_TIM(&htim3, TIM_DOWN_Callback); // 定时器3控制气缸发射
    CAN_FilterInit();                               // CAN过滤配置
    Chassis_Init();                                 // 底盘初始化
    Remote_Init();                                  // 遥控初始化

    /************* 初始化完成 ***********/
    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_2, RESET);
    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_3, RESET);
    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_4, RESET); // 气缸初始化
    UART_Print("Initialize Completely!\n");
    HAL_TIM_Base_Start_IT(&htim7); // 开启打印
}

/* 兔子运行循环 */
inline void Rabbit_Run(void)
{
    /* 底盘运行 */
    StartAction();
    Chassis_Run();
}
