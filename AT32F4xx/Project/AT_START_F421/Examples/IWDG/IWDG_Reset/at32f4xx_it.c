/**
  ******************************************************************************
  * File   : IWDG/IWDG_Reset/at32f4xx_it.c 
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
  
/** @addtogroup IWDG_Reset
  * @{
  */

__IO uint16_t IC1ReadValue1 = 0, IC1ReadValue2 = 0;
__IO uint16_t CaptureNumber = 0;
__IO uint32_t Capture = 0;
extern uint32_t LsiFreq;
extern uint8_t EXTI_flag;

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}


void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
}

/******************************************************************************/
/*            AT32F4xx Peripherals Interrupt Handlers                        */
/******************************************************************************/

/**
  * @brief  This function handles External line 0 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI1_0_IRQHandler(void)
{
  if(EXTI_GetIntStatus(EXTI_Line0) != RESET)
  {
    EXTI_flag = 1;
    
    /* Clear the  EXTI line 0 pending bit */
    EXTI_ClearIntPendingBit(EXTI_Line0);
  }
}

/******************************************************************************/
/*                 AT32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_at32f413_xx.s).                                            */
/******************************************************************************/

/**
  * @}
  */

/**
  * @}
  */
  
