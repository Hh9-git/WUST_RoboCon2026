#include "dvc_Action.h"
/* 姿态共用体 */
static union
{
    uint8_t data[24];
    float postureData[6];
} action;

uint8_t receiveData[28]; // 从码盘接收的数据
posture_t Action;

void Action_Init(void)
{
    Action_ClearData();

    HAL_UART_Receive_IT(&ACTION_UART, receiveData, 28);

    // 调整数据帧，使下一次串口接收的28个字节正确匹配数据帧
    for (uint8_t i = 0; i < 28; i++)
    {
        // 检测帧头的位置
        if (receiveData[i] == 0x0D && receiveData[i + 1] == 0x0A)
        {
            HAL_UART_Receive_IT(&ACTION_UART, receiveData, i); // 根据错误帧头的位置调整数据包
            break;
        }
    }

    HAL_UART_Receive_DMA(&ACTION_UART, receiveData, 28);
}

void Action_Callback(uint8_t *pData, uint8_t size)
{
    // 如果数据包的帧头或帧尾错误需要重新校准
    if (receiveData[0] != 0x0D || receiveData[1] != 0x0A || receiveData[26] != 0x0A || receiveData[27] != 0x0D)
    {
        HAL_UART_Receive(&ACTION_UART, receiveData, 28, HAL_MAX_DELAY);

        // 调整数据帧，使下一次串口接收的28个字节正确匹配数据帧
        for (uint8_t i = 0; i < 28; i++)
        {
            // 检测帧头的位置
            if (receiveData[i] == 0x0D && receiveData[i + 1] == 0x0A)
            {
                HAL_UART_Receive(&ACTION_UART, receiveData, i, HAL_MAX_DELAY); // 根据错误帧头的位置调整数据包
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
    Action.pos_x = action.postureData[3];
    Action.pos_y = action.postureData[4];
}

// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
//     if (huart == &ACTION_UART)
//     {
//         // __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF); // 添加ORE标志的作用
//         Action_Callback(receiveData, 28);
//         // 开启新的DMA接收
//         HAL_UART_Receive_DMA(&ACTION_UART, receiveData, 28);
//     }
// }

void Action_ClearData(void)
{
    HAL_UART_Transmit(&ACTION_UART, (uint8_t *)"ACT0", 5, HAL_MAX_DELAY);
}
