/**
 * @file    can.c
 * @brief   CAN1 初始化与引脚配置
 *
 * 位定时计算（APB1 = 36 MHz）：
 *   位速率 = 36 MHz / Prescaler / (1 + BS1 + BS2)
 *          = 36 MHz / 2 / (1 + 16 + 1)
 *          = 1 Mbps
 *   采样点位于 17/18 ≈ 94.4% 处。
 */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   CAN 外设的配置代码
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
#include "can.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* CAN1 全局句柄定义 */
CAN_HandleTypeDef hcan;

/**
 * @brief  CAN1 初始化
 * @note   - 正常通信模式（非回环/静默）
 *   - 预分频 2，BS1 = 16 TQ，BS2 = 1 TQ，SJW = 1 TQ → 1 Mbps
 *   - 自动总线关闭（AutoBusOff）和自动重发（AutoRetransmission）使能
 *   - 接收 FIFO 不锁定，发送优先级由请求顺序决定
 * @retval None
 */
void MX_CAN_Init(void)
{
  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */

  hcan.Instance = CAN1;                              /* 使用 CAN1 外设 */
  hcan.Init.Prescaler = 2;                           /* 分频后时钟 36/2 = 18 MHz */
  hcan.Init.Mode = CAN_MODE_NORMAL;                  /* 正常工作模式 */
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;             /* 重同步跳转宽度 1 TQ */
  hcan.Init.TimeSeg1 = CAN_BS1_16TQ;                 /* 段 1 = 16 TQ */
  hcan.Init.TimeSeg2 = CAN_BS2_1TQ;                  /* 段 2 = 1 TQ */
  hcan.Init.TimeTriggeredMode = DISABLE;             /* 关闭时间触发模式 */
  hcan.Init.AutoBusOff = ENABLE;                     /* 硬件自动恢复总线关闭 */
  hcan.Init.AutoWakeUp = DISABLE;                    /* 关闭自动唤醒（非睡眠模式） */
  hcan.Init.AutoRetransmission = ENABLE;             /* 发送失败自动重发 */
  hcan.Init.ReceiveFifoLocked = DISABLE;             /* FIFO 满时新报文覆盖旧报文 */
  hcan.Init.TransmitFifoPriority = DISABLE;          /* TXFP=0：按报文 ID 优先级发送（ID 越小越优先） */
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN CAN_Init 2 */

  /* USER CODE END CAN_Init 2 */
}

/**
 * @brief  CAN1 底层引脚、时钟与中断初始化（由 HAL_CAN_Init 内部调用）
 * @param  canHandle CAN 句柄
 * @retval None
 */
void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if (canHandle->Instance == CAN1)
  {
    /* USER CODE BEGIN CAN1_MspInit 0 */

    /* USER CODE END CAN1_MspInit 0 */

    /* 使能 CAN1 外设时钟 */
    __HAL_RCC_CAN1_CLK_ENABLE();

    /* 使能 GPIOA 时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* CAN 引脚配置：
     *   PA11 → CAN_RX（输入，浮空）
     *   PA12 → CAN_TX（复用推挽输出，高速）
     */
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* CAN1 RX0 中断（与 USB 低优先级中断共用向量）：
     * 优先级 2（数值越大优先级越低） */
    HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);

    /* USER CODE BEGIN CAN1_MspInit 1 */

    /* USER CODE END CAN1_MspInit 1 */
  }
}

/**
 * @brief  CAN1 底层反初始化：关闭时钟、释放引脚与中断
 * @param  canHandle CAN 句柄
 * @retval None
 */
void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{
  if (canHandle->Instance == CAN1)
  {
    /* USER CODE BEGIN CAN1_MspDeInit 0 */

    /* USER CODE END CAN1_MspDeInit 0 */

    /* 关闭 CAN1 时钟 */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /* 将 PA11、PA12 恢复为默认状态 */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

    /* 关闭 CAN1 RX0 中断 */
    HAL_NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);

    /* USER CODE BEGIN CAN1_MspDeInit 1 */

    /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
