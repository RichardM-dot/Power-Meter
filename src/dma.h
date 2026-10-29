/**
 * @file    dma.h
 * @brief   DMA 初始化函数声明
 *
 * 本工程只使用一个 DMA 通道：
 *   DMA1_Channel7 —— USART2_TX 的 DMA 发送请求（内存 → 外设）
 */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    dma.h
  * @brief   dma.c 的函数原型声明
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

#ifndef __DMA_H__
#define __DMA_H__

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
 * @brief  使能 DMA1 时钟并配置 DMA 通道中断优先级
 * @note   必须在 USART2（使用 DMA 发送）初始化之前调用
 */
void MX_DMA_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __DMA_H__ */
