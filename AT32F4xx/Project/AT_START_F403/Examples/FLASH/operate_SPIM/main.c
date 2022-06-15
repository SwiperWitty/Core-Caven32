/**
  ******************************************************************************
  * File   : FLASH/operate_SPIM/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 
  
/* Includes ------------------------------------------------------------------*/
#include "SPIM_test.h"
#include "at32f4xx.h"
#include "at32_board.h"
#include <stdio.h>

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup FLASH_Operate_SPIM
  * @{
  */

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{ 
  /* USART1 configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  UART_Print_Init(115200);
    
  /* To test ext.flash */
  SPIM_test(); 
  while(1)
  {
  }   
}


/**
  * @}
  */ 

/**
  * @}
  */ 

