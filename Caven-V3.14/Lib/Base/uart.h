#ifndef _UART_H_
#define _UART_H_

//	可以跨芯片移植	需要大改发送函数底层
/*
	CV_XXX_TypeDef	Caven 特构结构体
	UART使用特定IO不可修改IO相关参数（重映射/复用除外）
	底层
*/

#include "stm32f10x_rcc.h"

#define UART_1 1
#define UART_2 2
#define UART_3 3
#define UART_4 4
#define UART_5 5

#define String_Num_MAX 48	// UART_Send_String 函数最长允许发多长
#define RXD_Num_MAX 48 		// Read_RXD 函数最长允许收多长
#define String_End1  		'\0'
#define String_End2  		'\n'

#define RXD_End1  		'\0'
#define RXD_End2  		'\n'

struct _CV_UART
{
	char UART_x_Array[6][24]; //各串口的数据
	char Read_Flag[6];		 //各串口的读取结束位
	char Rxd_Num[6];		 //各串口接收数量
};

struct _UART
{
	void (*UART_x_Send_String)(char UART_x, char *String);
	void (*UART_x_Send_Data)(char UART_x, char *Data, unsigned char Length);
};


void UART_x_Init(char UART_x, uint32_t Baud, FunctionalState SET);
void UART_x_Send_String(char UART_x, char *String);						//不需要知道长度
void UART_x_Send_Data(char UART_x, char *Data, unsigned char Length);	//需要知道长度,长度公式 n = sizeof(data) / sizeof(data[0])	所有/元素
//结束函数都在中断


#endif
