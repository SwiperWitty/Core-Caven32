#include "MODE_UART.h"

#ifdef UART1_EXIST
struct _Uart_Data UART1_Data = {0};
#endif
#ifdef UART2_EXIST
struct _Uart_Data UART2_Data = {0};
#endif
#ifdef UART3_EXIST
struct _Uart_Data UART3_Data = {0};
#endif
#ifdef UART4_EXIST
struct _Uart_Data UART4_Data = {0}; 
#endif
#ifdef UART5_EXIST
struct _Uart_Data UART5_Data = {0}; 
#endif

#ifdef UART_End
struct _Uart_Data * CV_UART[UART_End];     //越狱通道
#else
struct _Uart_Data * CV_UART[3];
#endif

void Uart_Init(char Channel, const int Baud,int SET)
{
#ifdef Exist_UART
    switch (Channel)
    {
    case 1:
    #ifdef UART1_EXIST
        Uart1_Init(Baud,SET);
        CV_UART[Channel] = &UART1_Data;                 //数据绑定
    #endif
        break;
    case 2:
    #ifdef UART2_EXIST
        Uart2_Init(Baud,SET);
        CV_UART[Channel] = &UART2_Data;                 //数据绑定
    #endif
        break;
    case 3:
    #ifdef UART3_EXIST
        Uart3_Init(Baud,SET);
        CV_UART[Channel] = &UART3_Data;                 //数据绑定
    #endif
        break;
    case 4:
    #ifdef UART4_EXIST
        Uart4_Init(Baud,SET);
        CV_UART[Channel] = &UART4_Data;                 //数据绑定
    #endif
        #ifdef UART5_EXIST
        Uart5_Init(Baud,SET);
        CV_UART[Channel] = &UART5_Data;                 //数据绑定
    #endif
        break;
    default:
        break;
    }

#endif
}

#ifdef Exist_UART
static char Get_RXD(struct _Uart_Data *Target, const char res) //接收处理函数
{
    if (Target->Rxd_Received < 2)      // 0 允许接收，其他不允许接收
    {
        Target->DATA.index = 1;         //U8内有数据
        Target->UART_RxdBuff[Target->DATA.Length++] = res;
        #if END_Data != NO_END      //如果有 停止符
        if (res == END_Data)    //Get停止符
        {
            Target->Rxd_Received = 2;       //满足停止条件的接收
        }
        #else
        Target->Rxd_Received = 1;           //自由接收模式
        #endif

        #ifdef UART_Length_MAX
        if (Target->DATA.Length > UART_Length_MAX) //超长（异常需要清零）
        {
            Target->Rxd_Received = 3;       //超长的接收
        }
        #endif

        Target->DATA.Poit_U8 = Target->UART_RxdBuff;        //指针绑定，指针同步-RxdBuff

    }
    return Target->Rxd_Received;
}
#endif

#ifdef UART1_EXIST
void UART1_Interrupt()                         //Interrupt
{
    uint8_t temp,Channel = 1;
    if(UART_RXD_Flag(Channel) != 0)
    {
        temp = UART_RXD_Receive(Channel);
        Get_RXD(CV_UART[Channel], temp);
        UART_RXD_Flag_Clear(Channel);
        #ifdef DEBUG_RX_TX
        UART_TXD_Send(Channel,temp);          //debug
        #endif
        
    }
}
#endif

#ifdef UART2_EXIST
void UART2_Interrupt()
{
    uint8_t temp,Channel = 2;
    if(UART_RXD_Flag(Channel) != 0)
    {
        temp = UART_RXD_Receive(Channel);
        Get_RXD(CV_UART[Channel], temp);
        UART_RXD_Flag_Clear(Channel);
        #ifdef DEBUG_RX_TX
        UART_TXD_Send(Channel,temp);          //debug
        #endif
    }
}
#endif

#ifdef UART3_EXIST
void UART3_Interrupt()
{
    uint8_t temp,Channel = 3;
    if(UART_RXD_Flag(Channel) != 0)
    {
        temp = UART_RXD_Receive(Channel);
        Get_RXD(CV_UART[Channel], temp);
        UART_RXD_Flag_Clear(Channel);
        #ifdef DEBUG_RX_TX
        UART_TXD_Send(Channel,temp);          //debug
        #endif
    }
}
#endif

#ifdef UART4_EXIST
void UART4_Interrupt()
{
    uint8_t temp,Channel = 4;
    if(UART_RXD_Flag(Channel) != 0)
    {
        temp = UART_RXD_Receive(Channel);
        Get_RXD(CV_UART[Channel], temp);
        UART_RXD_Flag_Clear(Channel);
        #ifdef DEBUG_RX_TX
        UART_TXD_Send(Channel,temp);          //debug
        #endif
    }
}
#endif

#ifdef UART5_EXIST
void UART5_Interrupt()
{
    uint8_t temp,Channel = 5;
    if(UART_RXD_Flag(Channel) != 0)
    {
        temp = UART_RXD_Receive(Channel);
        Get_RXD(CV_UART[Channel], temp);
        UART_RXD_Flag_Clear(Channel);
        #ifdef DEBUG_RX_TX
        UART_TXD_Send(Channel,temp);          //debug
        #endif
    }
}
#endif


char UART_Send_Data(char Channel, const U8 *Data, int Length)
{
#ifdef Exist_UART
	int temp = MIN(Length,UART_Length_MAX);
    int i = 0;

    while (temp--)
    {
        UART_TXD_Send(Channel,Data[i++]);       //等待标志位在里面  
    }
#endif
    return (1);
}

void UART_Send_String(char Channel, const char *String)
{
    int Length = strlen(String);
    UART_Send_Data(Channel,(U8 *)String,Length);
}

