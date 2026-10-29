/**
 * @file    INA226.h
 * @brief   INA226 电流/电压/功率监测芯片驱动（I2C 接口）
 *
 * 芯片简介：
 *   INA226 是 TI 的高侧/低侧电流与总线电压监测芯片，
 *   内部通过采样电阻（分流电阻）测量分流电压，再结合校准寄存器
 *   计算出电流与功率，测量结果通过 I2C 读取。
 *
 * 本工程接线：
 *   I2C1：PB6 = SCL，PB7 = SDA，400 kHz
 *   INA226 的 A0/A1 接地时 7 位地址为 0x40，
 *   HAL 使用 8 位地址形式，因此写为 0x80。
 */

#ifndef __INA226_H
#define __INA226_H

#include "stm32f1xx_hal.h"

/* ============================ 芯片参数配置 ============================ */

/* I2C 从机地址：7 位地址 0x40 左移一位得到 8 位地址 0x80
 * （A0、A1 均接 GND 时的默认地址） */
#define INA226_I2C_ADDR    0x80

/* 默认采样电阻值（单位：Ω）。
 * 实际板子可以通过 INA226_Init() 中的 UID 映射表覆盖该值。 */
#define INA226_R_SHUNT     0.001f

/* 设计的最大测量电流（单位：A）。
 * 用于计算 Current_LSB：Current_LSB = 最大电流 / 32767。 */
#define INA226_MAX_CURRENT 32.7f

/* ============================ 寄存器地址 ============================== */

#define INA226_REG_CONFIG  0x00        /* 配置寄存器 */
#define INA226_REG_SHUNT   0x01        /* 分流电压寄存器（有符号） */
#define INA226_REG_BUS     0x02        /* 总线电压寄存器（无符号） */
#define INA226_REG_POWER   0x03        /* 功率寄存器（无符号） */
#define INA226_REG_CURRENT 0x04        /* 电流寄存器（有符号） */
#define INA226_REG_CALIB   0x05        /* 校准寄存器 */

/* ============================ 计算宏 ================================== */

/* 电流 LSB（每个最低有效位代表的电流值，单位 A）。
 * 注意：INA226.c 中会根据实际采样电阻重新计算运行时值，
 *       该宏主要作为默认参考。 */
#define INA226_CURRENT_LSB (INA226_MAX_CURRENT / 32767.0f)

/* 校准值宏：CAL = 0.00512 / (Current_LSB × R_SHUNT)
 * 原工程保留该宏，但实际初始化时使用板级电阻表计算，
 * 因此这里仅作为参考定义。 */
#define INA226_CALIB_VAL   (uint16_t)(0.00512f / (INA226_CURRENT_LSB * INA226_R_SHUNT))

/* 配置寄存器值 0x0417 的字段含义：
 *   AVG[13:11]   = 000 → 平均 1 次
 *   VBUSCT[10:8] = 100 → 总线电压转换时间 1.1 ms
 *   VSHCT[7:5]   = 000 → 分流电压转换时间 140 µs
 *   MODE[2:0]    = 111 → 分流电压 + 总线电压，连续转换
 *   bit4 为保留位，原工程写入值为 1，移植时保留。 */
#define INA226_CONFIG_VAL  0x0417

/* ============================ 对外接口 ================================ */

/**
 * @brief  初始化 INA226
 * @param  hi2c I2C 句柄（本工程为 &hi2c1）
 * @retval HAL_OK 表示配置寄存器与校准寄存器写入成功
 * @note   初始化时会读取 STM32 的 96 位唯一 ID 的低 32 位，
 *         在板级电阻表中查找对应的采样电阻值；
 *         若没有匹配项，则使用 INA226_R_SHUNT（默认 1 mΩ）。
 */
HAL_StatusTypeDef INA226_Init(I2C_HandleTypeDef *hi2c);

/**
 * @brief  读取分流电压（采样电阻两端压降）
 * @param  hi2c I2C 句柄
 * @retval 分流电压，单位 V；读取失败返回 0.0f
 */
float INA226_Read_ShuntVoltage(I2C_HandleTypeDef *hi2c);

/**
 * @brief  读取总线电压
 * @param  hi2c I2C 句柄
 * @retval 总线电压，单位 V（量程 0~36 V）；读取失败返回 0.0f
 */
float INA226_Read_BusVoltage(I2C_HandleTypeDef *hi2c);

/**
 * @brief  读取电流
 * @param  hi2c I2C 句柄
 * @retval 电流，单位 A（有符号）；读取失败返回 0.0f
 */
float INA226_Read_Current(I2C_HandleTypeDef *hi2c);

/**
 * @brief  读取功率
 * @param  hi2c I2C 句柄
 * @retval 功率，单位 W；读取失败返回 0.0f
 */
float INA226_Read_Power(I2C_HandleTypeDef *hi2c);

#endif /* __INA226_H */
