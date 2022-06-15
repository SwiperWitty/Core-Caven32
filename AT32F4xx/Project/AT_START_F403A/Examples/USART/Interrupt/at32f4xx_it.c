/**
  ******************************************************************************
  * File   : USART/Interrupt/at32f4xx_it.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx_it.h"

/** @addtogroup AT32F403A_StdPeriph_Examples
  * @{
  */


/** @addtogroup USART_Interrupt
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern uint8_t TxBuffer1[]; 
extern uint8_t TxBuffer2[]; 
extern uint8_t RxBuffer1[];
extern uint8_t RxBuffer2[];
extern __IO uint8_t TxCounter1;
extern __IO uint8_t TxCounter2;
extern __IO uint8_t RxCounter1; 
extern __IO uint8_t RxCounter2;
extern uint8_t NbrOfDataToTransfer1;
extern uint8_t NbrOfDataToTransfer2;
extern uint8_t NbrOfDataToRead1;
extern uint8_t NbrOfDataToRead2;

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
  * @brief  This function handles USART2 global interrupt request.
  * @param  None
  * @retval None
  */
void USART2_IRQHandler(void)
{
  if(USART_GetITStatus(USART2, USART_INT_RDNE) != RESET)
  {
    /* Read one byte from the receive data register */
    RxBuffer1[RxCounter1++] = USART_ReceiveData(USART2);

    if(RxCounter1 == NbrOfDataToRead1)
    {
      /* Disable the USART2 Receive interrupt */
      USART_INTConfig(USART2, USART_INT_RDNE, DISABLE);
    }
  }
  
  if(USART_GetITStatus(USART2, USART_INT_TDE) != RESET)
  {   
    /* Write one byte to the transmit data register */
    USART_SendData(USART2, TxBuffer1[TxCounter1++]);

    if(TxCounter1 == NbrOfDataToTransfer1)
    {
      /* Disable the USART2 Transmit interrupt */
      USART_INTConfig(USART2, USART_INT_TDE, DISABLE);
    }    
  }
}

/**
  * @brief  This function handles USART3 global interrupt request.
  * @param  None
  * @retval None
  */
void USART3_IRQHandler(void)
{
  if(USART_GetITStatus(USART3, USART_INT_RDNE) != RESET)
  {
    /* Read one byte from the receive data register */
    RxBuffer2[RxCounter2++] = USART_ReceiveData(USART3);

    if(RxCounter2 == NbrOfDataToRead1)
    {
      /* Disable the USART3 Receive interrupt */
      USART_INTConfig(USART3, USART_INT_RDNE, DISABLE);
    }
  }
  
  if(USART_GetITStatus(USART3, USART_INT_TDE) != RESET)
  {   
    /* Write one byte to the transmit data register */
    USART_SendData(USART3, TxBuffer2[TxCounter2++]);

    if(TxCounter2 == NbrOfDataToTransfer2)
    {
      /* Disable the USART3 Transmit interrupt */
      USART_INTConfig(USART3, USART_INT_TDE, DISABLE);
    }
  }
}

/**
  * @}
  */ 

/**
  * @}
  */ 


