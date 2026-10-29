/**
 * @file    i2c.h
 * @brief   I2C1 初始化函数与句柄声明
 *
 * I2C1 用于与 INA226 电流/电压/功率传感器通信：
 *   PB6 = I2C1_SCL
 *   PB7 = I2C1_SDA
 *   速率 400 kHz（快速模式）
 */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.h
  * @brief   i2c.c 的函数原型声明
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

#ifndef __I2C_H__
#define __I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

/* 包含公共 HAL 定义 */
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* I2C1 全局句柄，供 INA226 驱动使用 */
extern I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/**
 * @brief  初始化 I2C1（400 kHz、7 位地址、主机模式）
 */
void MX_I2C1_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __I2C_H__ */
