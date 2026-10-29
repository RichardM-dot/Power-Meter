# Power 电源监测工程（STM32F103C8 + INA226）

基于 **STM32F103C8T6** 的电源参数采集与上报工程：

- 通过 **I2C1** 读取 **INA226** 的分流电压、总线电压、电流、功率；
- 通过 **TIM1** 每 **2 ms（500 Hz）** 触发一次采样；
- 通过 **CAN1（1 Mbps）** 以标准帧 `0x605` 发送 4 个 `int16` 数据；
- 同时通过 **USART2 + DMA** 以 **VOFA+ justfloat** 格式上传 4 路波形。

构建环境：**PlatformIO + STM32Cube HAL**（`framework = stm32cube`）。

---

## 一、5 分钟速读版

如果只想快速弄懂“程序在干什么”，按下面 4 步读即可：

1. [`src/main.c`](src/main.c)：看 `main()` 的初始化顺序，以及 `HAL_TIM_PeriodElapsedCallback()` 里的采样/上报逻辑；
2. [`src/INA226.c`](src/INA226.c)：看 `INA226_Init()`、四个 `INA226_Read_xxx()` 如何把寄存器原始值换算成物理量；
3. [`src/CAN_TASK.c`](src/CAN_TASK.c)：看 `CAN_send()` 如何把 4 个 `int16` 打包成 8 字节 CAN 帧；
4. [`src/VOFA.c`](src/VOFA.c)：看 20 字节 justfloat 帧如何通过 DMA 发出。

理解这 4 个文件，就已经掌握了工程 80% 的行为。

---

## 二、完整推荐阅读顺序

> 原则：**先看总体流程 → 再看外设如何初始化 → 最后看中断回调如何串起业务**。
> 不要一上来逐个读 HAL 外设文件，容易迷失在寄存器配置里。

### 第 1 步：构建配置

| 顺序 | 文件 | 重点看什么 |
|---|---|---|
| 1.1 | [`platformio.ini`](platformio.ini) | 板卡 `genericSTM32F103C8`、框架 `stm32cube`。确认工程不自己维护 HAL/CMSIS。 |
| 1.2 | 本 README 的“工程结构与文件索引” | 先知道每个文件属于哪一层，后面读起来不会乱。 |

**要点：** `Drivers/`、`system_stm32f1xx.c`、启动汇编、`stm32f1xx_hal_conf.h` 都不在 `src/` 里，而是由 PlatformIO 的 `framework-stm32cubef1` 提供。因此本工程 `src/` 只放“应用层 + 外设初始化 + 用户模块”。

---

### 第 2 步：总体流程（最重要）

| 顺序 | 文件 | 重点看什么 |
|---|---|---|
| 2.1 | [`src/main.h`](src/main.h) | 文件头的引脚分配表；`Error_Handler()` 声明。 |
| 2.2 | [`src/main.c`](src/main.c) | `main()` 的初始化顺序；`SystemClock_Config()`；`HAL_TIM_PeriodElapsedCallback()`。 |

阅读 `main()` 时请特别留意初始化顺序：

```text
HAL_Init()
SystemClock_Config()           // HSE 8 MHz × 9 = 72 MHz
MX_GPIO_Init()
MX_DMA_Init()                  // 必须先于 USART2
MX_I2C1_Init()                 // PB6/PB7，400 kHz
MX_CAN_Init()                  // PA11/PA12，1 Mbps
MX_TIM1_Init()                 // 500 Hz 更新中断
MX_USART2_UART_Init()          // PA2/PA3，115200 + DMA1_Channel7
INA226_Init(&hi2c1)            // 写配置/校准寄存器
HAL_TIM_Base_Start_IT(&htim1)  // 启动 2 ms 采样节拍
CAN_Filter_Init(&hcan)         // 配置滤波器并启动 CAN
while (1) { }                  // 业务全部在中断里完成
```

**先记住这句话：主循环是空的，所有业务都在 TIM1 中断回调里。**

---

### 第 3 步：中断入口与 HAL 回调

| 顺序 | 文件 | 重点看什么 |
|---|---|---|
| 3.1 | [`src/stm32f1xx_it.c`](src/stm32f1xx_it.c) | `TIM1_UP_IRQHandler()`、`USB_LP_CAN1_RX0_IRQHandler()`、`DMA1_Channel7_IRQHandler()`、`USART2_IRQHandler()` 如何转交给 HAL。 |
| 3.2 | [`src/stm32f1xx_hal_msp.c`](src/stm32f1xx_hal_msp.c) | `HAL_MspInit()`：使能 AFIO/PWR 时钟，关闭 JTAG、保留 SWD。 |

这里只需要理解“中断向量 → HAL IRQHandler → HAL 回调”这条链路，不需要背每个中断函数。

---

### 第 4 步：外设初始化（按 `main()` 调用顺序读）

| 顺序 | 文件 | 重点看什么 |
|---|---|---|
| 4.1 | [`src/gpio.c/.h`](src/gpio.c) | 只使能 GPIOD/GPIOA/GPIOB 时钟，没有额外普通 IO。 |
| 4.2 | [`src/dma.c/.h`](src/dma.c) | DMA1 时钟；`DMA1_Channel7` 中断优先级。具体通道参数在 `usart.c` 的 `HAL_UART_MspInit()` 里。 |
| 4.3 | [`src/i2c.c/.h`](src/i2c.c) | I2C1 400 kHz、7 位地址；PB6/PB7 复用开漏；`hi2c1` 句柄。 |
| 4.4 | [`src/can.c/.h`](src/can.c) | CAN 位定时计算（1 Mbps）；PA11/PA12；`hcan` 句柄；RX0 中断优先级。 |
| 4.5 | [`src/tim.c/.h`](src/tim.c) | TIM1 预分频 71、周期 1999 → 500 Hz；`htim1` 句柄。 |
| 4.6 | [`src/usart.c/.h`](src/usart.c) | USART2 115200 8N1；PA2/PA3；DMA1_Channel7 的完整配置；`huart2` 句柄。 |

每个外设文件的结构都一样：

```text
MX_xxx_Init()          // 配置外设参数并调用 HAL_xxx_Init()
HAL_xxx_MspInit()      // 配置引脚、时钟、DMA、NVIC（由 HAL 自动调用）
HAL_xxx_MspDeInit()    // 反初始化（本工程运行时不调用）
```

---

### 第 5 步：传感器驱动 INA226

| 顺序 | 文件 | 重点看什么 |
|---|---|---|
| 5.1 | [`src/INA226.h`](src/INA226.h) | 寄存器地址、默认采样电阻、最大电流、配置值 `0x0417` 的字段含义。 |
| 5.2 | [`src/INA226.c`](src/INA226.c) | 板级 UID→采样电阻映射表；`Current_LSB` 与校准值计算；四个读取函数的换算系数。 |

关键换算关系：

```text
Current_LSB = INA226_MAX_CURRENT / 32767
CAL         = 0.00512 / (Current_LSB × R_SHUNT)
分流电压    = 原始值 × 2.5 µV
总线电压    = 原始值 × 1.25 mV
电流        = 原始值 × Current_LSB
功率        = 原始值 × 25 × Current_LSB
```

---

### 第 6 步：CAN 发送

| 顺序 | 文件 | 重点看什么 |
|---|---|---|
| 6.1 | [`src/CAN_TASK.h`](src/CAN_TASK.h) | 自定义 CAN 数据协议；三个函数声明。 |
| 6.2 | [`src/CAN_TASK.c`](src/CAN_TASK.c) | `CAN_send()` 的 8 字节打包；`CAN_Filter_Init()` 的全通过滤波；`HAL_CAN_ErrorCallback()` 的 Bus-Off 恢复。 |

数据格式：

```text
标准帧 ID = 0x605，DLC = 8
Byte 0..1：分流电压（mV，int16 大端）
Byte 2..3：总线电压（mV，int16 大端）
Byte 4..5：电流（mA，int16 大端）
Byte 6..7：功率（0.01 W，int16 大端）
```

---

### 第 7 步：VOFA+ 波形上传

| 顺序 | 文件 | 重点看什么 |
|---|---|---|
| 7.1 | [`src/VOFA.h`](src/VOFA.h) | justfloat 帧格式说明。 |
| 7.2 | [`src/VOFA.c`](src/VOFA.c) | `union` 共享内存；4 个 float + 4 字节帧尾；`HAL_UART_Transmit_DMA()`。 |

帧格式（20 字节）：

```text
[0..15]  4 个 float，小端字节序
[16..19] 0x00 0x00 0x80 0x7F（justfloat 帧尾）
```

---

### 第 8 步：回到 main.c 串起完整调用链

读完上面各模块后，再回看 `main.c` 的 `HAL_TIM_PeriodElapsedCallback()`，应该能顺畅理解整条链路：

```text
TIM1 更新中断（每 2 ms）
└─ TIM1_UP_IRQHandler()                     stm32f1xx_it.c
   └─ HAL_TIM_IRQHandler(&htim1)             HAL 库
      └─ HAL_TIM_PeriodElapsedCallback()     main.c
         ├─ INA226_Read_ShuntVoltage(&hi2c1) I2C1 阻塞读
         ├─ INA226_Read_BusVoltage(&hi2c1)
         ├─ INA226_Read_Current(&hi2c1)
         ├─ INA226_Read_Power(&hi2c1)
         ├─ CAN_send(&hcan, 0x605, ...)      CAN 发送邮箱
         └─ VOFA_justfloat(...)              USART2 + DMA1_Channel7
```

---

## 三、硬件与关键参数速查

| 项目 | 配置 |
|---|---|
| MCU | STM32F103C8T6，64 KB Flash / 20 KB RAM |
| 系统时钟 | HSE 8 MHz × 9 = 72 MHz |
| AHB / APB1 / APB2 | 72 MHz / 36 MHz / 72 MHz |
| CAN1 | PA11 = RX，PA12 = TX，**1 Mbps** |
| I2C1 | PB6 = SCL，PB7 = SDA，**400 kHz**，挂 INA226 |
| USART2 | PA2 = TX，PA3 = RX，**115200 8N1**，TX 用 **DMA1_Channel7** |
| TIM1 | 72 MHz / 72 / 2000 = **500 Hz（2 ms）** |
| 调试口 | PA13 = SWDIO，PA14 = SWCLK（关闭 JTAG，保留 SWD） |
| 中断优先级 | TIM1 更新 = 0，DMA1_CH7 = 0，USART2 = 1，CAN1 RX0 = 2 |

---

## 四、常用参数修改位置

| 想改什么 | 改哪里 |
|---|---|
| 采样周期（当前 2 ms） | `src/tim.c` 中 `htim1.Init.Prescaler` / `Period` |
| CAN 波特率（当前 1 Mbps） | `src/can.c` 中 `Prescaler`、`TimeSeg1`、`TimeSeg2` |
| CAN 发送 ID / 数据内容 | `src/main.c` 的 `CAN_send(&hcan, 0x605, ...)` |
| CAN 帧打包顺序 | `src/CAN_TASK.c` 的 `CAN_send()` |
| 采样电阻 / 板级配置 | `src/INA226.c` 的 `board_conf[]`，默认值在 `src/INA226.h` |
| INA226 量程 / 电流分辨率 | `src/INA226.h` 的 `INA226_MAX_CURRENT` |
| INA226 转换时间 / 平均次数 | `src/INA226.h` 的 `INA226_CONFIG_VAL` |
| 串口波特率 | `src/usart.c` 的 `huart2.Init.BaudRate` |
| 上送波形通道/格式 | `src/VOFA.c` 的 `VOFA_justfloat()` |
| 系统主频 | `src/main.c` 的 `SystemClock_Config()` |

---

## 五、工程结构与文件索引

```text
power/
├── platformio.ini            # PlatformIO 构建配置（未改动）
├── README.md                 # 本文件
└── src/
    ├── main.c                # 程序入口、时钟配置、TIM1 采样回调
    ├── main.h                # 公共头文件、引脚说明
    ├── gpio.c / gpio.h       # GPIO 端口时钟
    ├── dma.c / dma.h         # DMA1 时钟与通道中断
    ├── i2c.c / i2c.h         # I2C1 初始化（INA226）
    ├── can.c / can.h         # CAN1 初始化（1 Mbps）
    ├── tim.c / tim.h         # TIM1 初始化（500 Hz）
    ├── usart.c / usart.h     # USART2 + DMA 初始化（VOFA+）
    ├── stm32f1xx_it.c/.h     # 中断向量与 HAL 转发
    ├── stm32f1xx_hal_msp.c   # 全局 MSP 初始化（AFIO/PWR/SWD）
    ├── INA226.c / INA226.h   # INA226 驱动与校准
    ├── CAN_TASK.c/.h         # CAN 发送、滤波、Bus-Off 恢复
    └── VOFA.c / VOFA.h       # VOFA+ justfloat 协议
```

分层关系：

```text
应用层        main.c  ──────────────┐
                                    │
用户模块      INA226 / CAN_TASK / VOFA
                                    │
外设初始化    gpio / dma / i2c / can / tim / usart
                                    │
中断与 MSP    stm32f1xx_it / stm32f1xx_hal_msp
                                    │
HAL / CMSIS   PlatformIO framework-stm32cubef1（不在 src 中）
```

---

## 六、移植说明与注意事项

### 1. 为什么 `src/` 里没有 `Drivers/`、`system_stm32f1xx.c` 和启动文件？

因为当前工程使用 `framework = stm32cube`，这些文件由 PlatformIO 的 `framework-stm32cubef1` 提供。再复制进 `src/` 会造成重复定义或重复编译。

### 2. 为什么头文件名有大小写要求？

原工程在 Windows/MDK 下使用了 `CAN_task.h`、`ina226.h` 等与磁盘文件名大小写不一致的写法。Linux 区分大小写，因此移植时已统一为实际文件名 `CAN_TASK.h`、`INA226.h`。

### 3. 原工程保留的行为（未改动）

- `CAN_send()` 虽然接收 `_hcan` 参数，但函数内部固定使用全局 `hcan`；现有调用传入的也是 `&hcan`，行为一致。
- `CAN_Send_INA226()` 只有声明，没有实现；当前没有任何地方调用它。
- CAN 接收中断已使能，但没有实现 `HAL_CAN_RxFifo0MsgPendingCallback()`，因此收到的报文不会被业务处理。
- 总线电压按 `×1000`、功率按 `×100` 转成 `int16_t` 通过 CAN 发送，**总线电压超过 32.767 V、功率超过 327.67 W 会溢出**（VOFA 通道发送的是原始 float，不受影响）。
- INA226 的 4 次 I2C 读取在 TIM1 中断里同步阻塞完成；若 I2C 总线异常，可能拉长中断时间。
- VOFA 每 2 ms 发送 20 字节，约 10 kB/s；115200 波特率理论上限约 11.5 kB/s，负载已经较高，DMA 忙时 `HAL_UART_Transmit_DMA()` 会返回 `HAL_BUSY`，该帧被跳过。

### 4. 硬件接线注意

- I2C1 的 PB6/PB7 需要上拉电阻（常见 INA226 模块已自带）。
- INA226 的 A0/A1 接地时地址为 7 位 `0x40`，HAL 使用 8 位地址形式 `0x80`。
- CAN 总线两端需要 120 Ω 终端电阻。

---

## 七、编译与烧录

```bash
# 编译
pio run

# 通过 ST-Link 烧录
pio run -t upload

# 串口监视（115200）
pio device monitor -b 115200
```

> 本工程移植完成时未执行编译/烧录，以上命令供后续使用。
