/**
 * @file    VOFA.h
 * @brief   VOFA+ 上位机波形协议（justfloat 格式）函数声明
 *
 * justfloat 帧格式（共 20 字节）：
 *   [0..15] 4 个 float，小端字节序（STM32 为小端，可直接内存拷贝）
 *   [16..19] 帧尾 0x00 0x00 0x80 0x7F（即 float 值 0x7F800000）
 *
 * VOFA+ 的 justfloat 控件会自动按帧尾切分数据并绘制波形。
 */

#ifndef __VOFA_H_
#define __VOFA_H_

/**
 * @brief  以 VOFA+ justfloat 格式发送 4 路 float 数据
 * @param  a 通道 1 数据（本工程为分流电压，V）
 * @param  b 通道 2 数据（本工程为总线电压，V）
 * @param  c 通道 3 数据（本工程为电流，A）
 * @param  d 通道 4 数据（本工程为功率，W）
 * @note   使用 USART2 + DMA 发送，共 20 字节。
 */
void VOFA_justfloat(float a, float b, float c, float d);

#endif /* __VOFA_H_ */
