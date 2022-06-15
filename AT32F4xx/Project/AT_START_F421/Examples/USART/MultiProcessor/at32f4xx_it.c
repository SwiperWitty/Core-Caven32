/**
  ******************************************************************************
  * File   : USART/MultiProcessor/at32f4xx_it.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx_it.h"

/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */

/** @addtogroup USART_MultiProcessor
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/**
  * @brief  This function handles External lines 0 to 1 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI1_0_IRQHandler(void)
{
  
  /* Wait while USART1 TXE = 0 */
  while(USART_GetFlagStatus(USART1, USART_FLAG_TDE) == RESET)
  {
  }
  /* Send the address mark (0x102) to wakeup USART2 */
  USART_SendData(USART1, 0x102);
  
  /* Clear EXTI Line 0 Pending Bit */
  EXTI_ClearIntPendingBit(EXTI_Line0);
}

/**
  * @brief  This function handles External lines 15 to 4 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI15_4_IRQHandler(void)
{

  if(EXTI_GetIntStatus(EXTI_Line8) != RESET)
  {
    /* Flush DR register and clear the USART2 RXNE flag */
    USART_ReceiveData(USART2);

    /* Enable the USART2 mute mode*/
    USART_ReceiverWakeUpCmd(USART2, ENABLE);

    /* Clear Key Button EXTI Line Pending Bit */
    EXTI_ClearIntPendingBit(EXTI_Line8); 
  }
}

/**
  * @}
  */ 

/**
  * @}
  */ 


