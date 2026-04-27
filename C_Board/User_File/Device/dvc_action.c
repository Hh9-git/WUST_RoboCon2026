#include "dvc_action.h"

posture_t Action;

/* 姿态共用体 */
static union
{
    uint8_t data[24];
    float postureData[6];
}action;

uint8_t receiveData[28]; // 从码盘接收的数据

void Action_Callback(uint8_t *pData, uint16_t size)
{
    // 如果数据包的帧头或帧尾错误需要重新校准
    if (receiveData[0] != 0x0D || receiveData[1] != 0x0A || receiveData[26] != 0x0A || receiveData[27] != 0x0D)
    {
        HAL_UARTEx_ReceiveToIdle_DMA(&ACTION_UART, receiveData, 28);

        // 调整数据帧，使下一次串口接收的28个字节正确匹配数据帧
        for (uint8_t i = 0; i < 28; i++)
        {
            // 检测帧头的位置
            if (receiveData[i] == 0x0D && receiveData[i + 1] == 0x0A)
            {
                HAL_UARTEx_ReceiveToIdle_DMA(&ACTION_UART, receiveData, 28);
                break;
            }
        }
    }

    // 数据解析
    for (uint8_t i = 0; i < 24; i++)
    {
        action.data[i] = receiveData[i + 2];
    }
    Action.yaw = action.postureData[0];
    Action.pos_x = action.postureData[3] - Action.offset_x;
    Action.pos_y = action.postureData[4] - Action.offset_y;
}

void Action_Init(void)
{
    Action_ClearData();
    AttachInterrupt_UART_DMA(&ACTION_UART, receiveData, 28, Action_Callback);
    HAL_UARTEx_ReceiveToIdle_DMA(&ACTION_UART, receiveData, 28);
}

void Action_ClearData(void)
{
    HAL_UART_Transmit(&ACTION_UART, (uint8_t *)"ACT0", 5, HAL_MAX_DELAY);
}

/* Action校准 */
void Action_Correct(void)
{
    HAL_UART_Transmit(&ACTION_UART, (uint8_t *)"ACTR", 5, HAL_MAX_DELAY);
}
