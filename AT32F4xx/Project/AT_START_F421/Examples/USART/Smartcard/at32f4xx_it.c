/**
  ******************************************************************************
  * File   : USART/Smartcard/at32f4xx_it.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx_it.h"
#include "platform_config.h"

/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */
  
/** @addtogroup USART_Smartcard
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern __IO uint32_t CardInserted; 

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
  * @brief  This function handles External lines 9 to 5 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI15_4_IRQHandler(void)
{
  /* Select Smart Card CMDVCC */  
  GPIO_ResetBits(GPIO_CMDVCC, SC_CMDVCC);

  /* Set RSTIN LOW */  
  GPIO_ResetBits(GPIO_RESET, SC_RESET);

  /* Set RSTIN HIGH */  
  GPIO_SetBits(GPIO_RESET, SC_RESET);
    
  /* Clear SC EXTIT Line Pending Bit */
  EXTI_ClearIntPendingBit(SC_EXTI);

  /* Smart card detected */
  CardInserted = 1;
}

/**
  * @brief  This function handles SC_USART global interrupt request.
  * @param  None
  * @retval None
  */
void SC_USART_IRQHandler(void)
{
  /* If the SC_USART detects a parity error */
  if(USART_GetITStatus(SC_USART, USART_INT_PERR) != RESET)
  {
    /* Enable SC_USART RXNE Interrupt (until receiving the corrupted byte) */
    USART_INTConfig(SC_USART, USART_INT_RDNE, ENABLE);
    /* Flush the SC_USART DR register */
    USART_ReceiveData(SC_USART);
  }
  
  if(USART_GetITStatus(SC_USART, USART_INT_RDNE) != RESET)
  {
    /* Disable SC_USART RXNE Interrupt */
    USART_INTConfig(SC_USART, USART_INT_RDNE, DISABLE);
    USART_ReceiveData(SC_USART);
  }
}

/**
  * @}
  */ 

/**
  * @}
  */ 


