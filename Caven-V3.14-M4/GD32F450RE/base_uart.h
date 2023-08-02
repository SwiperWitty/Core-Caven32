#ifndef _BASE__UART_H
#define _BASE__UART_H

/*base_uart.h*/

#include "gd32f4xx.h"
#include <stdio.h>

int uart1_init(int baud,int set);
void UART0_Send_Data(const void * data,int size);



#endif

