#ifndef _MAIN__H_
#define _MAIN__H_

#include "Items.h"
#include "Base_Sys_time.h" 
#include "Base_Exist_GPIO.h" 

#include "bt.h"
#include "lpm.h"

#ifndef __UART_H__
#include "lpuart.h" 
#endif 


void Main_Init (void);

void Uart_lp_Init (int Set);

void TxIntCallback(void);
void RxIntCallback(void);
void ErrIntCallback(void);

/*
**		Hardware Map 
** KEY	P03
** RX	P33 
** TX	P34
** GP0	P23 
** GPI	P24 
** MOSA	P26 
** MOSB	P25 
** ADC	P35
** 
*/

#endif 
