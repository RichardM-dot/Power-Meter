/**
 * @file    gpio.h
 * @brief   GPIO 初始化函数声明
 *
 * 本工程没有额外的普通 GPIO 功能引脚，
 * 只有 HSE 晶振引脚 PD0/PD1 以及各外设复用引脚（在各自 MspInit 中配置）。
 */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   gpio.c 的函数原型声明
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

#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* 包含公共 HAL 定义 */
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/**
 * @brief  初始化所有用到的 GPIO 端口时钟
 * @note   各外设的具体引脚（CAN/I2C/USART）在对应外设的 MspInit 中配置
 */
void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /* __GPIO_H__ */
