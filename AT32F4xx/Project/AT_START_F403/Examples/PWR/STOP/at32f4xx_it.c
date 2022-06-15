/**
  **************************************************************************
  * File   :PWR/STOP/at32f4xx_it.c
  * Brief  : Main Interrupt Service Routines.
  *              This file provides template for all exceptions handler and peripherals
  *              interrupt service routine.
  **************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx_it.h"
#include "at32_board.h"

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup PWR_STOP
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
  * @brief  This function handles RTC Alarm interrupt request.
  * @param  None
  * @retval None
  */
void RTCAlarm_IRQHandler(void)
{
	if(RTC_GetINTStatus(RTC_INT_ALA) != RESET)
	{
		AT32_LEDn_Toggle(LED4);                      ///<Toggle LED4
		EXTI_ClearIntPendingBit(EXTI_Line17);        ///<Clear EXTI line17 pending bit
		if(PWR_GetFlagStatus(PWR_FLAG_WUF) != RESET) ///<Check if the Wake-Up flag is set
		{
			PWR_ClearFlag(PWR_FLAG_WUF);             ///<Clear Wake Up flag
		}
		RTC_WaitForLastTask();                       ///<Wait until last write operation on RTC registers has finished
		RTC_ClearINTPendingBit(RTC_INT_ALA);         ///<Clear RTC Alarm interrupt pending bit
		RTC_WaitForLastTask();                       ///<Wait until last write operation on RTC registers has finished
	}
}

/**
  * @}
  */ 

/**
  * @}
  */ 

