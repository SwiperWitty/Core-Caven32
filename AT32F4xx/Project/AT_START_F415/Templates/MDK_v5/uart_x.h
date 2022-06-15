#ifndef _UART_X__H_
#define _UART_X__H_

#include "at32f4xx.h"
#include <stdio.h>

struct _uart_x
{
    char Rxd_Received;
    int Rxd_Number;
    unsigned char Rxd_Data[1124];
    char *Rxd_Opinter;
};

extern struct _uart_x Uart_4;               //数据源

void UARTx_Init(char Channel,int bound);
void UARTx_Send_Data(char Channel,const unsigned char * Data,int Length);
void UARTx_Send_String(char Channel,const char * String);

#endif
