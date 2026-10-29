/**
 * @file    i2c.c
 * @brief   I2C1 初始化（主机、7 位地址、400 kHz）
 *
 * 引脚：PB6 = SCL，PB7 = SDA，均为复用开漏输出，
 *       总线上需要有上拉电阻（INA226 模块一般已自带）。
 */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   I2C 外设的配置代码
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
#include "i2c.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* I2C1 全局句柄定义 */
I2C_HandleTypeDef hi2c1;

/**
 * @brief  I2C1 初始化
 * @note   - 时钟频率 400 kHz（快速模式）
 *         - 7 位从机地址模式，本机地址 0（只做主机）
 *         - 关闭双地址、广播呼叫和时钟延展
 * @retval None
 */
void MX_I2C1_Init(void)
{
  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */

  hi2c1.Instance = I2C1;                          /* 使用 I2C1 外设 */
  hi2c1.Init.ClockSpeed = 400000;                 /* SCL = 400 kHz */
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;         /* 快速模式占空比 2:1 */
  hi2c1.Init.OwnAddress1 = 0;                     /* 本机地址（主机模式不用） */
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT; /* 7 位地址模式 */
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;/* 关闭双地址模式 */
  hi2c1.Init.OwnAddress2 = 0;                     /* 第二地址不使用 */
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;/* 关闭广播呼叫 */
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;    /* 允许从机时钟延展 */
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */
}

/**
 * @brief  I2C1 底层引脚与时钟初始化（由 HAL_I2C_Init 内部调用）
 * @param  i2cHandle I2C 句柄
 * @retval None
 */
void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if (i2cHandle->Instance == I2C1)
  {
    /* USER CODE BEGIN I2C1_MspInit 0 */

    /* USER CODE END I2C1_MspInit 0 */

    /* 使能 GPIOB 时钟 */
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* I2C1 引脚配置：
     *   PB6 → I2C1_SCL
     *   PB7 → I2C1_SDA
     * 复用开漏模式，高速输出 */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* 使能 I2C1 外设时钟 */
    __HAL_RCC_I2C1_CLK_ENABLE();

    /* USER CODE BEGIN I2C1_MspInit 1 */

    /* USER CODE END I2C1_MspInit 1 */
  }
}

/**
 * @brief  I2C1 底层反初始化：关闭时钟并释放引脚
 * @param  i2cHandle I2C 句柄
 * @retval None
 */
void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{
  if (i2cHandle->Instance == I2C1)
  {
    /* USER CODE BEGIN I2C1_MspDeInit 0 */

    /* USER CODE END I2C1_MspDeInit 0 */

    /* 关闭 I2C1 时钟 */
    __HAL_RCC_I2C1_CLK_DISABLE();

    /* 将 PB6、PB7 恢复为默认状态 */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);

    /* USER CODE BEGIN I2C1_MspDeInit 1 */

    /* USER CODE END I2C1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
