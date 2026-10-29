/**
 * @file    CAN_TASK.c
 * @brief   CAN 发送、滤波器配置与总线错误恢复
 */

/* 包含文件（注意：Linux 下文件名区分大小写，必须与实际文件名一致） */
#include "CAN_TASK.h"
#include "can.h"

/**
 * @brief  通过 CAN 发送 4 个 int16 数据
 * @param  _hcan CAN 句柄（本实现内部使用全局 hcan，参数保留以兼容原接口）
 * @param  stdid 标准帧标识符
 * @param  num1  第 1 个数据
 * @param  num2  第 2 个数据
 * @param  num3  第 3 个数据
 * @param  num4  第 4 个数据
 *
 * 数据格式：每帧固定 8 字节，4 个 int16 按大端（高字节在前）排列。
 * CAN 标准帧数据长度最大 8 字节，正好容纳 4 个 int16。
 */
void CAN_send(CAN_HandleTypeDef *_hcan, int16_t stdid, int16_t num1,
              int16_t num2, int16_t num3, int16_t num4)
{
    CAN_TxHeaderTypeDef tx;
    uint8_t Data[8];
    uint32_t mailbox = 0;

    /* 帧头配置：标准帧、数据帧、DLC = 8 */
    tx.DLC   = 0x08;          /* 数据长度 8 字节 */
    tx.IDE   = CAN_ID_STD;    /* 标准标识符（11 位） */
    tx.RTR   = CAN_RTR_DATA;  /* 数据帧（非远程帧） */
    tx.StdId = stdid;         /* 标准帧 ID */
    tx.ExtId = 0x000;         /* 扩展帧 ID 不使用 */

    /* 4 个 int16 拆成 8 字节，高字节在前（大端） */
    Data[0] = ((num1) >> 8);
    Data[1] = (num1);
    Data[2] = ((num2) >> 8);
    Data[3] = (num2);
    Data[4] = ((num3) >> 8);
    Data[5] = (num3);
    Data[6] = ((num4) >> 8);
    Data[7] = (num4);

    /* 写入发送邮箱。注意：原工程此处使用全局 hcan，而不是参数 _hcan，
     * 调用方传入的也是 &hcan，因此行为一致；移植时保持原样。 */
    HAL_CAN_AddTxMessage(&hcan, &tx, Data, &mailbox);
}

/**
 * @brief  CAN 滤波器初始化并启动 CAN 控制器
 * @param  hcan CAN 句柄
 *
 * 配置说明：
 *   - 先 DeInit/Init 一次，确保从干净的寄存器状态开始；
 *   - 滤波器组 0，32 位掩码模式；
 *   - FilterId 与 FilterMask 全部为 0，表示“接收总线上所有报文”；
 *   - 接收到的报文放入 FIFO0；
 *   - 启动 CAN 并使能 RX FIFO0 消息挂起中断、发送邮箱空中断。
 */
void CAN_Filter_Init(CAN_HandleTypeDef *hcan)
{
    /* 复位并重新初始化 CAN 控制器，清除之前的配置 */
    HAL_CAN_DeInit(hcan);
    HAL_CAN_Init(hcan);

    CAN_FilterTypeDef sFilterConfig;

    sFilterConfig.FilterBank           = 0;                    /* 使用滤波器组 0 */
    sFilterConfig.FilterMode           = CAN_FILTERMODE_IDMASK;/* ID 掩码模式 */
    sFilterConfig.FilterScale          = CAN_FILTERSCALE_32BIT;/* 32 位宽度 */
    sFilterConfig.FilterIdHigh         = 0x0000;               /* 过滤 ID 高 16 位 */
    sFilterConfig.FilterIdLow          = 0x0000;               /* 过滤 ID 低 16 位 */
    sFilterConfig.FilterMaskIdHigh     = 0x0000;               /* 掩码高 16 位：全不关心 */
    sFilterConfig.FilterMaskIdLow      = 0x0000;               /* 掩码低 16 位：全不关心 */
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;         /* 匹配报文入 FIFO0 */
    sFilterConfig.FilterActivation     = ENABLE;               /* 使能该滤波器 */
    sFilterConfig.SlaveStartFilterBank = 14;                   /* 从机滤波器起始组 */

    if (HAL_CAN_ConfigFilter(hcan, &sFilterConfig) != HAL_OK) {
        /* 滤波器配置失败 */
        Error_Handler();
    }

    /* 启动 CAN 控制器 */
    if (HAL_CAN_Start(hcan) != HAL_OK) {
        /* 启动失败 */
        Error_Handler();
    }

    /* 使能 RX FIFO0 有新报文中断 */
    if (HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) {
        /* 中断使能失败 */
        Error_Handler();
    }

    /* 使能发送邮箱空中断 */
    if (HAL_CAN_ActivateNotification(hcan, CAN_IT_TX_MAILBOX_EMPTY) != HAL_OK) {
        /* 中断使能失败 */
        Error_Handler();
    }
}

/**
 * @brief  CAN 错误回调（HAL 弱函数，此处重写）
 * @param  hcan CAN 句柄
 * @note   检测到 Bus-Off（总线关闭）状态时，先停止再启动 CAN，
 *         让控制器重新参与总线通信。
 *         注意：MX_CAN_Init 中已经使能 AutoBusOff，硬件也会自动恢复，
 *         这里的软件恢复逻辑是原工程保留的额外保护。
 */
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
    /* 检查错误状态寄存器中的 Bus-Off 标志 */
    if (hcan->Instance->ESR & CAN_ESR_BOFF)
    {
        HAL_CAN_Stop(hcan);
        HAL_CAN_Start(hcan);
    }
}
