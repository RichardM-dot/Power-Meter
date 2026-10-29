/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : 主程序入口
  *
  * 程序功能：
  *   1. 初始化系统时钟（HSE 8 MHz × 9 = 72 MHz）；
  *   2. 初始化 GPIO、DMA、I2C1、CAN1、TIM1、USART2；
  *   3. 初始化 INA226 电流/电压/功率传感器；
  *   4. 启动 TIM1 更新中断，每 2 ms 触发一次；
  *   5. 在定时器中断回调中读取 INA226 数据，并通过：
  *        - CAN 标准帧 ID 0x605（8 字节，4 个 int16 大端数据）发送；
  *        - USART2 + DMA，以 VOFA+ justfloat 格式发送 4 个 float。
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* 私有包含文件 ---------------------------------------------------------------*/
#include "main.h"
#include "can.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
/* 用户模块：
 *   INA226   —— 电流/电压/功率采集芯片驱动
 *   CAN_TASK —— CAN 发送与过滤器初始化
 *   VOFA     —— VOFA+ 上位机 justfloat 波形协议
 */
#include "INA226.h"
#include "CAN_TASK.h"
#include "VOFA.h"
/* USER CODE END Includes */

/* 私有类型定义 ---------------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* INA226 测量结果，单位分别为：V、V、A、W */
float shunt_volt, bus_volt, current, power;

/* 为便于 CAN 传输而缩放后的整型值：
 *   send_shunt : 分流电压，单位 1 mV
 *   send_bus   : 总线电压，单位 1 mV
 *   send_curr  : 电流，单位 1 mA
 *   send_power : 功率，单位 0.01 W
 */
int16_t send_shunt, send_bus, send_curr, send_power;
/* USER CODE END PTD */

/* 私有宏定义 ---------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* 私有宏与用户回调 -----------------------------------------------------------*/
/* USER CODE BEGIN PM */

/**
 * @brief  TIM1 更新（溢出）中断回调函数
 * @param  htim 触发回调的定时器句柄
 * @note   TIM1 配置为 72 MHz / (71+1) / (1999+1) = 500 Hz，即每 2 ms 调用一次。
 *         该函数由 HAL_TIM_IRQHandler() 在 TIM1_UP_IRQHandler 中调用。
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* 只处理 TIM1 的更新事件，其它定时器事件不响应 */
  if (htim == &htim1)
  {
    /* ① 读取 INA226 四个测量值（分流电压、总线电压、电流、功率） */
    shunt_volt = INA226_Read_ShuntVoltage(&hi2c1);
    bus_volt   = INA226_Read_BusVoltage(&hi2c1);
    current    = INA226_Read_Current(&hi2c1);
    power      = INA226_Read_Power(&hi2c1);

    /* ② 转换为整型，减少 CAN 每帧的字节数
     *   电压/电流放大 1000 倍（mV / mA），功率放大 100 倍（0.01 W）
     *   注意：总线电压超过 32.767 V、功率超过 327.67 W 时 int16_t 会溢出，
     *         此为原工程既有行为，移植时保持原样（VOFA 通道发送原始 float，不受影响） */
    send_shunt = (int16_t)(shunt_volt * 1000.0f);
    send_bus   = (int16_t)(bus_volt   * 1000.0f);
    send_curr  = (int16_t)(current    * 1000.0f);
    send_power = (int16_t)(power      * 100.0f);

    /* ③ 通过 CAN1 发送：标准帧 ID = 0x605，数据为 4 个 int16（大端） */
    CAN_send(&hcan, 0x605, send_shunt, send_bus, send_curr, send_power);

    /* ④ 同时通过 USART2 + DMA 发送 VOFA+ justfloat 波形数据 */
    VOFA_justfloat(shunt_volt, bus_volt, current, power);
  }
}

/* USER CODE END PM */

/* 私有变量 ---------------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* 私有函数原型 ---------------------------------------------------------------*/

/**
 * @brief  系统时钟配置：HSE 8 MHz 经 PLL 9 倍频得到 72 MHz
 */
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* 用户私有代码 ---------------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  程序入口
 * @retval int（实际不会返回）
 */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU 配置 ----------------------------------------------------------------*/

  /* 复位所有外设，初始化 Flash 接口和 SysTick 时基 */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* 配置系统时钟：72 MHz */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* 初始化所有已配置的外设
   * 注意顺序：DMA 必须在 I2C/CAN/USART 之前初始化，
   *          因为 USART2 的 MspInit 中会配置并链接 DMA 通道。 */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_CAN_Init();
  MX_TIM1_Init();
  MX_USART2_UART_Init();

  /* USER CODE BEGIN 2 */
  /* 初始化 INA226（写配置寄存器与校准寄存器）。
   * 原工程未检查返回值，此处保持相同行为。 */
  INA226_Init(&hi2c1);

  /* 启动 TIM1 更新中断：此后每 2 ms 进入一次 HAL_TIM_PeriodElapsedCallback */
  HAL_TIM_Base_Start_IT(&htim1);

  /* 配置 CAN 验收滤波器并启动 CAN（同时打开 RX FIFO0 与发送邮箱空中断） */
  CAN_Filter_Init(&hcan);

  /* USER CODE END 2 */

  /* 主循环：所有工作都在定时器中断中完成，主循环保持空闲 */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
 * @brief  系统时钟配置
 * @note   HSE = 8 MHz 外部晶振，PLL × 9 → SYSCLK = 72 MHz
 *         AHB  = 72 MHz（不分频）
 *         APB1 = 36 MHz（二分频，CAN 与 USART2 挂载在 APB1）
 *         APB2 = 72 MHz（不分频）
 *         Flash 等待周期 = 2（72 MHz 下的必要设置）
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /* 配置振荡器：打开 HSE，PLL 时钟源选择 HSE，9 倍频 */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;              /* 开启外部 8 MHz 晶振 */
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;/* HSE 不分频后输入 PLL */
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;              /* 内部 RC 也保持开启 */
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;          /* 开启 PLL */
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;  /* PLL 输入来自 HSE */
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;          /* 8 MHz × 9 = 72 MHz */
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /* 配置总线时钟：SYSCLK 选择 PLL，AHB/APB1/APB2 分频系数 */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; /* 系统时钟来自 PLL */
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;        /* HCLK = 72 MHz */
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;         /* PCLK1 = 36 MHz */
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;         /* PCLK2 = 72 MHz */

  /* 72 MHz 主频下 Flash 需要 2 个等待周期 */
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  错误处理函数
 * @note   发生 HAL 初始化错误时进入这里：关闭全局中断并死循环，
 *         便于用调试器查看调用栈或点灯定位。
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* 用户可以在下面添加自己的错误上报代码 */
  __disable_irq();  /* 关闭所有中断 */
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  断言失败时的回调（仅在定义 USE_FULL_ASSERT 时生效）
 * @param  file 源文件名
 * @param  line 出错行号
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* 用户可以在下面添加断言失败的处理，例如打印文件名与行号：
     printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
