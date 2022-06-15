/**
  **************************************************************************
  * File   : usart.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file provides a set of functions abourt USART.
  **************************************************************************
  */
 
#include "sys.h"
#include "usart.h"	  

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup CortexM4_FPU_LCD_Display
  * @{
  */ 
  
/**
  * @brief  Retargets the C library printf function to the USART1.
  * @param
  * @retval
  */
int fputc(int ch, FILE *f)
{
  while((USART1->STS & 0X40) == 0)
    ;

  USART1->DT = (u8)ch;
  return ch;
}

/**
  * @brief  Initialize USART1.
  * @param  None
  * @retval None
  */
void Uart1_Init(u32 bound)
{
  GPIO_InitType GPIO_InitStructure;
  USART_InitType USART_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_USART1 | RCC_APB2PERIPH_GPIOA, ENABLE);	

  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_9; 
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
  GPIO_Init(GPIOA, &GPIO_InitStructure); 

  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
  GPIO_Init(GPIOA, &GPIO_InitStructure); 

  USART_InitStructure.USART_BaudRate = bound; 
  USART_InitStructure.USART_WordLength = USART_WordLength_8b; 
  USART_InitStructure.USART_StopBits = USART_StopBits_1; 
  USART_InitStructure.USART_Parity = USART_Parity_No; 
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	

  USART_Init(USART1, &USART_InitStructure);
  USART_Cmd(USART1, ENABLE); 
}
/**
  * @}
  */ 

/**
  * @}
  */ 
  
