/**
 * @file    can.h
 * @brief   CAN1 初始化函数与句柄声明
 *
 * CAN1 引脚：
 *   PA11 = CAN_RX（浮空输入）
 *   PA12 = CAN_TX（复用推挽输出）
 * 位速率：1 Mbps（APB1 = 36 MHz，预分频 2，BS1 = 16 TQ，BS2 = 1 TQ）
 */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.h
  * @brief   can.c 的函数原型声明
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

#ifndef __CAN_H__
#define __CAN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* 包含公共 HAL 定义 */
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* CAN1 全局句柄，供 CAN_TASK 与 main 使用 */
extern CAN_HandleTypeDef hcan;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/**
 * @brief  初始化 CAN1：正常模式、1 Mbps、自动总线关闭、自动重发
 */
void MX_CAN_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __CAN_H__ */
