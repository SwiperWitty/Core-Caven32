#ifndef _UART_X__H_
#define _UART_X__H_

#include "Items.h"

#define RXD_Falg    USART_IT_RXNE     //  接收标志
#define TXD_Falg    USART_FLAG_TC       //  【USART_FLAG_TXE】这个只是说明，数据被cpu取走,【USART_FLAG_TC】这是完全发送完成

/* 【宏函数群】   */

#define UART_Channel_MAX  2     //最高通道数

#define END_Data    'N'             //这个作为串口接收【结束符】
#define NO_END      'N'             //如果【结束符】和它相同，那么就没有 结束符


#ifdef Exist_UART
/*  中断   */

    #define UART1_Interrupt() USART1_IRQHandler()
    #define UART2_Interrupt() USART2_IRQHandler()
    #define UART3_Interrupt() USART3_IRQHandler()

    void USART1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));      //很关键
    void USART2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));      //很关键
    void USART3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));      //很关键

    #if UART_Channel_MAX >= 4
        #define UART4_Interrupt() UART4_IRQHandler()
    #endif
    #if UART_Channel_MAX >= 5
        #define UART5_Interrupt() UART5_IRQHandler()
    #endif

#endif

/*  end */

void Uart1_Init(int Baud,int SET);
void Uart2_Init(int Baud,int SET);
void Uart3_Init(int Baud,int SET);
#ifdef UART_Channel_MAX
    #if UART_Channel_MAX >= 4
    void Uart4_Init(int Baud,int SET);
    #endif
    #if UART_Channel_MAX >= 5
    void Uart5_Init(int Baud,int SET);
    #endif
#endif

uint16_t UART_RXD_Receive(char Channel);
void UART_TXD_Send(char Channel,uint16_t DATA);

char UART_RXD_Flag(char Channel);
void UART_RXD_Flag_Clear(char Channel);

#endif
