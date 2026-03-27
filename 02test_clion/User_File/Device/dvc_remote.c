
#include "dvc_remote.h"


//参数定义

//extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart1;

uint8_t rx_buf[64] = {0};		//接收缓冲区
uint8_t rx_pointer;				//接收缓冲区指针
uint8_t data_pointer;			//数据包起始位置指针
uint8_t rx_flag;				//接收完成指示
uint8_t rx_data;				//接收数据

remote_t *remote_ctrl;			//遥控数据包


//函数定义

/**
  * @brief  库DEBUG
  * @param  无
  * @retval 无（结果输出到串口）
  */
void __remote_debug(void)
{
	HAL_UART_Transmit(&huart2,rx_buf,64,1000);
	HAL_UART_Transmit(&huart2,"\r\n",3,1000);
}

/**
  * @brief  数据包反序列化
  * @param  buf 数据包头指针
  * @retval 无（返回给指定结构体）
  */
//void __remote_sbus_deserialize(const uint8_t *buf)
//{
//	remote_ctrl->X_R = ((int16_t)buf[ 2] >> 0 | ((int16_t)buf[ 3] << 8 )) & 0x07FF;
//	remote_ctrl->Y_R = ((int16_t)buf[ 3] >> 3 | ((int16_t)buf[ 4] << 5 )) & 0x07FF;
//	remote_ctrl->Y_L = ((int16_t)buf[ 4] >> 6 | ((int16_t)buf[ 5] << 2 )  | (int16_t)buf[ 6] << 10 ) & 0x07FF;
//	remote_ctrl->X_L = ((int16_t)buf[ 6] >> 1 | ((int16_t)buf[ 7] << 7 )) & 0x07FF;
//	remote_ctrl->key_a = ((int16_t)buf[ 7] >> 4 | ((int16_t)buf[ 8] << 4 )) & 0x07FF;
//	remote_ctrl->key_b = ((int16_t)buf[ 8] >> 7 | ((int16_t)buf[ 9] << 1 )  | (int16_t)buf[10] <<  9 ) & 0x07FF;
//	remote_ctrl->key_c = ((int16_t)buf[10] >> 2 | ((int16_t)buf[11] << 6 )) & 0x07FF;
//	remote_ctrl->key_d = ((int16_t)buf[11] >> 5 | ((int16_t)buf[12] << 3 )) & 0x07FF;
//
//	remote_ctrl->adc_a = ((int16_t)buf[13] << 0 | ((int16_t)buf[14] << 8 )) & 0x07FF;
//	remote_ctrl->adc_b = ((int16_t)buf[14] >> 3 | ((int16_t)buf[15] << 5 )) & 0x07FF;
////	CH[10] = ((int16_t)buf[15] >> 6 | ((int16_t)buf[16] << 2 )  | (int16_t)buf[17] << 10 ) & 0x07FF;
////	CH[11] = ((int16_t)buf[17] >> 1 | ((int16_t)buf[18] << 7 )) & 0x07FF;
////	CH[12] = ((int16_t)buf[18] >> 4 | ((int16_t)buf[19] << 4 )) & 0x07FF;
////	CH[13] = ((int16_t)buf[19] >> 7 | ((int16_t)buf[20] << 1 )  | (int16_t)buf[21] <<  9 ) & 0x07FF;
////	CH[14] = ((int16_t)buf[21] >> 2 | ((int16_t)buf[22] << 6 )) & 0x07FF;
////	CH[15] = ((int16_t)buf[22] >> 5 | ((int16_t)buf[23] << 3 )) & 0x07FF;
//}
void __remote_sbus_deserialize(const uint8_t *buf)
{
	remote_ctrl->X_R = ((int16_t)buf[ 1] >> 0 | ((int16_t)buf[ 2] << 8 )) & 0x07FF;//C0
	remote_ctrl->Y_R = ((int16_t)buf[ 2] >> 3 | ((int16_t)buf[ 3] << 5 )) & 0x07FF;//C1
	remote_ctrl->Y_L = ((int16_t)buf[ 3] >> 6 | ((int16_t)buf[ 4] << 2 )  | (int16_t)buf[ 5] << 10 ) & 0x07FF;
	remote_ctrl->X_L = ((int16_t)buf[ 5] >> 1 | ((int16_t)buf[ 6] << 7 )) & 0x07FF;//C3
	remote_ctrl->key_a = ((int16_t)buf[ 6] >> 4 | ((int16_t)buf[ 7] << 4 )) & 0x07FF;//C4
	remote_ctrl->key_b = ((int16_t)buf[ 7] >> 7 | ((int16_t)buf[ 8] << 1 )  | (int16_t)buf[9] <<  9 ) & 0x07FF;//C5
	remote_ctrl->key_c = ((int16_t)buf[9] >> 2 | ((int16_t)buf[10] << 6 )) & 0x07FF;//C6
	remote_ctrl->key_d = ((int16_t)buf[10] >> 5 | ((int16_t)buf[11] << 3 )) & 0x07FF;//C7

	remote_ctrl->adc_a = ((int16_t)buf[12] << 0 | ((int16_t)buf[13] << 8 )) & 0x07FF;//C8
	remote_ctrl->adc_b = ((int16_t)buf[13] >> 3 | ((int16_t)buf[14] << 5 )) & 0x07FF;//C9
//	CH[10] = ((int16_t)buf[15] >> 6 | ((int16_t)buf[16] << 2 )  | (int16_t)buf[17] << 10 ) & 0x07FF;
//	CH[11] = ((int16_t)buf[17] >> 1 | ((int16_t)buf[18] << 7 )) & 0x07FF;
//	CH[12] = ((int16_t)buf[18] >> 4 | ((int16_t)buf[19] << 4 )) & 0x07FF;
//	CH[13] = ((int16_t)buf[19] >> 7 | ((int16_t)buf[20] << 1 )  | (int16_t)buf[21] <<  9 ) & 0x07FF;
//	CH[14] = ((int16_t)buf[21] >> 2 | ((int16_t)buf[22] << 6 )) & 0x07FF;
//	CH[15] = ((int16_t)buf[22] >> 5 | ((int16_t)buf[23] << 3 )) & 0x07FF;
}

/**
  * @brief  周期运行任务，用于更新遥控数据
  * @param  无
  * @retval 无（返回给用户指定结构体）
  */
void remote_task(void)
{
	if(rx_flag)
	{
		__remote_sbus_deserialize(&rx_buf[data_pointer]);
		rx_flag = 0;
	}
}

/**
  * @brief  初始化库
  * @param  ctrl 由用户提供的数据包结构体
  * @retval 无
  */
void remote_init(remote_t *ctrl)
{
	remote_ctrl = ctrl;
	HAL_UART_Receive_IT(&huart1,&rx_data,1);
}




//回调函数，不应该被用户调用

// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
// 	if(huart->Instance == USART1){
// 		//接收数据存入缓冲区
// 		rx_buf[rx_pointer++] = rx_data;
//
// 		//判断是否有数据包需要处理
// 		if(rx_data == 0x00 && rx_pointer >= 24 && rx_buf[rx_pointer - 25] == 0x0f){
// 			rx_flag = 1;
// 			data_pointer = rx_pointer - 25;
// 			rx_pointer = 0;
// 		}
//
// 		//开启新一轮传输
// 		HAL_UART_Receive_IT(&huart1,&rx_data,1);
// 	}
//
// }
//
//
