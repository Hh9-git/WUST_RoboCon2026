# C_Board — 武汉科技大学 RoboCon 机器人控制板固件

基于 STM32F407 的全向底盘 + 机械臂嵌入式控制系统，采用裸机超级循环架构，支持 DJI RoboMaster 电机、海泰电机、SBUS 遥控器、BMI088 IMU 等外设。

---

## 目录

- [硬件平台](#硬件平台)
- [工程结构](#工程结构)
- [软件架构](#软件架构)
- [模块说明](#模块说明)
- [通信协议](#通信协议)
- [构建与烧录](#构建与烧录)
- [已知问题](#已知问题)

---

## 硬件平台

| 项目 | 规格 |
|---|---|
| MCU | STM32F407IGHx (ARM Cortex-M4, 168MHz, FPU) |
| FLASH / RAM | 1024KB / 192KB (128KB + 64KB CCM) |
| 时钟 | HSE 12MHz → PLL → 168MHz 系统时钟 |
| 调试接口 | SWD (ST-Link) |

### 外设分配总览

| 外设 | 引脚 | 用途 |
|---|---|---|
| **CAN1** (1Mbps) | PD0/PD1 | DJI M3508/M2006 电机总线 |
| **CAN2** (1Mbps) | PB5/PB6 | 海泰 8108 电机总线 (MIT 协议) |
| **USART3** (100kbaud) | PC10/PC11 | FS-i6X SBUS 遥控器 |
| **USART1** (115200) | PA9/PB7 | Vofa+ 调试 / Action 计程模块 |
| **USART6** (115200) | PG14/PG9 | printf 调试输出 |
| **SPI1** (328kHz) | PB3/PB4/PA7 | BMI088 IMU (加速度计+陀螺仪) |
| **I2C2** (400kHz) | PF0/PF1 | SSD1306 OLED 显示屏 (0.96", 128×64) |
| **TIM1** (50Hz PWM) | PE9/PE11/PE13/PE14 | 舵机控制 (4通道, ±135°) |
| **TIM4** (CH3) | PD14 | 蜂鸣器 |
| **TIM5** (CH1-3) | PH10/PH11/PH12 | RGB LED |
| **TIM7** (1ms 中断) | 内部 | 系统定时 (按键、OLED 刷新、LED) |
| **TIM8** (1kHz) | 内部 | 64-LED 灯带 (CH3) |
| **GPIO 输出** | PC6 / PI6 / PI7 | 气动夹爪 / 推杆 / 吸盘 |
| **GPIO 输入** | PA0 | 用户按键 |
| **EXTI4** | PC4 | BMI088 加速度计数据就绪 |
| **EXTI9_5** | PC5 | BMI088 陀螺仪数据就绪 |

### DMA 通道分配

| DMA Stream | 通道 | 用途 |
|---|---|---|
| DMA2_Stream2/Ch4 | USART1_RX | Vofa+ / Action 接收 |
| DMA2_Stream7/Ch4 | USART1_TX | Vofa+ / Action 发送 |
| DMA1_Stream1/Ch4 | USART3_RX | SBUS 接收 |
| DMA2_Stream1/Ch5 | USART6_RX | 调试串口接收 |
| DMA2_Stream0/Ch3 | SPI1_RX | BMI088 接收 |
| DMA2_Stream3/Ch3 | SPI1_TX | BMI088 发送 |

---

## 工程结构

```
C_Board/
├── C_Board.ioc                 # STM32CubeMX 工程文件 (MX 6.16.1)
├── CMakeLists.txt              # 顶层 CMake 构建脚本
├── CMakePresets.json            # CMake 预设 (Debug/Release)
├── STM32F407XX_FLASH.ld        # GCC 链接脚本
├── stlink.cfg                  # ST-Link 调试配置
│
├── Core/                       # STM32CubeMX 自动生成代码
│   ├── Inc/                    #   外设头文件 (main.h, can.h, gpio.h, ...)
│   └── Src/                    #   外设初始化 + 中断向量 + main()
│
├── Drivers/                    # ST 官方驱动库
│   ├── CMSIS/                  #   ARM CMSIS 核心头文件
│   └── STM32F4xx_HAL_Driver/   #   HAL 库源码
│
└── User_File/                  # 用户代码 ★
    ├── BSP/                    # 板级支持包 (按键、LED、蜂鸣器、BMI088)
    ├── Middleware/
    │   ├── Driver/             #   底层驱动抽象 (CAN、UART、TIM、PWM)
    │   └── Algorithm/          #   控制算法 (PID、快速数学)
    ├── Device/                 # 设备驱动 (DJI电机、海泰电机、遥控器、OLED、舵机、Vofa+)
    └── Task/                   # 应用层任务 (主循环、底盘、遥控、机械臂)
```

---

## 软件架构

采用四层分层架构，裸机超级循环 + 定时器中断驱动：

```
┌─────────────────────────────────────────────────┐
│                  应用层 (Task)                     │
│   Task.c / Chassis.c / RemoteControl.c / Arm.c   │
├─────────────────────────────────────────────────┤
│                 设备层 (Device)                    │
│  dvc_dji_motor / dvc_HT_motor / dvc_remote /     │
│  dvc_oled / dvc_servo / dvc_vofa / dvc_action    │
├─────────────────────────────────────────────────┤
│               中间件 (Middleware)                  │
│  Driver: drv_can / drv_usart / drv_tim / drv_PWM │
│  Algorithm: alg_pid / alg_fastmath               │
├─────────────────────────────────────────────────┤
│                板级支持 (BSP)                      │
│  bsp_key / bsp_RGB_LED / bsp_buzzer / BMI088     │
└─────────────────────────────────────────────────┘
```

### 运行流程

```
main()
  ├── HAL / 时钟 / 外设初始化 (CubeMX 生成)
  └── Task_Init() → Task_loop() 主循环
        │
        ├── Task_Init(): 初始化所有模块
        │     BMI088 → Servo → PWM → Buzzer → LED → OLED
        │     → 注册 TIM7 中断回调
        │     → 注册 Vofa+ / 海泰 / DJI 电机 CAN 回调
        │     → 初始化电机 (3508 #5,#6 / 2006 #7 / HT #8)
        │     → Chassis_Init → Remote_Init
        │
        └── Task_loop(): 超级循环
              OLED 显示 → HAL_Delay(2ms)

中断驱动的异步任务:
  ├── TIM7 (1ms)  → 按键扫描 / OLED 刷新(40ms) / LED 更新(100ms)
  ├── USART3 DMA  → SBUS 解码 → Remote_Callback() → 底盘 + 电机控制
  ├── CAN1 RX     → DJI 电机反馈解析 → 角度/速度/电流更新
  └── CAN2 RX     → 海泰电机反馈解析
```

---

## 模块说明

### 1. DJI 电机驱动 (`dvc_dji_motor`)

支持 DJI RoboMaster **M3508** 和 **M2006** 两种电机，通过 CAN1 总线通信。

- **电机数量**: 最多 8 个 (ID 0x201-0x208)
- **减速比**: M3508 = 19:1，M2006 = 36:1
- **编码器**: 13 位 (8192 count/rev)，支持多圈角度累计
- **控制模式**:
  - `SPEED_MODE` — 单环速度 PID
  - `ANGLE_MODE` — 串级 PID (TDPID 角度外环 → PID 速度内环)
  - `ANGLEINC_MODE` — 增量角度控制
  - `CURRENT_MODE` / `IMPEDANCE_METHOD` — 直接电流 / 阻抗控制
- **串级 PID 结构**:

```
目标角度 → [TDPID 角度环] → 目标速度 → [PID 速度环] → 电流指令 → 电机
```

- **M2006 默认 PID 参数**:

| 控制器 | Kp | Ki | Kd | max_out |
|---|---|---|---|---|
| 角度环 (TDPID) | 150.0 | 0.5 | 10.0 | 5000 |
| 速度环 (PID) | 5.0 | 0.0 | 0.0 | 16000 |

### 2. 海泰电机驱动 (`dvc_HT_motor`)

支持海泰 8108 电机，通过 CAN2 使用 MIT 协议通信。

- **协议**: MIT (位置+速度+Kp+Kd+力矩 打包为 8 字节)
- **控制**: 位置模式 (`HT_SetPosition`) / 力矩模式 (`HT_SetTorque`)
- **参数范围**: 位置 ±95.5 rad，速度 ±45 rad/s，Kp 0-500，Kd 0-30，力矩 ±18 N·m
- **使能命令**: 0xFC (使能) / 0xFD (失能) / 0xFE (设置零位)

### 3. 底盘控制 (`Chassis`)

4 轮麦克纳姆轮 / 全向轮底盘。

- **轮布局**: 4 个 45° 安装的全向轮，轮径 76mm，中心距 281mm
- **运动学**: 逆运动学将 (Vx, Vy, Vw) 分解为 4 轮转速，应用 √2/2 旋转矩阵
- **控制模式**:
  - `VELOCITY_MODE` — 速度闭环 (mm/s, rad/s)
  - `POSITION_MODE` — 位置闭环 (mm, rad)
- **电机**: DJI M3508 × 4 (CAN1, ID 1-4)

### 4. 遥控器接口 (`RemoteControl`)

解析 FlySky FS-i6X 的 SBUS 信号，映射到各执行器。

| 遥控通道 | 功能 |
|---|---|
| 右摇杆 X/Y | 底盘前后/左右平移 |
| 左摇杆 X | 底盘自转 |
| 左摇杆 Y | 3508 升降台角度 |
| VRA 旋钮 | 海泰电机 (云台) 角度 |
| VRB 旋钮 | 2006 夹爪旋转角度 |
| SWA | 急停 (向下 = 停机) |
| SWB | 气动夹爪开/合 |
| SWC | 推杆开关 |
| SWD | 舵机 + 吸盘开关 |

### 5. 机械臂 (`Arm`)

2-DOF 平面机械臂阻抗力控制（开发中）。

- **连杆长度**: L1 = 0.4m，L2 = 0.3m
- **控制律**: F = Kp·(xd - x) - Kd·v，τ = J^T·F
- **安全限幅**: ±3.0 N·m / 关节
- **状态**: 模块已实现，尚未接入主控制循环

### 6. PID 算法 (`alg_pid`)

| 类型 | 说明 | 特点 |
|---|---|---|
| `PID_t` | 标准位置式 PID | 积分限幅，通用 |
| `TDPID_t` | 跟踪微分器 PID | TD 平滑目标值，抗噪声微分 |
| `fastPID_t` | 增量式 PID | 预计算增益 A0/A1/A2，执行快 |
| `Impedance_t` | 阻抗控制器 | out = Kp·dp + Kd·dv + t |

### 7. 其他设备

| 模块 | 文件 | 说明 |
|---|---|---|
| OLED 显示 | `dvc_oled` | SSD1306 128×64, I2C2, 双缓冲, `OLED_printf()` |
| 舵机 | `dvc_servo` | TIM1 PWM 50Hz, 4 通道, ±135° |
| Vofa+ 调试 | `dvc_vofa` | JustFloat/FireWater 协议, 支持运行时 PID 调参 |
| Action 计程 | `dvc_action` | 28 字节帧, 输出 yaw/pos_x/pos_y |
| BMI088 IMU | `BMI088driver` | 6 轴, SPI1, 3G/2000dps, 800Hz |
| 蜂鸣器 | `bsp_buzzer` | TIM4 PWM, 支持旋律播放 |
| RGB LED | `bsp_RGB_LED` | TIM5 PWM + 64-LED 灯带 (TIM8) |
| 按键 | `bsp_key` | PA0, 20ms 去抖, TIM7 轮询 |

---

## 通信协议

### SBUS (遥控器)

```
波特率: 100000 | 数据位: 9 | 校验: Even | 停止位: 1
帧格式: [0x0F] [22字节数据] [0x00]
通道: 16 × 11bit, 范围 ~240-1807, 中值 ~1024
```

### DJI 电机 CAN 协议 (CAN1, 1Mbps)

```
反馈帧: StdId 0x201-0x208, 8字节
  [0-1] 编码器角度 (uint16, 0-8191)
  [2-3] 转速 (int16, rpm)
  [4-5] 实际电流 (int16)

控制帧: StdId 0x200 (电机1-4) / 0x1FF (电机5-8), 8字节
  4 × int16 电流值 (-16384 ~ +16384)
```

### 海泰电机 MIT 协议 (CAN2, 1Mbps)

```
控制帧: StdId 由电机ID决定, 8字节
  [0-1] 位置 (16bit) + [2-3] 速度 (12bit) + [3-4] Kp (12bit) + [5-6] Kd (12bit) + [6-7] 力矩 (12bit)

反馈帧: StdId 0x00, 8字节
  [0] 电机ID + [1-2] 位置 (16bit) + [3-4] 速度 (12bit) + [5-6] 力矩 (12bit)
```

### Vofa+ 调试协议 (USART1, 115200)

```
上位机 → 下位机 (PID 调参命令):
  格式: "=XXYYYY!" (例: "P1=1.23!", "I2=0.5!")
  XX: P1/I1/D1 (速度环) / P2/I2/D2 (位置环)
  YYYY: 浮点数值

下位机 → 上位机 (遥测数据):
  JustFloat: N × float32 (小端) + 尾帧 0x0000807F
  FireWater: CSV 文本 + \r\n
```

---

## 构建与烧录

### 前置依赖

- `gcc-arm-none-eabi` (ARM 交叉编译工具链)
- CMake ≥ 3.22
- Ninja 构建系统
- ST-Link 工具 (烧录)

### 编译

```bash
# 配置 (Debug 模式)
cmake --preset Debug

# 编译
cmake --build build/Debug
```

### 烧录

```bash
# 使用 ST-Link
st-flash --config stlink.cfg write build/Debug/C_Board.bin 0x08000000
```

### CubeMX 重新生成

工程由 STM32CubeMX 6.16.1 生成，固件包版本 FW_F4 V1.28.3。修改外设配置后：

```bash
# 重新生成外设初始化代码
stm32cubemx C_Board.ioc
```

> ⚠️ CubeMX 会覆盖 `Core/Src/` 和 `Core/Inc/` 下的外设文件，但不会修改 `User_File/`。

---

## 已知问题

| # | 严重程度 | 问题 | 位置 |
|---|---|---|---|
| 1 | 🔴 高 | CAN1 回调被重复注册 (Chassis_Init 和 Task_Init 各注册一次)，每帧 CAN 消息被处理两次 | `Task.c:51` + `Chassis.c` |
| 2 | 🔴 高 | CAN 反馈中 speed 和 current 用 `uint16_t` 接收，应为 `int16_t`，导致反转时速度反馈错误 | `dvc_dji_motor.c:49-50` |
| 3 | 🟡 中 | 速度内环 Ki=0 (纯 P 控制)，存在固有稳态误差，电机无法精准到达目标角度 | `dvc_dji_motor.c:86` |
| 4 | 🟡 中 | TDPID 跟踪微分器引入目标值滞后，角度环残余误差无法完全消除 | `alg_pid.c:96-97` |
| 5 | 🟡 中 | `Remote_Callback()` 在 SBUS 中断中执行全部电机 PID + CAN 发送，中断耗时过长 | `RemoteControl.c` |
| 6 | 🟡 中 | `DJ_MotorRun()` 在一次回调中被调用 3 次，产生冗余 CAN 帧 | `RemoteControl.c:28,36,48` |
| 7 | 🟡 中 | 遥控器摇杆无死区处理，回中漂移导致电机持续微调抖动 | `RemoteControl.c` |
| 8 | 🟢 低 | OLED 刷新在 TIM7 中断中执行 I2C 传输，可能阻塞其他中断 | `Task.c` |
| 9 | 🟢 低 | USART1 被 Vofa+ 和 Action 模块共用，运行时只能激活一个 | `dvc_vofa.h` / `dvc_action.h` |
| 10 | 🟢 低 | `Task_loop()` 中 `HAL_Delay(2)` 固定阻塞，降低主循环频率 | `Task.c:67` |
| 11 | 🟢 低 | 机械臂模块 (`Arm.c`) 已实现但未接入控制循环 | `Arm.c` |

---

## 快速上手

### 遥控器操作

1. **SWA 向上** — 进入使能模式
2. **右摇杆** — 控制底盘前后左右移动
3. **左摇杆 X 轴** — 控制底盘自转
4. **左摇杆 Y 轴** — 控制升降台
5. **VRA 旋钮** — 控制云台角度
6. **VRB 旋钮** — 控制夹爪旋转
7. **SWB 向下** — 气动夹爪夹紧
8. **SWC 向下** — 推杆伸出
9. **SWD 向下** — 舵机旋转 + 吸盘启动
10. **SWA 向下** — **紧急停机**

### PID 调参

1. 连接 Vofa+ 到 USART1 (115200 baud)
2. 选择 JustFloat 数据格式
3. 发送命令调整参数 (如 `P1=8.0!` 设置速度环 Kp)
4. 实时观察速度环/角度环的 目标值、反馈值、输出值

---

## 开发团队

武汉科技大学 WUST RoboCon 战队
