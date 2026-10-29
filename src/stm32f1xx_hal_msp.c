/**
 * @file    stm32f1xx_hal_msp.c
 * @brief   HAL 全局 MSP（MCU Support Package）初始化
 *
 * 这里只包含与具体外设无关的全局初始化；
 * 各外设（CAN/I2C/TIM/USART）的引脚与时钟配置分别在
 * can.c、i2c.c、tim.c、usart.c 的 MspInit 中完成。
 */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         stm32f1xx_hal_msp.c
  * @brief        MSP 初始化与反初始化代码
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

/* 包含文件 -------------------------------------------------------------------*/
#include "main.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  全局 MSP 初始化
  * @note   - 使能 AFIO（复用功能/重映射）与 PWR（电源控制）时钟
  *         - 关闭 JTAG，仅保留 SWD 调试接口（释放 PA15/PB3/PB4 引脚）
  * @retval None
  */
void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  /* 使能复用功能与电源控制时钟 */
  __HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  /* 系统中断初始化 */

  /** NOJTAG：关闭 JTAG-DP，保留 SW-DP（SWDIO/SWCLK）调试功能 */
  __HAL_AFIO_REMAP_SWJ_NOJTAG();

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
