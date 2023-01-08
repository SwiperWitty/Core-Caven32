#ifndef _UART_HANDLE__H_
#define _UART_HANDLE__H_

#include "Base.h"
#include "Caven.h"
/*
        【API】      ————C语言环境即可运行
    判断串口结束的 define被丢到底层了(很烦)        ———— 2022.8.18

    //没数据？ 看看 END_Data      
    // DEBUG_RX_TX 看通不通 
*/

#ifdef MCU_Buff_Len
    #define UART_Length_MAX    MCU_Buff_Len     //Buff MAX 长度(来自Items & User_items)
#else
    #define UART_Length_MAX    Buff_Length      //Buff MAX 长度(来自Caven)
#endif

//#define DEBUG_RX_TX                   //

struct _Uart_Data
{
    struct Caven_Data DATA;
    char Rxd_Received;
    U8 UART_RxdBuff[UART_Length_MAX];       //串口数据在这，大小由MCU的底层决定
};


struct _Uart_
{
    void (*WAY_Send_String)(char Channel, const char *String);
    char (*WAY_Send_Data)(char Channel, const U8 *Data, int Length);
    
    struct _Uart_Data **DATA_UART;          //指针的指针，慎用
};

extern struct _Uart_Data * CV_UART[];       //别的地方定义数量，且它是指针数组


void Uart_Init(char Channel, int Baud, int SET);

void UART_Send_String(char Channel, const char *String);
char UART_Send_Data(char Channel, const U8 *Data, int Length);


#endif
