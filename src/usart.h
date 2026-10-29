/**
 * @file    usart.h
 * @brief   USART2 初始化函数与句柄声明
 *
 * USART2 用于向 VOFA+ 上位机发送波形数据：
 *   PA2 = USART2_TX → DMA1_Channel7
 *   PA3 = USART2_RX
 *   波特率 115200，8 数据位，无校验，1 停止位
 */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   usart.c 的函数原型声明
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

#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* 包含公共 HAL 定义 */
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USART2 全局句柄，供 VOFA 模块发送数据使用 */
extern UART_HandleTypeDef huart2;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/**
 * @brief  初始化 USART2：115200 8N1，TX 使用 DMA1_Channel7
 */
void MX_USART2_UART_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */
