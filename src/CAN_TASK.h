/**
 * @file    CAN_TASK.h
 * @brief   CAN 收发相关函数声明
 *
 * 数据协议（本工程自定义）：
 *   CAN 标准帧，标识符 0x605，DLC = 8
 *   数据区依次为 4 个 int16 大端数（高字节在前）：
 *     [0..1] 分流电压（mV）
 *     [2..3] 总线电压（mV）
 *     [4..5] 电流（mA）
 *     [6..7] 功率（0.01 W）
 */

#ifndef __CAN_TASK_H
#define __CAN_TASK_H

/* main.h 中已包含 stm32f1xx_hal.h，可直接使用 CAN_HandleTypeDef */
#include "main.h"

/**
 * @brief  通过 CAN 发送 4 个 int16 数据
 * @param  _hcan CAN 句柄（原工程实现内部固定使用全局 hcan）
 * @param  stdid 标准帧标识符（11 位）
 * @param  num1  第 1 个 16 位数据（大端发送）
 * @param  num2  第 2 个 16 位数据（大端发送）
 * @param  num3  第 3 个 16 位数据（大端发送）
 * @param  num4  第 4 个 16 位数据（大端发送）
 * @note   该函数只把数据写入发送邮箱，实际发送由 CAN 控制器完成。
 */
void CAN_send(CAN_HandleTypeDef *_hcan, int16_t stdid, int16_t num1,
              int16_t num2, int16_t num3, int16_t num4);

/**
 * @brief  发送 INA226 测量数据的预留接口
 * @note   原 MDK 工程中只有声明、没有实现，移植时保持原样。
 *         如果需要使用，请在本文件中补充实现，参数依次为：
 *         分流电压、总线电压、电流、功率（均为 float）。
 */
void CAN_Send_INA226(CAN_HandleTypeDef *hcan, uint16_t stdid, float shunt_v,
                     float bus_v, float current, float power);

/**
 * @brief  配置 CAN 验收滤波器并启动 CAN
 * @param  hcan CAN 句柄
 * @note   滤波器组 0，32 位掩码模式，掩码全 0（接收所有 ID），
 *         接收 FIFO0；同时使能 RX FIFO0 与发送邮箱空中断。
 */
void CAN_Filter_Init(CAN_HandleTypeDef *hcan);

#endif /* __CAN_TASK_H */
