#include "dvc_vofa.h"
#include "usart.h"
#include "string.h"
#include "drv_usart.h"

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
    HAL_UART_Transmit(VOFA_UART,&Byte,sizeof(Byte),HAL_MAX_DELAY);
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


void Vofa_Callback(uint8_t *data, uint16_t size)
{

    RxLine++;                      //每接收到一个数据，进入回调数据长度加1
    DataBuff[RxLine-1]=RxBuffer[0];  //把每次接收到的数据保存到缓存数组
    if(RxBuffer[0]==frametail)            //接收结束标志位，这个数据可以自定义，根据实际需求，这里只做示例使用，不一定是0x21
    {
        /**********************
        * 下面是调试代码，可以在串口助手上查看接收的数据
         *********************/
        UART_Print("RXLen=%d\r\n",RxLine);
        for(int i=0;i<RxLine;i++)
           UART_Print("UART DataBuff[%d] = %c\r\n",i,DataBuff[i]);
        USART_PID_Adjust(0);//数据解析和参数赋值函数
        memset(DataBuff,0,sizeof(DataBuff));  //清空缓存数组
        RxLine=0;  //清空接收长度
    }
    RxBuffer[0]=0;
    // HAL_UART_Receive_IT(&huart1,RxBuffer,1);
    // // HAL_UART_Receive_DMA(&huart1,(uint8_t *)RxBuffer,1);
    HAL_UARTEx_ReceiveToIdle_DMA(VOFA_UART, (uint8_t *)RxBuffer, 1); //每接收一个数据，就打开一次串口中断接收，否则只会接收一个数据就停止接收
}
/*
 * 解析出DataBuff中的数据
 * 返回解析得到的数据
 */
float Get_Data(void)
{
    uint8_t data_Start_Num = 0; // 记录数据位开始的地方
    uint8_t data_End_Num = 0; // 记录数据位结束的地方
    uint8_t data_Num = 0; // 记录数据位数
    uint8_t minus_Flag = 0; // 判断是不是负数
    float data_return = 0; // 解析得到的数据
    for(uint8_t i=0;i<commandlength;i++) // 查找等号和感叹号的位置
    {
        if(DataBuff[i] == framehead) data_Start_Num = i + 1; // +1是直接定位到数据起始位
        if(DataBuff[i] == frametail)
        {
            data_End_Num = i - 1;
            break;
        }
    }
    if(DataBuff[data_Start_Num] == '-') // 如果是负数
    {
        data_Start_Num += 1; // 后移一位到数据位
        minus_Flag = 1; // 负数flag
    }
    data_Num = data_End_Num - data_Start_Num + 1;
    if(data_Num == 4) // 数据共4位
    {
        data_return = (DataBuff[data_Start_Num]-48)  + (DataBuff[data_Start_Num+2]-48)*0.1f +
                (DataBuff[data_Start_Num+3]-48)*0.01f;
    }
    else if(data_Num == 5) // 数据共5位
    {
        data_return = (DataBuff[data_Start_Num]-48)*10 + (DataBuff[data_Start_Num+1]-48) + (DataBuff[data_Start_Num+3]-48)*0.1f +
                (DataBuff[data_Start_Num+4]-48)*0.01f;
    }
    else if(data_Num == 6) // 数据共6位
    {
        data_return = (DataBuff[data_Start_Num]-48)*100 + (DataBuff[data_Start_Num+1]-48)*10 + (DataBuff[data_Start_Num+2]-48) +
                (DataBuff[data_Start_Num+4]-48)*0.1f + (DataBuff[data_Start_Num+5]-48)*0.01f;
    }
    else if(data_Num == 7) // 数据共7位
    {
        data_return = (DataBuff[data_Start_Num]-48)*1000 + (DataBuff[data_Start_Num+1]-48)*100 + (DataBuff[data_Start_Num+2]-48)*10 +
                (DataBuff[data_Start_Num+4]-48) + (DataBuff[data_Start_Num+5]-48)*0.1f;
    }
    if(minus_Flag == 1)  data_return = -data_return;
    UART_Print("data=%.2f\r\n",data_return);
    return data_return;
}

/*
 * 根据串口信息进行PID调参
 */
void USART_PID_Adjust(uint8_t Motor_n)
{
    float data_Get = Get_Data(); // 存放接收到的数据
    // UART_Print("%.2f,100\r\n",data_Get);
    if(Motor_n == 0)//电机1
    {
        if(DataBuff[0]=='P' && DataBuff[1]=='1') // 位置环P
            para.speed_kp = data_Get;
        else if(DataBuff[0]=='I' && DataBuff[1]=='1') // 位置环I
            para.speed_ki = data_Get;
        else if(DataBuff[0]=='D' && DataBuff[1]=='1') // 位置环D
            para.speed_kd = data_Get;
        else if(DataBuff[0]=='P' && DataBuff[1]=='2') // 速度环P
            para.position_kp = data_Get;
        else if(DataBuff[0]=='I' && DataBuff[1]=='2') // 速度环I
            para.position_ki = data_Get;
        else if(DataBuff[0]=='D' && DataBuff[1]=='2') // 速度环D
            para.position_kd = data_Get;
        //        else if((DataBuff[0]=='S' && DataBuff[1]=='p') && DataBuff[2]=='e') //目标速度
        //            g = data_Get;
        //        else if((DataBuff[0]=='P' && DataBuff[1]=='o') && DataBuff[2]=='s') //目标位置
        //            h = data_Get;
    }
    //    else if(Motor_n == 1) // 电机2
    //    {
    //        if(DataBuff[0]=='P' && DataBuff[1]=='1') // 位置环P
    //            pid_r_position.kp = data_Get;
    //        else if(DataBuff[0]=='I' && DataBuff[1]=='1') // 位置环I
    //            pid_r_position.ki = data_Get;
    //        else if(DataBuff[0]=='D' && DataBuff[1]=='1') // 位置环D
    //            pid_r_position.kd = data_Get;
    //        else if(DataBuff[0]=='P' && DataBuff[1]=='2') // 速度环P
    //            pid_r_speed.kp = data_Get;
    //        else if(DataBuff[0]=='I' && DataBuff[1]=='2') // 速度环I
    //            pid_r_speed.ki = data_Get;
    //        else if(DataBuff[0]=='D' && DataBuff[1]=='2') // 速度环D
    //            pid_r_speed.kd = data_Get;
    //        else if((DataBuff[0]=='S' && DataBuff[1]=='p') && DataBuff[2]=='e') //目标速度
    //            R_Target_Speed = data_Get;
    //        else if((DataBuff[0]=='P' && DataBuff[1]=='o') && DataBuff[2]=='s') //目标位置
    //            R_Target_Position = data_Get;
    //    }
    // UART_Print("dataget=%.2f,speedkp=%.2f\r\n",data_Get,para.speed_kp);
    // UART_Print("%.2f,%.2f\r\n",data_Get,para.speed_kp);
}





