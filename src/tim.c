/**
 * @file    tim.c
 * @brief   TIM1 基础定时器初始化
 *
 * 定时周期计算：
 *   TIM1 时钟 = APB2 = 72 MHz（APB2 不分频）
 *   预分频 = 71 → 计数时钟 = 72 MHz / 72 = 1 MHz
 *   自动重装 = 1999 → 更新周期 = 2000 / 1 MHz = 2 ms（500 Hz）
 */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.c
  * @brief   TIM 外设的配置代码
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
#include "tim.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* TIM1 全局句柄定义 */
TIM_HandleTypeDef htim1;

/**
 * @brief  TIM1 初始化（仅使用更新中断，不输出 PWM）
 * @note   预分频 71、周期 1999，得到 500 Hz 更新中断
 * @retval None
 */
void MX_TIM1_Init(void)
{
  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */

  htim1.Instance = TIM1;                              /* 使用 TIM1（高级定时器） */
  htim1.Init.Prescaler = 71;                          /* 72 MHz / 72 = 1 MHz */
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;        /* 向上计数 */
  htim1.Init.Period = 1999;                           /* 2000 个计数 → 2 ms */
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;  /* 死区/滤波时钟不分频 */
  htim1.Init.RepetitionCounter = 0;                   /* 重复计数 0（每次溢出都产生事件） */
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; /* 关闭预装载 */
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }

  /* 时钟源选择内部时钟（CK_INT） */
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /* 主从模式配置：本定时器独立运行，不输出触发信号 */
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
}

/**
 * @brief  TIM1 底层时钟与中断初始化（由 HAL_TIM_Base_Init 内部调用）
 * @param  tim_baseHandle 定时器句柄
 * @retval None
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{
  if (tim_baseHandle->Instance == TIM1)
  {
    /* USER CODE BEGIN TIM1_MspInit 0 */

    /* USER CODE END TIM1_MspInit 0 */

    /* 使能 TIM1 时钟 */
    __HAL_RCC_TIM1_CLK_ENABLE();

    /* TIM1 更新中断优先级设为 0（最高），保证 2 ms 采样周期稳定 */
    HAL_NVIC_SetPriority(TIM1_UP_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM1_UP_IRQn);

    /* USER CODE BEGIN TIM1_MspInit 1 */

    /* USER CODE END TIM1_MspInit 1 */
  }
}

/**
 * @brief  TIM1 底层反初始化：关闭时钟与中断
 * @param  tim_baseHandle 定时器句柄
 * @retval None
 */
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{
  if (tim_baseHandle->Instance == TIM1)
  {
    /* USER CODE BEGIN TIM1_MspDeInit 0 */

    /* USER CODE END TIM1_MspDeInit 0 */

    /* 关闭 TIM1 时钟 */
    __HAL_RCC_TIM1_CLK_DISABLE();

    /* 关闭 TIM1 更新中断 */
    HAL_NVIC_DisableIRQ(TIM1_UP_IRQn);

    /* USER CODE BEGIN TIM1_MspDeInit 1 */

    /* USER CODE END TIM1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
