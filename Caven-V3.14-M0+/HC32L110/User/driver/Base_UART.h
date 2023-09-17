#ifndef _Base_UART_H__
#define _Base_UART_H__

#ifdef DEFAULT
#include "items.h"              //默认功能
#endif

#ifndef DEFAULT
#include "User_items.h"         //自行设置功能，一般出现在本地文件的User中
#endif


//#define UART_pf

/* 【宏函数群】   */
#ifdef Exist_UART
	#define UART0_EXIST
	#define UART1_EXIST
//	#define UART2_EXIST
//	#define UART3_EXIST
//	#define UART4_EXIST
	// #define UART5_EXIST
	#define UART_End 3

    #define END_Data    '}'             //这个作为串口接收【结束符】                     //因为单片机差异性所以放在这
	#define NO_END      'N'             //如果【结束符】和它相同，那么就没有 结束符

/*	标志位	*/
	#define RXD_Falg    USART_RDBF_FLAG     //  接收标志
	#define TXD_Falg    USART_TDC_FLAG		//  【USART_FLAG_TXE】这个只是说明，数据被cpu取走,【USART_FLAG_TC】这是完全发送完成

/*  中断   */
	#ifdef UART0_EXIST
    	#define UART0_Interrupt() USART0_IRQHandler()
	#endif
	#ifdef UART1_EXIST
    	#define UART1_Interrupt() USART1_IRQHandler()
	#endif
    #ifdef UART2_EXIST
    	#define UART2_Interrupt() USART2_IRQHandler()
	#endif
    #ifdef UART3_EXIST
    	#define UART3_Interrupt() USART3_IRQHandler()
	#endif
	#ifdef UART4_EXIST
    	#define UART4_Interrupt() USART4_IRQHandler()
	#endif
	#ifdef UART5_EXIST
    	#define UART5_Interrupt() USART5_IRQHandler()
	#endif
#endif
/*  end */

//可以有多个UART,建议把应用层也这样写一遍
#ifdef UART0_EXIST
void Uart0_Init(int Baud,int SET);
#endif
#ifdef UART1_EXIST
void Uart1_Init(int Baud,int SET);
#endif
#ifdef UART2_EXIST
void Uart2_Init(int Baud,int SET);
#endif
#ifdef UART3_EXIST
void Uart3_Init(int Baud,int SET);
#endif
#ifdef UART4_EXIST
void Uart4_Init(int Baud,int SET);
#endif
#ifdef UART5_EXIST
void Uart5_Init(int Baud,int SET);
#endif

//发送、接收
uint16_t UART_RXD_Receive(char Channel);            //接收
void UART_TXD_Send(char Channel,uint16_t DATA);     //发送

//接收的标志位，给应用层更大权限、灵活度
char UART_RXD_Flag(char Channel);
void UART_RXD_Flag_Clear(char Channel);

#endif
