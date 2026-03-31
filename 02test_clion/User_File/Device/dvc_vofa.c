#include "dvc_vofa.h"
#include "usart.h"


pid_para_t para;

uint8_t RxBuffer[1];//串口接收缓冲
uint16_t RxLine;//指令长度初始值设置为零
uint8_t DataBuff[commandlength];//指令内容



#if VOFA_DATA_FORMAT==0

void firewater_displaydata(float position_target,float position_actual,float position_out,float speed_target,float speed_actual,float speed_out)
{
    static uint32_t vofa_tick=0;
    if(uwTick-vofa_tick<50)return;
    vofa_tick=uwTick;

    UART_Print("%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\r\n",position_target,position_actual,position_out,speed_target,speed_actual,speed_out);

}

#endif



#if  VOFA_DATA_FORMAT==1

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

void JustFloat_test(void)	//justfloat 数据协议测试
{
    //    float a=1,b=2;	//发送的数据 两个通道
    USART_PID_Adjust(0);
    uint8_t byte[4]={0};		//float转化为4个字节数据
    uint8_t tail[4]={0x00, 0x00, 0x80, 0x7f};	//帧尾

    //向上位机发送两个通道数据
    Float_to_Byte(para.speed_kp,byte);
    Serial_SendArray(byte,4);	//1转化为4字节数据 就是  0x00 0x00 0x80 0x3F

    Float_to_Byte(para.speed_ki,byte);
    Serial_SendArray(byte,4);	//2转换为4字节数据 就是  0x00 0x00 0x00 0x40

    //发送帧尾
    Serial_SendArray(tail,4);	//帧尾为 0x00 0x00 0x80 0x7f

}

void justfloat_displaydata(float position_target,float position_actual,float position_out,float speed_target,float speed_actual,float speed_out)
{
    // USART_PID_Adjust(0);
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

#endif

