#ifndef _MAIN__H_
#define _MAIN__H_

#include "Items.h"
#include "Base_Sys_time.h" 
#include "Base_GPIO.h" 

#include "lpm.h"
#include "lpt.h"
#include "clk.h"
#include "flash.h"

#ifndef __UART_H__
#include "lpuart.h" 
#endif 

typedef enum
{
    m_Clk_H = 1,
    m_Clk_L,
}User_Clk_Type;

void Main_Init (void);

void Uart_lp_Init (int Set);

void TxIntCallback(void);
void RxIntCallback(void);
void ErrIntCallback(void);

int TIME_lp_Init (int Set);
void Low_Pow_MODE(int Set);
void SYSClkwork_Mode (User_Clk_Type Mode);

int User_do_check (void);


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
