/**
 * @file    INA226.c
 * @brief   INA226 电流/电压/功率监测芯片驱动实现
 *
 * 数据换算关系：
 *   分流电压 = 原始值 × 2.5 µV
 *   总线电压 = 原始值 × 1.25 mV
 *   电流     = 原始值 × Current_LSB
 *   功率     = 原始值 × 25 × Current_LSB
 *
 * 其中 Current_LSB = INA226_MAX_CURRENT / 32767，
 * 校准寄存器 CAL = 0.00512 / (Current_LSB × R_SHUNT)。
 */

/* 包含 INA226 驱动头文件（注意：Linux 下文件名区分大小写） */
#include "INA226.h"

/* ======================== 板级采样电阻配置表 ========================== */

/**
 * @brief 板级配置项：MCU 唯一 ID → 采样电阻值
 *
 * 不同功率板可能使用不同阻值的采样电阻，
 * 通过 STM32 的 96 位唯一 ID 低 32 位区分板子，
 * 这样同一份固件可以在多块板子上自动适配。
 */
typedef struct {
    uint32_t uid;       /* STM32 唯一 ID 的低 32 位 */
    float    r_shunt;   /* 该板对应的采样电阻值（Ω） */
} BoardConfig_t;

/* 板级配置表：按实际板子的 UID 与采样电阻填写。
 * 如果当前芯片 UID 不在表中，则使用 INA226_R_SHUNT（默认 1 mΩ）。 */
static const BoardConfig_t board_conf[] = {
    {0x0664FF54, 0.0009897f},   /* 板子 1：约 0.99 mΩ */
    {0x87654321, 0.005f},       /* 板子 2：5 mΩ（示例） */
    {0x11223344, 0.010f},       /* 板子 3：10 mΩ（示例） */
};

/* 配置表项数 */
#define BOARD_CONF_COUNT (sizeof(board_conf) / sizeof(board_conf[0]))

/* 运行时计算出的电流 LSB（单位 A/LSB）。
 * 使用静态变量保存，供 INA226_Read_Current()/Power() 换算使用。 */
static float g_CurrentLSB = 0.0f;

/* 最近一次读取的原始寄存器值，保留为全局变量便于调试观察 */
int16_t raw_v1;   /* 分流电压原始值（有符号） */
int16_t raw_v;    /* 总线电压原始值 */
int16_t raw_i;    /* 电流原始值（有符号） */
int16_t raw_p;    /* 功率原始值 */

/* 当前 MCU 的唯一 ID 低 32 位，初始化时保存，便于调试查看 */
uint32_t mcu_id;

/**
 * @brief  INA226 初始化
 * @param  hi2c I2C 句柄
 * @retval HAL 状态：HAL_OK 表示成功
 *
 * 初始化步骤：
 *   1. 读取 MCU 唯一 ID，查表得到本板采样电阻；
 *   2. 计算 Current_LSB 与校准寄存器值；
 *   3. 写配置寄存器（设置转换时间、平均次数、连续测量模式）；
 *   4. 写校准寄存器（芯片据此计算电流与功率）。
 */
HAL_StatusTypeDef INA226_Init(I2C_HandleTypeDef *hi2c)
{
    uint8_t tx_data[2];
    HAL_StatusTypeDef status;
    float current_r_shunt = INA226_R_SHUNT; /* 默认使用头文件中的采样电阻 */
    uint16_t calib_val;
    uint8_t i;

    /* ① 读取 STM32 唯一 ID 的低 32 位 */
    mcu_id = HAL_GetUIDw0();

    /* ② 在板级配置表中查找匹配的采样电阻 */
    for (i = 0; i < BOARD_CONF_COUNT; i++) {
        if (board_conf[i].uid == mcu_id) {
            current_r_shunt = board_conf[i].r_shunt;
            break;
        }
    }

    /* ③ 计算电流 LSB：把最大电流均匀映射到 15 位有符号满量程 */
    g_CurrentLSB = INA226_MAX_CURRENT / 32767.0f;

    /* ④ 计算校准寄存器值：CAL = 0.00512 / (Current_LSB × R_SHUNT) */
    if (g_CurrentLSB > 0 && current_r_shunt > 0) {
        calib_val = (uint16_t)(0.00512f / (g_CurrentLSB * current_r_shunt));
    } else {
        calib_val = 0;  /* 参数异常时写入 0，避免出现非法值 */
    }

    /* ⑤ 写配置寄存器（高字节在前） */
    tx_data[0] = (INA226_CONFIG_VAL >> 8) & 0xFF;
    tx_data[1] = INA226_CONFIG_VAL & 0xFF;
    status = HAL_I2C_Mem_Write(hi2c, INA226_I2C_ADDR, INA226_REG_CONFIG,
                               1, tx_data, 2, 100);
    if (status != HAL_OK) return status;

    /* ⑥ 写校准寄存器（高字节在前） */
    tx_data[0] = (calib_val >> 8) & 0xFF;
    tx_data[1] = calib_val & 0xFF;
    status = HAL_I2C_Mem_Write(hi2c, INA226_I2C_ADDR, INA226_REG_CALIB,
                               1, tx_data, 2, 100);
    return status;
}

/**
 * @brief  读取分流电压（采样电阻两端压降）
 * @param  hi2c I2C 句柄
 * @retval 分流电压值，单位 V；I2C 读取失败时返回 0.0f
 * @note   分流电压 LSB 固定为 2.5 µV
 */
float INA226_Read_ShuntVoltage(I2C_HandleTypeDef *hi2c)
{
    uint8_t rx_data[2];

    if (HAL_I2C_Mem_Read(hi2c, INA226_I2C_ADDR, INA226_REG_SHUNT,
                         1, rx_data, 2, 100) != HAL_OK) {
        return 0.0f;
    }

    /* 高字节在前，组合成 16 位有符号数 */
    raw_v1 = (int16_t)((rx_data[0] << 8) | rx_data[1]);
    return raw_v1 * 2.5e-6f;  /* LSB = 2.5 µV */
}

/**
 * @brief  读取总线电压
 * @param  hi2c I2C 句柄
 * @retval 总线电压值，单位 V（量程 0~36 V）；读取失败返回 0.0f
 * @note   总线电压 LSB 固定为 1.25 mV
 */
float INA226_Read_BusVoltage(I2C_HandleTypeDef *hi2c)
{
    uint8_t rx_data[2];

    if (HAL_I2C_Mem_Read(hi2c, INA226_I2C_ADDR, INA226_REG_BUS,
                         1, rx_data, 2, 100) != HAL_OK) {
        return 0.0f;
    }

    raw_v = (rx_data[0] << 8) | rx_data[1];
    return raw_v * 1.25e-3f;  /* LSB = 1.25 mV */
}

/* 电流读取缓冲区（原工程定义为全局变量，移植时保持不变） */
uint8_t rx_data1[2];

/**
 * @brief  读取电流
 * @param  hi2c I2C 句柄
 * @retval 电流值，单位 A；读取失败返回 0.0f
 * @note   电流 = 原始有符号值 × g_CurrentLSB
 */
float INA226_Read_Current(I2C_HandleTypeDef *hi2c)
{
    if (HAL_I2C_Mem_Read(hi2c, INA226_I2C_ADDR, INA226_REG_CURRENT,
                         1, rx_data1, 2, 100) != HAL_OK) {
        return 0.0f;
    }

    raw_i = (int16_t)((rx_data1[0] << 8) | rx_data1[1]);
    return raw_i * g_CurrentLSB;
}

/**
 * @brief  读取功率
 * @param  hi2c I2C 句柄
 * @retval 功率值，单位 W；读取失败返回 0.0f
 * @note   功率 LSB = 25 × Current_LSB
 */
float INA226_Read_Power(I2C_HandleTypeDef *hi2c)
{
    uint8_t rx_data[2];

    if (HAL_I2C_Mem_Read(hi2c, INA226_I2C_ADDR, INA226_REG_POWER,
                         1, rx_data, 2, 100) != HAL_OK) {
        return 0.0f;
    }

    raw_p = (rx_data[0] << 8) | rx_data[1];
    return raw_p * 25.0f * g_CurrentLSB;
}
