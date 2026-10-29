/**
 * @file    stm32f1xx_it.h
 * @brief   中断服务函数声明
 *
 * 本工程用到的中断：
 *   - SysTick                  ：HAL 1 ms 时基
 *   - DMA1_Channel7            ：USART2_TX 的 DMA 传输完成/半完成
 *   - USB_LP_CAN1_RX0          ：CAN1 接收 FIFO0 消息挂起
 *   - TIM1_UP                  ：TIM1 更新事件（2 ms 采样节拍）
 *   - USART2                   ：USART2 全局中断
 */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.h
  * @brief   中断处理函数头文件
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

#ifndef __STM32F1xx_IT_H
#define __STM32F1xx_IT_H

#ifdef __cplusplus
extern "C" {
#endif

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Cortex-M3 内核异常处理函数 ------------------------------------------------*/
void NMI_Handler(void);          /* 不可屏蔽中断 */
void HardFault_Handler(void);    /* 硬件错误 */
void MemManage_Handler(void);    /* 存储器管理错误（Cortex-M3 上不使用） */
void BusFault_Handler(void);     /* 总线错误 */
void UsageFault_Handler(void);   /* 用法错误 */
void SVC_Handler(void);          /* 系统服务调用 */
void DebugMon_Handler(void);     /* 调试监视 */
void PendSV_Handler(void);       /* 可挂起系统服务 */
void SysTick_Handler(void);      /* 系统滴答定时器 */

/* STM32F1 外设中断处理函数 -----------------------------------------------*/
void DMA1_Channel7_IRQHandler(void);      /* DMA1 通道 7（USART2_TX） */
void USB_LP_CAN1_RX0_IRQHandler(void);    /* USB 低优先级 / CAN1 RX0 */
void TIM1_UP_IRQHandler(void);            /* TIM1 更新中断 */
void USART2_IRQHandler(void);             /* USART2 全局中断 */

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* __STM32F1xx_IT_H */
