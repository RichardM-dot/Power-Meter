/**
 * @file    main.h
 * @brief   主程序公共头文件（由 STM32CubeMX 工程整理移植而来）
 *
 * 硬件平台：STM32F103C8T6（64 KB Flash / 20 KB RAM）
 * 构建框架：PlatformIO + STM32Cube HAL（platformio.ini 中 framework = stm32cube）
 *
 * 本工程使用的外设与引脚分配：
 *   - CAN1   ：PA11 = CAN_RX，PA12 = CAN_TX，速率 1 Mbps
 *   - I2C1   ：PB6  = I2C1_SCL，PB7 = I2C1_SDA，速率 400 kHz，挂接 INA226
 *   - USART2 ：PA2  = USART2_TX，PA3 = USART2_RX，115200 8N1，
 *              发送端使用 DMA1_Channel7（输出 VOFA+ justfloat 数据）
 *   - TIM1   ：内部时钟源，500 Hz 更新中断（即 2 ms 采样一次 INA226）
 *   - SWD    ：PA13 = SWDIO，PA14 = SWCLK（只保留 SWD，关闭 JTAG）
 *
 * 移植说明：
 *   HAL 驱动、CMSIS 以及启动文件由 PlatformIO 的 framework-stm32cubef1 提供，
 *   因此 src 目录中不再放置 Drivers/、system_stm32f1xx.c 和启动汇编文件，
 *   否则会与框架中的同名实现产生重复定义（multiple definition）链接错误。
 */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Main program body 的公共声明
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

/* 防止头文件被重复包含 -------------------------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* 包含 HAL 头文件（会间接包含 stm32f1xx.h、hal_conf 等） --------------------*/
#include "stm32f1xx_hal.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* 对外函数声明 ---------------------------------------------------------------*/

/**
 * @brief  错误处理函数：关闭中断并原地死循环，便于调试器定位异常
 * @note   各外设初始化失败时会调用该函数
 */
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
