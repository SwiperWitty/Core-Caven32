/**
  ******************************************************************************
  * File   : FreeRTOS/QM_Mailbox/TimerForRunTimeState.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Bsp Service Routines.
  ******************************************************************************
  */ 
  
/* Includes ------------------------------------------------------------------*/  
#include "bsp.h"

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup FreeRTOS_QM_Mailbox
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define  timerINTERRUPT_FREQUENCY	20000 //50us
#define  timerHIGHEST_PRIORITY		1

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//the time base for gathering task statistics  
volatile uint32_t ulHighFrequencyTimerTicks = 0UL;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Configure timer6 for gather task statistcs
  * @param  None
  * @retval None
  */
void vTimerInitForRunTimeState(void)
{
	bsp_SetTMRforInt(TMR6, timerINTERRUPT_FREQUENCY, timerHIGHEST_PRIORITY, 0);
}

/**
  * @brief  Interrupt handler for timer 6
  * @param  None
  * @retval None
  */
void TMR6_GLOBAL_IRQHandler( void )
{
	if(TMR_GetINTStatus(TMR6, TMR_INT_Overflow) != RESET)
	{
		ulHighFrequencyTimerTicks++;
		TMR_ClearITPendingBit(TMR6, TMR_INT_Overflow);
	}
}


/**
  * @}
  */ 

/**
  * @}
  */ 

