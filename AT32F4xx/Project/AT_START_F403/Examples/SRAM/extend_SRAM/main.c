/**
  ******************************************************************************
  * File   : SRAM/extend_SRAM/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 
  
/* Includes ------------------------------------------------------------------*/
#include "extend_SRAM.h"
#include "at32f4xx.h"
#include "at32_board.h"
#include <stdio.h>

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

 /** @addtogroup SRAM_Extended_SRAM
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
  
  /* Check EOPB0 */
#ifdef EXTEND_SRAM_96K

  if(((UOPTB->EOPB0)&0xFF)==0xFF)
  {
  	  printf("Extend 96KB SRAM success.\r\n");
  } else {
  	  printf("Extend 96KB SRAM failed.\r\n");
  }		  
#endif

#ifdef EXTEND_SRAM_224K

  if(((UOPTB->EOPB0)&0xFF)==0xFE)
  {
  	  printf("Extend 224KB SRAM success.\r\n");
  } else {
  	  printf("Extend 224KB SRAM failed.\r\n");
  }		  
#endif
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
  
