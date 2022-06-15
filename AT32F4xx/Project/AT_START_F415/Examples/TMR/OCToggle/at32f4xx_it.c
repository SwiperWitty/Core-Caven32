/**
  ******************************************************************************
  * File   : TMR/OCToggle/at32f4xx_it.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "at32f4xx_it.h"

/** @addtogroup AT32F415_StdPeriph_Examples
  * @{
  */

/** @addtogroup TMR_OCToggle
  * @{
  */

uint16_t capture = 0;
extern __IO uint16_t CCR1_Val;
extern __IO uint16_t CCR2_Val;
extern __IO uint16_t CCR3_Val;
extern __IO uint16_t CCR4_Val;

void TMRingDelay_Decrement(void);
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
//  TMRingDelay_Decrement();
}

/******************************************************************************/
/*                 AT32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_at32f413_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles TMR3 global interrupt request.
  * @param  None
  * @retval None
  */
void TMR3_GLOBAL_IRQHandler(void)
{
  /* TMR3_CH1 toggling with frequency = 183.1 Hz */
  if (TMR_GetINTStatus(TMR3, TMR_INT_CC1) != RESET)
  {
    TMR_ClearITPendingBit(TMR3, TMR_INT_CC1 );
    capture = TMR_GetCapture1(TMR3);
    TMR_SetCompare1(TMR3, capture + CCR1_Val );
  }

  /* TMR3_CH2 toggling with frequency = 366.2 Hz */
  if (TMR_GetINTStatus(TMR3, TMR_INT_CC2) != RESET)
  {
    TMR_ClearITPendingBit(TMR3, TMR_INT_CC2);
    capture = TMR_GetCapture2(TMR3);
    TMR_SetCompare2(TMR3, capture + CCR2_Val);
  }

  /* TMR3_CH3 toggling with frequency = 732.4 Hz */
  if (TMR_GetINTStatus(TMR3, TMR_INT_CC3) != RESET)
  {
    TMR_ClearITPendingBit(TMR3, TMR_INT_CC3);
    capture = TMR_GetCapture3(TMR3);
    TMR_SetCompare3(TMR3, capture + CCR3_Val);
  }

  /* TMR3_CH4 toggling with frequency = 1464.8 Hz */
  if (TMR_GetINTStatus(TMR3, TMR_INT_CC4) != RESET)
  {
    TMR_ClearITPendingBit(TMR3, TMR_INT_CC4);
    capture = TMR_GetCapture4(TMR3);
    TMR_SetCompare4(TMR3, capture + CCR4_Val);
  }
}

/**
  * @}
  */ 

/**
  * @}
  */

