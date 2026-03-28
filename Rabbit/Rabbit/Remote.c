#include "Remote.h"
#include "stdlib.h"

float speedX = 0;
float speedY = 0;
float speedW = 0;
float speedUD = 0;
float MaxSpeedX = 1000;
float MaxSpeedY = 1000;
float MaxSpeedW = 1.5;
uint8_t MM_flag = 0;
uint8_t FF_flag = 0;
uint16_t up_flag = 0;
uint16_t down_flag = 0;
uint8_t get_loop_flag = 1;      // 第一次下降丝杆并跑点,第二次取第二堆环
uint16_t shoot_left_flag = 0;   // 左推杆
uint16_t shoot_right_flag = 0;  // 右推杆
uint16_t shoot_left2_flag = 0;  // 左上推杆
uint16_t shoot_left3_flag = 0;  // 左下推杆
uint16_t shoot_right2_flag = 0; // 右上推杆
uint16_t shoot_right3_flag = 0; // 右下推杆
uint8_t move_flag = 0;          // 手动丝杆模式
uint8_t shoot_up = 0;           // 发射次数
uint8_t stop_flag = 0;          // 急停电机和丝杆
uint8_t calibrate_flag = 0;     // 校准零点
uint16_t test_flag = 0;         // 测试零点位置准度
uint8_t zero_flag = 1;          // 电机角度清零
uint8_t urg_flag = 3;           // 紧急前进时第一次上坡,第二次手动调整好第二次取环位置并码盘清零,第三次下降到发射位置
uint8_t urg_up_flag = 0;        // 紧急上升取环
uint8_t urg_down_flag = 0;      // 紧急下降取环
uint8_t res = 1;

// 第一发 11203,11040(基准发射点)  163
// 第二发 11755.40,11592.54    162.86  (552.40,552.54)
// 第三发 12359,12190.33       168.67  (603.6,597.79)
// 第四发 13083.22,12914.18    169.04  (724.22,723.85)
// 第五发 13639.57,13473.51    166.06  (556.35,440.67)
// 第六发 14283.10,14100.89    182.21
// 第七发 14874.76,14702.6     172.16
// 第八发 15501.90,15334.01    167.89
// 第九发 16175.51,16012.49    163.02
// 第十发 16728.10,16564.68    163.42

// 第一发 11203,11040(基准发射点)
// 第二发 11795.64,11632.31
// 12411.45,12241.02
// 13055.82,12882.33
// 13720.36,13387.69
// 14375.41,14104.86
// 14926.86,14650.40
// 15553.35,15365.15
// 16166.83,16060.04
// 16772.52,16702.32
float x_arr[10] = {11203, 11795.64, 12411.45, 13055.82, 13720.36, 14375.41, 14926.86, 15553.35, 16166.83, 16772.52};
float y_arr[10] = {11040, 11632.31, 12241.02, 12882.33, 13387.69, 14104.86, 14650.40, 15365.15, 16060.04, 16702.32};
// float x_arr[10] = {11203, 11755.40, 12359, 13083.22, 13639.57, 14283.10, 14874.76, 15501.90, 16175.51, 16728.10};
// float y_arr[10] = {11040, 11592.54, 12190.33, 12914.18, 13473.51, 14100.89, 14702.6, 15334.01, 16012.49, 16564.68};

/**
 * 遥控串口中断回调函数
 */
void Remote_Callback(uint8_t *pData, uint8_t size)
{
    /**
     * 默认初始下拉
     * 手动自动通道4的切换数组
     * 对应手动时通道6小阀发射的状态数组
     * 对应手动时通道7升降的状态数组
     * 对应自动时通道5跑1,2两二型柱定点
     * 对应手动时通道5启动/取消高射炮模式
     * 对应手动时通道5发射的状态数组
     * 对应自动+高射炮时通道5启动正常情况的定点跑
     * 对应自动+高射炮时通道6启动异常情况的定点跑
     * 对应自动+高射炮时通道7上拉码盘清空，下拉...
     * 对应手动/自动射完一堆环后下降高射炮不然影响丝杆
     * 对应手动/自动急停一切电机和丝杆
     */
    static State_t state[11] = {DOWN, DOWN, DOWN, DOWN, DOWN, DOWN, DOWN, DOWN, DOWN, DOWN, DOWN};
    static uint16_t sbus_backup[16] = {0}; // 遥控器挡位滤波数组
    // static uint8_t is_initial = 0;         // 若为1,则第一次上升/下降一格变为丝杆上升到初始位置(慎用)
    static uint8_t shoot_initial = 1; // 若为1,则第一次发射为上升到发射位置

    // static uint8_t is_initial = 1;
    // if (is_initial)
    // {
    //     HAL_TIM_Base_Start_IT(&htim2);
    //     is_initial = 0;
    // }

    uint8_t sbus_dif[16];

    for (int i = 0; i < 16; i++)
    {
        int16_t dif = sbus_channels[i] - sbus_backup[i];
        dif = dif > 0 ? dif : -dif;

        sbus_dif[i] = dif < 100; // 滤波消除遥控器挡位切换突变
    }
    /* 自转速度*/
    speedW = -(sbus_channels[0] - 1000) / 800.0f * MaxSpeedW;
    /* 丝杆升降*/
    speedUD = (sbus_channels[1] - 1000) / 800.0f * MaxSpeedUD;
    /* 前后平移*/
    speedY = (sbus_channels[2] - 1000) / 800.0f * MaxSpeedY; //[-998.75,1001.25]
    /* 左右平移*/
    speedX = (sbus_channels[3] - 1000) / 800.0f * MaxSpeedX; //[-1005,995]

    /* 判断遥感在中心的阈值*/
    if (abs(sbus_channels[0] - 1000) <= 50 && sbus_dif[0])
    {
        speedW = 0;
    }
    if (abs(sbus_channels[1] - 1000) <= 50 && sbus_dif[1])
    {
        speedUD = 0;
    }
    if (abs(sbus_channels[2] - 1000) <= 50 && sbus_dif[2])
    {
        speedY = 0;
    }
    if (abs(sbus_channels[3] - 1000) <= 50 && sbus_dif[3])
    {
        speedX = 0;
    }

#if plan_A
    /* 判断三种工作模式*/
    if (IN_RANGE(sbus_channels[4], PUSH_DOWN, 10) && sbus_dif[4])
    {
        // 手动
        if (state[0] != DOWN)
        {
            state[0] = DOWN;
        }
        /*急停一切电机和丝杆*/
        if (sbus_dif[5])
        {
            if (IN_RANGE(sbus_channels[5], PUSH_DOWN, 10))
            {
                if (state[10] != DOWN)
                {
                    state[10] = DOWN;
                    stop_flag = 0;
                }
            }
            else if (IN_RANGE(sbus_channels[5], PUSH_UP, 10))
            {
                if (state[10] != UP)
                {
                    state[10] = UP;
                    stop_flag = 1;
                }
            }
        }
        /* 发射环通道6,下拉不变,上拉小阀发射*/
        if (sbus_dif[6])
        {
            if (IN_RANGE(sbus_channels[6], PUSH_DOWN, 10))
            {
                if (state[1] != DOWN)
                {
                    state[1] = DOWN;
                }
            }
            else if (IN_RANGE(sbus_channels[6], PUSH_UP, 10))
            {
                if (state[1] != UP)
                {
                    state[1] = UP;
                    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_2, RESET);
                    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_3, RESET);
                    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_4, RESET);
                    HAL_TIM_Base_Start_IT(&htim2); // 控制气缸发射
                }
            }
        }
        /* 升降对应通道7,上拉上升一格,下拉下降一格,中间不变*/
        if (sbus_dif[7])
        {
            if (IN_RANGE(sbus_channels[7], PUSH_UP, 100))
            {
                if (state[2] != UP)
                {
                    shoot_up++;
                    state[2] = UP;
                    // launch(shoot_distance + shoot_up * Shoot_UP_DOWN); // 丝杆升起一格距离
                    // DJ_SetAngle(&chassis.ChassisMotors[6], shoot_distance - 200 + 163 + shoot_up * Shoot_UP_DOWN, 15000.0f); // 163
                    // DJ_SetAngle(&chassis.ChassisMotors[7], shoot_distance - 200 + shoot_up * Shoot_UP_DOWN, 15000.0f);
                    if (res == 1)
                    {
                        DJ_SetAngle(&chassis.ChassisMotors[6], x_arr[shoot_up], 15000.0f); // 163
                        DJ_SetAngle(&chassis.ChassisMotors[7], y_arr[shoot_up], 15000.0f);
                    }
                    else if (res == 0)
                    {
                        DJ_SetAngle(&chassis.ChassisMotors[6], x_arr[shoot_up] + 100, 15000.0f); // 163
                        DJ_SetAngle(&chassis.ChassisMotors[7], y_arr[shoot_up] + 100, 15000.0f);
                    }
                    // DJ_SetSpeed(&chassis.ChassisMotors[6], 0);
                    // DJ_SetSpeed(&chassis.ChassisMotors[7], 0);
                }
            }
            else if (IN_RANGE(sbus_channels[7], PUSH_DOWN, 100))
            {
                if (state[2] != DOWN)
                {
                    shoot_up--;
                    state[2] = DOWN;
                    // launch(shoot_distance + shoot_up * Shoot_UP_DOWN); // 丝杆降落一格距离
                    // DJ_SetAngle(&chassis.ChassisMotors[6], shoot_distance - 200 + 163 + shoot_up * Shoot_UP_DOWN, 15000.0f); // 163
                    // DJ_SetAngle(&chassis.ChassisMotors[7], shoot_distance - 200 + shoot_up * Shoot_UP_DOWN, 15000.0f);
                    if (res == 1)
                    {
                        DJ_SetAngle(&chassis.ChassisMotors[6], x_arr[shoot_up], 15000.0f); // 163
                        DJ_SetAngle(&chassis.ChassisMotors[7], y_arr[shoot_up], 15000.0f);
                    }
                    else if (res == 0)
                    {
                        DJ_SetAngle(&chassis.ChassisMotors[6], x_arr[shoot_up] + 100, 15000.0f); // 163
                        DJ_SetAngle(&chassis.ChassisMotors[7], y_arr[shoot_up] + 100, 15000.0f);
                    }
                    // DJ_SetSpeed(&chassis.ChassisMotors[6], 0);
                    // DJ_SetSpeed(&chassis.ChassisMotors[7], 0);
                }
            }
            else if (IN_RANGE(sbus_channels[7], PUSH_MEDIUM, 100))
            {
                if (state[2] != MEDIUM)
                {
                    state[2] = MEDIUM;
                }
            }
        }
        /*防止高射炮破坏丝杆*/
        if (shoot_up >= 9)
        {
            if (sbus_dif[6])
            {
                if (IN_RANGE(sbus_channels[6], PUSH_DOWN, 10))
                {
                    if (state[9] != DOWN)
                    {
                        state[9] = DOWN;
                    }
                }
                else if (IN_RANGE(sbus_channels[6], PUSH_UP, 10))
                {
                    if (state[9] != UP)
                    {
                        state[9] = UP;
                        HAL_TIM_Base_Start_IT(&htim3);
                    }
                }
                else if (IN_RANGE(sbus_channels[6], PUSH_MEDIUM, 10))
                {
                    if (state[9] != MEDIUM)
                    {
                        state[9] = MEDIUM;
                    }
                }
            }
        }
        Chassis_SetVel(speedX, speedY, speedW); // 遥控底盘运动
        // 急停
        if (stop_flag == 1)
        {
            DJ_SetSpeed(&chassis.ChassisMotors[6], speedUD); // 遥控丝杆升降
            DJ_SetSpeed(&chassis.ChassisMotors[7], speedUD); // 遥控丝杆升降
            speedW = 0;                                      // 清空手动时读取的遥控 不然会导致丝杆移动时底盘运动
        }
    }

    else if (IN_RANGE(sbus_channels[4], PUSH_MEDIUM, 10) && sbus_dif[4])
    {
        // 自动(通道5 6 7 可去掉另写功能)
        if (state[0] != MEDIUM)
        {
            state[0] = MEDIUM;
            down_flag = 0;
            up_flag = 0;
            shoot_left_flag = 0;
            shoot_right_flag = 0;
            shoot_left2_flag = 0;
            shoot_right2_flag = 0;
            shoot_left3_flag = 0;
            shoot_right3_flag = 0;
        }
        /*****拨杆控制高射炮模式和跑四个定点 x∈[-1005,995] y∈[-998.75,1001.25]*****/
        if (sbus_dif[2])
        {
            // 开启高射炮模式(上拉推杆)
            if (speedY >= 800 && speedY <= 1100 && speedX >= -200 && speedX <= 200)
            {
                up_flag++;
                shoot_left_flag = 0;
                shoot_right_flag = 0;
                shoot_left2_flag = 0;
                shoot_right2_flag = 0;
                shoot_left3_flag = 0;
                shoot_right3_flag = 0;
                if (up_flag >= 50)
                {
                    up_flag = 0;
                    Move_On(); // 开启高射炮模式
                }
            }
            // 关闭高射炮模式(下拉推杆)
            else if (speedY <= -800 && speedY >= -1100 && speedX >= -200 && speedX <= 200)
            {
                down_flag++;
                shoot_left_flag = 0;
                shoot_right_flag = 0;
                shoot_left2_flag = 0;
                shoot_right2_flag = 0;
                shoot_left3_flag = 0;
                shoot_right3_flag = 0;
                if (down_flag >= 50)
                {
                    down_flag = 0;
                    Move_Off(); // 关闭高射炮模式
                }
            }
            // 跑自家左边二型柱定点(左拉推杆)
            else if (speedX <= -800 && speedX >= -1100 && speedY >= -200 && speedY <= 200)
            {
                shoot_left_flag++;
                shoot_right_flag = 0;
                shoot_left2_flag = 0;
                shoot_right2_flag = 0;
                shoot_left3_flag = 0;
                shoot_right3_flag = 0;
                if (shoot_left_flag >= 50)
                {
                    shoot_left_flag = 0;
                    Chassis_SetPos(-2878.94, -259.55, RAD(-39.65));
                    //-1746.13, 306.37, 0
                    // A.x:-1713.325195 A.y:547.044128 A.angle:-1.751151
                }
            }
            // 跑自家右边二型柱定点(右拉推杆)
            else if (speedX >= 800 && speedX <= 1100 && speedY >= -200 && speedY <= 200)
            {

                shoot_left_flag = 0;
                shoot_right_flag++;
                shoot_left2_flag = 0;
                shoot_right2_flag = 0;
                shoot_left3_flag = 0;
                shoot_right3_flag = 0;
                if (shoot_right_flag >= 50)
                {
                    shoot_right_flag = 0;
                    Chassis_SetPos(2338.52, -187.19, RAD(43.37));
                    // Chassis_SetPos(2043.23, -266.06, RAD(36.9));  //2338.518066 A.y:-187.187378 A.angle:43.371029
                }
            }
            // plan_A:跑对面左边二型柱定点(左上拉推杆)
            else if (speedY >= 800 && speedY <= 1100 && speedX >= -1100 && speedX <= -800)
            {

                shoot_left_flag = 0;
                shoot_right_flag = 0;
                shoot_left2_flag++;
                shoot_right2_flag = 0;
                shoot_left3_flag = 0;
                shoot_right3_flag = 0;
                if (shoot_left2_flag >= 50)
                {
                    shoot_left2_flag = 0;
                    // Chassis_SetPos(-2341.17, 2007.24, RAD(-19.63)); // A.x:-2320.637695 A.y:2132.239502 A.angle:-26.322838
                    Chassis_SetPos(-2401.18, 2158.76, RAD(-22.43)); // A.x:-2455.331299 A.y:1711.322754 A.angle:-19.771091
                    // A.x:-2401.185303 A.y:2158.763428 A.angle:-22.430676
                }
            }
            // plan_A:跑对面右边二型柱定点(右上拉推杆)
            else if (speedY >= 800 && speedY <= 1100 && speedX <= 1100 && speedX >= 800)
            {

                shoot_left_flag = 0;
                shoot_right_flag = 0;
                shoot_left2_flag = 0;
                shoot_right2_flag++;
                shoot_left3_flag = 0;
                shoot_right3_flag = 0;
                if (shoot_right2_flag >= 50)
                {
                    shoot_right2_flag = 0;
                    // Chassis_SetPos(1565.20, 1796.28, RAD(10.65)); // A.x:1519.210083 A.y:2068.890381 A.angle:17.855894
                    Chassis_SetPos(1469.87, 2114.75, RAD(19.49)); // 1476.253540 A.y:1945.634399 A.angle:19.558395
                    // A.x:1469.874878 A.y:2114.751953 A.angle:19.495632
                }
            }
            // plan_A:第一次丝杆下降,取第二堆环,第二次上升到发射位置(左下拉推杆)
            else if (speedY >= -1100 && speedY <= -800 && speedX >= -1100 && speedX <= -800)
            {

                shoot_left_flag = 0;
                shoot_right_flag = 0;
                shoot_left2_flag = 0;
                shoot_right2_flag = 0;
                shoot_left3_flag++;
                shoot_right3_flag = 0;
                if (shoot_left3_flag >= 50)
                {
                    if (get_loop_flag == 1)
                    {
                        get_loop_flag = 0;
                        shoot_up = 0;
                        launch(100);
                        Chassis_SetPos(-813.9, 0, 0);
                    }
                    else if (get_loop_flag == 0)
                    {
                        res = 0;
                        DJ_SetAngle(&chassis.ChassisMotors[6], shoot_distance - 100 + 163, 15000.0f); // 163
                        DJ_SetAngle(&chassis.ChassisMotors[7], shoot_distance - 100, 15000.0f);
                    }
                    shoot_left3_flag = 0;
                }
            }
            // 跑(0,0,0)点(右下拉推杆)
            else if (speedX >= 800 && speedX <= 1100 && speedY >= -1100 && speedY <= -800)
            {

                shoot_left_flag = 0;
                shoot_right_flag = 0;
                shoot_left2_flag = 0;
                shoot_right2_flag = 0;
                shoot_left3_flag = 0;
                shoot_right3_flag++;
                if (shoot_right3_flag >= 50)
                {
                    shoot_right3_flag = 0;
                    Chassis_SetPos(0, 0, 0);
                }
            }
        }
        /*急停一切电机和丝杆*/
        if (sbus_dif[5])
        {
            if (IN_RANGE(sbus_channels[5], PUSH_DOWN, 10))
            {
                if (state[10] != DOWN)
                {
                    state[10] = DOWN;
                    stop_flag = 0;
                }
            }
            else if (IN_RANGE(sbus_channels[5], PUSH_UP, 10))
            {
                if (state[10] != UP)
                {
                    state[10] = UP;
                    stop_flag = 1;
                }
            }
        }
        /* 发射环对应通道6，下拉不变，上拉小阀发射,第一次为上升丝杆到发射位置*/
        if (sbus_dif[6])
        {
            if (IN_RANGE(sbus_channels[6], PUSH_DOWN, 10))
            {
                if (state[1] != DOWN)
                {
                    state[1] = DOWN;
                }
            }
            else if (IN_RANGE(sbus_channels[6], PUSH_UP, 10))
            {
                if (state[1] != UP)
                {
                    state[1] = UP;
                    if (shoot_initial)
                    {
                        // launch(11560); // 初始值-4.39,4.63  total_angle : 17253.44,17132.21
                        DJ_SetAngle(&chassis.ChassisMotors[6], 0, 1000.0f); // 121.23 252.97
                        DJ_SetAngle(&chassis.ChassisMotors[7], 0, 1000.0f);
                        DJ_SetSpeed(&chassis.ChassisMotors[6], 0);
                        DJ_SetSpeed(&chassis.ChassisMotors[7], 0);
                        DJ_ClearAngle(&chassis.ChassisMotors[6]);
                        DJ_ClearAngle(&chassis.ChassisMotors[7]);
                        DJ_SetAngle(&chassis.ChassisMotors[6], shoot_distance - 200 + 163, 15000.0f); // 163
                        DJ_SetAngle(&chassis.ChassisMotors[7], shoot_distance - 200, 15000.0f);
                        shoot_initial = 0;
                        // 第一发 11203,11040(基准发射点)
                        // 第二发 11755.40,11592.54
                        // 第三发 12359,12190.33
                        // 第四发 13083.22,12914.18
                        // 第五发 13639.57,13473.51
                        // 第六发 14283.10,14100.89
                        // 第七发 14874.76,14702.6
                        // 第八发 15501.90,15334.01
                        // 第九发 16175.51,16012.49
                        // 第十发 16728.10,16564.68
                    }
                    else
                    {
                        HAL_GPIO_WritePin(GPIOH, GPIO_PIN_2, RESET);
                        HAL_GPIO_WritePin(GPIOH, GPIO_PIN_3, RESET);
                        HAL_GPIO_WritePin(GPIOH, GPIO_PIN_4, RESET);
                        HAL_TIM_Base_Start_IT(&htim2); // 控制气缸发射
                    }
                }
            }
        }
        /* 升降对应通道7,上拉上升一格,下拉下降一格,中间不变*/
        if (sbus_dif[7])
        {
            if (IN_RANGE(sbus_channels[7], PUSH_UP, 100))
            {
                if (state[2] != UP)
                {
                    shoot_up++;
                    state[2] = UP;
                    if (res == 1)
                    {
                        DJ_SetAngle(&chassis.ChassisMotors[6], x_arr[shoot_up] + 80, 15000.0f); // 163
                        DJ_SetAngle(&chassis.ChassisMotors[7], y_arr[shoot_up] + 80, 15000.0f);
                    }
                    else if (res == 0)
                    {
                        DJ_SetAngle(&chassis.ChassisMotors[6], x_arr[shoot_up] + 100, 15000.0f); // 163
                        DJ_SetAngle(&chassis.ChassisMotors[7], y_arr[shoot_up] + 100, 15000.0f);
                    }
                    // DJ_SetSpeed(&chassis.ChassisMotors[6], 0);
                    // DJ_SetSpeed(&chassis.ChassisMotors[7], 0);
                    // DJ_SetAngle(&chassis.ChassisMotors[6], shoot_distance - 200 + 163 + shoot_up * Shoot_UP_DOWN, 15000.0f); // 163
                    // DJ_SetAngle(&chassis.ChassisMotors[7], shoot_distance - 200 + shoot_up * Shoot_UP_DOWN, 15000.0f);
                    // launch(shoot_distance + shoot_up * Shoot_UP_DOWN); // 丝杆升起一格距离
                }
            }
            else if (IN_RANGE(sbus_channels[7], PUSH_DOWN, 100))
            {
                if (state[2] != DOWN)
                {
                    shoot_up--;
                    state[2] = DOWN;
                    if (res == 1)
                    {
                        DJ_SetAngle(&chassis.ChassisMotors[6], x_arr[shoot_up] + 80, 15000.0f); // 163
                        DJ_SetAngle(&chassis.ChassisMotors[7], y_arr[shoot_up] + 80, 15000.0f);
                    }
                    else if (res == 0)
                    {
                        DJ_SetAngle(&chassis.ChassisMotors[6], x_arr[shoot_up] + 100, 15000.0f); // 163
                        DJ_SetAngle(&chassis.ChassisMotors[7], y_arr[shoot_up] + 100, 15000.0f);
                    }
                    // DJ_SetSpeed(&chassis.ChassisMotors[6], 0);
                    // DJ_SetSpeed(&chassis.ChassisMotors[7], 0);
                    // launch(shoot_distance + shoot_up * Shoot_UP_DOWN); // 丝杆降落一格距离
                    // DJ_SetAngle(&chassis.ChassisMotors[6], shoot_distance - 200 + 163 + shoot_up * Shoot_UP_DOWN, 15000.0f); // 163
                    // DJ_SetAngle(&chassis.ChassisMotors[7], shoot_distance - 200 + shoot_up * Shoot_UP_DOWN, 15000.0f);
                }
            }
            else if (IN_RANGE(sbus_channels[7], PUSH_MEDIUM, 100))
            {
                if (state[2] != MEDIUM)
                {
                    state[2] = MEDIUM;
                }
            }
        }
        /*防止高射炮破坏丝杆*/
        if (shoot_up >= 9)
        {
            if (sbus_dif[6])
            {
                if (IN_RANGE(sbus_channels[6], PUSH_DOWN, 10))
                {
                    if (state[9] != DOWN)
                    {
                        state[9] = DOWN;
                    }
                }
                else if (IN_RANGE(sbus_channels[6], PUSH_UP, 10))
                {
                    if (state[9] != UP)
                    {
                        state[9] = UP;
                        HAL_TIM_Base_Start_IT(&htim3);
                    }
                }
                else if (IN_RANGE(sbus_channels[6], PUSH_MEDIUM, 10))
                {
                    if (state[9] != MEDIUM)
                    {
                        state[9] = MEDIUM;
                    }
                }
            }
        }
        // 急停
        if (stop_flag == 1)
        {
            DJ_SetSpeed(&chassis.ChassisMotors[6], speedUD); // 遥控丝杆升降
            DJ_SetSpeed(&chassis.ChassisMotors[7], speedUD); // 遥控丝杆升降
            speedW = 0;                                      // 清空手动时读取的遥控 不然会导致丝杆移动时底盘运动
        }
    }

    else if (IN_RANGE(sbus_channels[4], PUSH_UP, 10) && sbus_dif[4])
    {
        // 自动 + 高射炮
        if (state[0] != UP)
        {
            calibrate_flag = 0;
            test_flag = 0;
            urg_up_flag = 0;
            urg_down_flag = 0;
            
            state[0] = UP;
        }
        /*通道2,上下拨杆控制丝杆零点上升 初始上升到13000(上拉推杆)*/
        if (sbus_dif[2])
        {
            // 初始上升到13000(上拉推杆)
            if (speedY >= 800 && speedY <= 1100 && speedX >= -200 && speedX <= 200)
            {
                calibrate_flag++;
                if (calibrate_flag >= 50)
                {
                    launch(13000);
                    calibrate_flag = 0;
                }
            }
            //初始下降到-12800(上拉推杆)
            else if (speedY <= -800 && speedY >= -1100 && speedX >= -200 && speedX <= 200)
            {
                test_flag++;
                if (test_flag >= 100)
                {
                    launch(-12800);
                    test_flag = 0;
                }
            }
            // 异常时电机到发射位置(左拉推杆)
            else if (speedX <= -800 && speedX >= -1100 && speedY >= -200 && speedY <= 200)
            {
                urg_up_flag++;
                if (urg_up_flag >= 100)
                {
                    DJ_SetAngle(&chassis.ChassisMotors[6], shoot_distance - 100 + 163, 15000.0f); // 163
                    DJ_SetAngle(&chassis.ChassisMotors[7], shoot_distance - 100, 15000.0f);
                    urg_up_flag = 0;
                }
            }
            // 异常时控制丝杆到拾取环的位置(右拉推杆)
            else if (speedX >= 800 && speedX <= 1100 && speedY >= -200 && speedY <= 200)
            {

                urg_down_flag++;
                if (urg_down_flag >= 100)
                {
                    launch(100);
                    shoot_up = 0;
                    urg_down_flag = 0;
                }
            }
        }
        /* 通道5,正常跑定点模式,第一次之后为回到原点*/
        if (sbus_dif[5])
        {
            if (IN_RANGE(sbus_channels[5], PUSH_UP, 100))
            {
                if (state[6] != UP)
                {
                    if (flag == 9)
                    {

                        chassis.PID_y.Kp = 4.0f;
                        chassis.PID_x.Kp = 4.0f;
                        chassis.PID_angle.Kp = 2.0;
                        Chassis_SetPos(0, 0, 0);
                    }
                    else if (flag == 10)
                    {
                        flag = 1;
                    }
                    state[6] = UP;
                }
            }
            else if (IN_RANGE(sbus_channels[5], PUSH_DOWN, 100))
            {
                if (state[6] != DOWN)
                {
                    state[6] = DOWN;
                }
            }
        }
        /* 通道6,异常跑定点模式(flag == 11)*/
        if (sbus_dif[6])
        {
            if (IN_RANGE(sbus_channels[6], PUSH_UP, 10))
            {
                if (state[7] != UP)
                {
                    if (urg_flag == 3)
                    {
                        flag = (flag == 10) ? 11 : flag;
                        urg_flag--;
                    }
                    else if (urg_flag == 2)
                    {
                        launch(100);
                        urg_flag--;
                        // 下降丝杆到拾取位置
                    }
                    else if (urg_flag == 1)
                    {
                        Chassis_SetPos(0, 0, 0);
                        Chassis_SetVel(0, 0, 0);
                        Action_ClearData();
                        Chassis_clear(chassis);
                    }
                    state[7] = UP;
                }
            }
            else if (IN_RANGE(sbus_channels[6], PUSH_DOWN, 10))
            {
                if (state[7] != DOWN)
                {
                    state[7] = DOWN;
                }
            }
        }
        /* 通道7,上拉开启手动丝杆,下拉码盘清零,中间不变*/
        if (sbus_dif[7])
        {
            if (IN_RANGE(sbus_channels[7], PUSH_UP, 100))
            {
                if (state[8] != UP)
                {
                    move_flag = 1;
                    state[8] = UP;
                }
            }
            else if (IN_RANGE(sbus_channels[7], PUSH_DOWN, 100))
            {
                if (state[8] != DOWN)
                {
                    Chassis_SetPos(0, 0, 0);
                    Chassis_SetVel(0, 0, 0);
                    Action_ClearData();
                    Chassis_clear(chassis);
                    state[8] = DOWN;
                }
            }
            else if (IN_RANGE(sbus_channels[7], PUSH_MEDIUM, 100))
            {
                if (state[8] != MEDIUM)
                {
                    move_flag = 0;
                    state[8] = MEDIUM;
                }
            }
        }
        /* 手动丝杆模式 */
        if (move_flag)
        {
            DJ_SetSpeed(&chassis.ChassisMotors[6], speedUD); // 遥控丝杆升降
            DJ_SetSpeed(&chassis.ChassisMotors[7], speedUD); // 遥控丝杆升降
            speedW = 0;                                      // 清空手动时读取的遥控 不然会导致丝杆移动时底盘运动
        }
    }

#elif plan_B

#elif plan_C

#endif

    for (int i = 0; i < 16; i++)
    {
        sbus_backup[i] = sbus_channels[i];
    }

    if (strncmp((char *)pData, "re", 2) == 0)
    {
        HAL_NVIC_SystemReset();
    }
}

void Remote_Init(void)
{
    // AttachInterrupt_UART(&REMOTE_UART, 64, Remote_Callback);
    SBUS_Init((void *)Remote_Callback);
}
