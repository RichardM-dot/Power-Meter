/**
 * @file    usart.c
 * @brief   USART2 初始化（115200 8N1，TX 使用 DMA 发送）
 *
 * 引脚与 DMA：
 *   PA2 = USART2_TX → DMA1_Channel7（内存 → 外设，字节对齐）
 *   PA3 = USART2_RX
 */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   USART 外设的配置代码
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
#include "usart.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* USART2 及其 TX DMA 句柄定义 */
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_tx;

/**
 * @brief  USART2 初始化
 * @note   波特率 115200，8 位数据，无校验，1 位停止位，
 *         同时收发模式，无硬件流控，16 倍过采样
 * @retval None
 */
void MX_USART2_UART_Init(void)
{
  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */

  huart2.Instance = USART2;                          /* 使用 USART2 外设 */
  huart2.Init.BaudRate = 115200;                     /* 波特率 115200 */
  huart2.Init.WordLength = UART_WORDLENGTH_8B;       /* 8 位数据 */
  huart2.Init.StopBits = UART_STOPBITS_1;            /* 1 位停止位 */
  huart2.Init.Parity = UART_PARITY_NONE;             /* 无校验 */
  huart2.Init.Mode = UART_MODE_TX_RX;                /* 收发模式 */
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;       /* 无硬件流控 */
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;   /* 16 倍过采样 */
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */
}

/**
 * @brief  USART2 底层引脚、DMA 与中断初始化（由 HAL_UART_Init 内部调用）
 * @param  uartHandle UART 句柄
 * @retval None
 */
void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if (uartHandle->Instance == USART2)
  {
    /* USER CODE BEGIN USART2_MspInit 0 */

    /* USER CODE END USART2_MspInit 0 */

    /* 使能 USART2 时钟 */
    __HAL_RCC_USART2_CLK_ENABLE();

    /* 使能 GPIOA 时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* USART2 引脚配置：
     *   PA2 → USART2_TX（复用推挽输出）
     *   PA3 → USART2_RX（输入，浮空）
     */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART2_TX DMA 配置：DMA1_Channel7，内存 → 外设 */
    hdma_usart2_tx.Instance = DMA1_Channel7;
    hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH; /* 内存搬到串口数据寄存器 */
    hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;     /* 外设地址不递增 */
    hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;         /* 内存地址递增 */
    hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; /* 外设按字节 */
    hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;    /* 内存按字节 */
    hdma_usart2_tx.Init.Mode = DMA_NORMAL;                /* 普通模式（非循环） */
    hdma_usart2_tx.Init.Priority = DMA_PRIORITY_LOW;      /* 低优先级 */
    if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK)
    {
      Error_Handler();
    }

    /* 将 DMA 句柄链接到 UART 句柄的 hdmatx 字段 */
    __HAL_LINKDMA(uartHandle, hdmatx, hdma_usart2_tx);

    /* USART2 全局中断优先级 1 */
    HAL_NVIC_SetPriority(USART2_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);

    /* USER CODE BEGIN USART2_MspInit 1 */

    /* USER CODE END USART2_MspInit 1 */
  }
}

/**
 * @brief  USART2 底层反初始化：关闭时钟、释放引脚与 DMA
 * @param  uartHandle UART 句柄
 * @retval None
 */
void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{
  if (uartHandle->Instance == USART2)
  {
    /* USER CODE BEGIN USART2_MspDeInit 0 */

    /* USER CODE END USART2_MspDeInit 0 */

    /* 关闭 USART2 时钟 */
    __HAL_RCC_USART2_CLK_DISABLE();

    /* 将 PA2、PA3 恢复为默认状态 */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

    /* 反初始化 USART2_TX DMA 通道 */
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* 关闭 USART2 中断 */
    HAL_NVIC_DisableIRQ(USART2_IRQn);

    /* USER CODE BEGIN USART2_MspDeInit 1 */

    /* USER CODE END USART2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
