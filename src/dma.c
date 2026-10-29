/**
 * @file    dma.c
 * @brief   DMA 控制器初始化
 *
 * 通道分配：
 *   DMA1_Channel7 → USART2_TX（由 HAL_UART_MspInit 完成通道参数配置）
 */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    dma.c
  * @brief   所有 DMA 传输的配置代码
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
#include "dma.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* 配置 DMA                                                                   */
/*----------------------------------------------------------------------------*/

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * @brief  使能 DMA 控制器时钟，并设置 DMA 通道中断优先级
  * @note   DMA1_Channel7 对应 USART2_TX。
  *         优先级设为 0（最高），保证串口 DMA 发送及时响应。
  * @retval None
  */
void MX_DMA_Init(void)
{
  /* 使能 DMA1 控制器时钟 */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* 配置 DMA1_Channel7 中断优先级并使能中断（USART2_TX 使用） */
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);
}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
