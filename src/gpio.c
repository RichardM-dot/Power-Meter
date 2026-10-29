/**
 * @file    gpio.c
 * @brief   GPIO 端口时钟初始化
 */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   所有 GPIO 引脚的配置代码
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
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* 配置 GPIO                                                                  */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * @brief  配置所有用到的 GPIO 端口时钟
  * @note   - GPIOD：PD0/PD1 为 HSE 外部晶振输入/输出引脚（由 RCC 使用）
  *         - GPIOA：PA2/PA3（USART2）、PA11/PA12（CAN1）、
  *                  PA13/PA14（SWD 调试口）
  *         - GPIOB：PB6/PB7（I2C1）
  *         各引脚的工作模式在对应外设的 MspInit 中设置。
  * @retval None
  */
void MX_GPIO_Init(void)
{
  /* 使能各 GPIO 端口时钟 */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
