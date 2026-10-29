/**
 * @file    VOFA.c
 * @brief   VOFA+ justfloat 波形数据发送
 *
 * 实现方式：
 *   用一个 union 让 4 个 float（16 字节）与 20 字节发送缓冲区共享内存，
 *   这样不需要做 float → 字节的强制转换，直接按小端内存布局发送。
 */

#include "VOFA.h"
#include "main.h"
#include "usart.h"

/**
 * @brief 发送缓冲区
 *
 * union 的大小由最大成员决定，为 20 字节：
 *   - data1[4]：4 个 float，占 16 字节
 *   - data2[20]：完整的一帧数据，占 20 字节
 *
 * 前 16 字节由 4 个 float 填充，后 4 字节写入 justfloat 帧尾。
 * 注意：union 的标签沿用了原工程的 "main" 名字，移植时保持不变；
 *       它只是类型标签，不会与 main() 函数冲突。
 */
union main
{
    float   data1[4];   /* 4 路 float 数据 */
    uint8_t data2[20];  /* 20 字节原始发送缓冲区 */
} data;

/**
 * @brief  以 VOFA+ justfloat 格式发送 4 路 float 数据
 * @param  a 通道 1 数据
 * @param  b 通道 2 数据
 * @param  c 通道 3 数据
 * @param  d 通道 4 数据
 *
 * 帧结构：
 *   data2[0..3]   = float a（小端）
 *   data2[4..7]   = float b（小端）
 *   data2[8..11]  = float c（小端）
 *   data2[12..15] = float d（小端）
 *   data2[16..19] = 0x00 0x00 0x80 0x7F（帧尾）
 */
void VOFA_justfloat(float a, float b, float c, float d)
{
    uint8_t i = 0;

    /* 依次把 4 个 float 写入 union 的 float 数组 */
    data.data1[i++] = a;
    data.data1[i++] = b;
    data.data1[i++] = c;
    data.data1[i++] = d;

    /* 写入 justfloat 帧尾：小端表示的 0x7F800000 */
    data.data2[16] = 0x00;
    data.data2[17] = 0x00;
    data.data2[18] = 0x80;
    data.data2[19] = 0x7f;

    /* 通过 USART2 的 DMA 通道发送整帧 20 字节（非阻塞） */
    HAL_UART_Transmit_DMA(&huart2, data.data2, sizeof(data.data2));
}
