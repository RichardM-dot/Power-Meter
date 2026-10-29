/**
 * @file    tim.h
 * @brief   TIM1 初始化函数与句柄声明
 *
 * TIM1 用作采样定时器：
 *   72 MHz / (71+1) = 1 MHz 计数频率
 *   计数周期 1999+1 = 2000 → 更新中断频率 500 Hz（2 ms）
 */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.h
  * @brief   tim.c 的函数原型声明
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

#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* 包含公共 HAL 定义 */
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* TIM1 全局句柄，供 main 启动定时器使用 */
extern TIM_HandleTypeDef htim1;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/**
 * @brief  初始化 TIM1：内部时钟、500 Hz 更新中断
 */
void MX_TIM1_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */
