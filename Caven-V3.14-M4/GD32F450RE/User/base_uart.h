#ifndef _BASE__UART_H
#define _BASE__UART_H

/*base_uart.h*/

#include "Items.h"

int uart0_init(int baud,int set);
void UART0_Send_Data(const void * data,int size);
void DMA_UART0_Send_Data(const void * data,int size);

void  Ser_Printf (u8 *format, ...);

#endif

