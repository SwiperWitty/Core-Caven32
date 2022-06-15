/**
  ******************************************************************************
  * File   : RTC/LSI_Calib/at32f4xx_it.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "at32f4xx_it.h"
#include "at32_board.h"
#include "main.h" 

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup RTC_LSI_Calib
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint16_t tmpCC4[2] = {0, 0};

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
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
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
  * @brief  This function handles RTC global interrupt request.
  * @param  None
  * @retval None
  */
void RTC_IRQHandler(void)
{
	if (RTC_GetINTStatus(RTC_INT_PACE) != RESET)
	{
		/* Toggle LED3 */
		AT32_LEDn_Toggle(LED3);
		
		/* Clear Interrupt pending bit */
		RTC_ClearINTPendingBit(RTC_FLAG_PACE);
	}
}

/**
  * @brief  This function handles TMR5 global interrupt request.
  * @param  None
  * @retval None
  */
void TMR5_GLOBAL_IRQHandler(void)
{
	uint32_t tmp = 0; 

	if (TMR_GetINTStatus(TMR5, TMR_INT_CC4) == SET)
	{
		tmpCC4[IncrementVar_OperationComplete()] = (uint16_t)(TMR5->CC4);

		TMR_ClearITPendingBit(TMR5, TMR_INT_CC4);

		if (GetVar_OperationComplete() >= 2)
		{
			/* Compute the period length */
			tmp = (uint16_t)(tmpCC4[1] - tmpCC4[0] + 1);
			SetVar_PeriodValue(tmp);

			/* Disable the interrupt */
			TMR_INTConfig(TMR5, TMR_INT_CC4, DISABLE);
			TMR_Cmd(TMR5, DISABLE);
		}
	}
}

/**
  * @}
  */ 

/**
  * @}
  */ 

