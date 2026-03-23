
#include "dvc_vofa.h"



// 按printf格式写，最后必须加\r\n
void Vofa_FireWater(const char *format, ...)
{
    uint8_t txBuffer[100];
    uint32_t n;
    va_list args;
    va_start(args, format);
    n = vsnprintf((char *)txBuffer, 100, format, args);

    //....在此替换你的串口发送函数...........
    HAL_UART_Transmit_DMA(&huart2, (uint8_t *)txBuffer, n);
    //......................................

    va_end(args);
}

// 输入个数和数组地址
void Vofa_JustFloat(float *_data, uint8_t _num)
{
    uint8_t tempData[256];
    uint8_t temp_end[4] = {0, 0, 0x80, 0x7F};
    float temp_copy[_num];

    memcpy(&temp_copy, _data, sizeof(float) * _num);

    memcpy(tempData, (uint8_t *)&temp_copy, sizeof(temp_copy));
    memcpy(&tempData[_num * 4], &temp_end[0], 4);

    //....在此替换你的串口发送函数...........
    HAL_UART_Transmit_DMA(&huart2, tempData, (_num + 1) * 4);
    //......................................
}



/*
要点提示:
1. float和unsigned long具有相同的数据结构长度
2. union据类型里的数据存放在相同的物理空间
*/
typedef union
{
    float fdata;
    unsigned long ldata;
} FloatLongType;


/*
将浮点数f转化为4个字节数据存放在byte[4]中
*/
void Float_to_Byte(float f,unsigned char byte[])
{
    FloatLongType fl;
    fl.fdata=f;
    byte[0]=(unsigned char)fl.ldata;
    byte[1]=(unsigned char)(fl.ldata>>8);
    byte[2]=(unsigned char)(fl.ldata>>16);
    byte[3]=(unsigned char)(fl.ldata>>24);
}



void justfloat_displaydata(float position_target,float position_actual,float position_out,float speed_target,float speed_actual,float speed_out)
{

    uint8_t byte[4]={0};		//float转化为4个字节数据
    uint8_t tail[4]={0x00, 0x00, 0x80, 0x7f};	//帧尾

    Float_to_Byte(position_target,byte);
    Serial_SendArray(byte,4);

    Float_to_Byte(position_actual,byte);
    Serial_SendArray(byte,4);

    Float_to_Byte(position_out,byte);
    Serial_SendArray(byte,4);

    Float_to_Byte(speed_target,byte);
    Serial_SendArray(byte,4);

    Float_to_Byte(speed_actual,byte);
    Serial_SendArray(byte,4);

    Float_to_Byte(speed_out,byte);
    Serial_SendArray(byte,4);


    Serial_SendArray(tail,4);

}


void Serial_SendByte(uint8_t Byte)
{
    HAL_UART_Transmit(&huart2,&Byte,sizeof(Byte),HAL_MAX_DELAY);
}

void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
    uint16_t i;
    for (i = 0; i < Length; i ++)
    {
        Serial_SendByte(Array[i]);
    }
}