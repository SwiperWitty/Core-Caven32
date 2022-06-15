/**
  ******************************************************************************
  * File   : RTC/LSI_Calib/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "at32_board.h"
#include "main.h"
#include <stdio.h>

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup RTC_LSI_Calib
  * @{
  */ 
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define RTCClockOutput_Enable

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TMR_TimerBaseInitType TMR_TimeBaseStructure;
TMR_ICInitType TMR_ICInitStructure;
RCC_ClockType RCC_Clocks;
__IO uint32_t PeriodValue = 0,  LsiFreq = 0;
__IO uint32_t OperationComplete = 0;

/* Private function prototypes -----------------------------------------------*/
void RTC_Configuration(void);
void NVIC_Configuration(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	/* AT Board Initial */
	AT32_Board_Init();

	/* USART configuration */
	UART_Print_Init(115200);

	/* RTC Configuration */
	RTC_Configuration();

	printf("\r\n\nStart Calib");

	/* Get the Frequency value */
	RCC_GetClocksFreq(&RCC_Clocks);

	/* Enable TMR5 APB1 clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR5, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO, ENABLE);
	
	/* Connect internally the TM5_CH4 Input Capture to the LSI clock output */
	GPIO_PinsRemapConfig(GPIO_Remap_TMR5CH4_LSI, ENABLE);
	
	/* TMR5 Time base configuration */
	TMR_TimeBaseStructInit(&TMR_TimeBaseStructure);
	TMR_TimeBaseStructure.TMR_DIV = 0;
	TMR_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;
	TMR_TimeBaseStructure.TMR_Period = 0xFFFF;
	TMR_TimeBaseStructure.TMR_ClockDivision = TMR_CKD_DIV1;
	TMR_TimeBaseInit(TMR5, &TMR_TimeBaseStructure);

	/* TMR5 Channel4 Input capture Mode configuration */
	TMR_ICStructInit(&TMR_ICInitStructure);
	TMR_ICInitStructure.TMR_Channel = TMR_Channel_4;
	TMR_ICInitStructure.TMR_ICPolarity = TMR_ICPolarity_Rising;
	TMR_ICInitStructure.TMR_ICSelection = TMR_ICSelection_DirectTI;
	TMR_ICInitStructure.TMR_ICDIV = TMR_ICDIV_DIV1;
	TMR_ICInitStructure.TMR_ICFilter = 0;
	TMR_ICInit(TMR5, &TMR_ICInitStructure);

	/* Reinitialize the index for the interrupt */
	OperationComplete = 0;

	/* Enable the TMR5 Input Capture counter */
	TMR_Cmd(TMR5, ENABLE);
	/* Reset all TMR5 flags */
	TMR5->STS = 0;
	/* Enable the TMR5 channel 4 */
	TMR_INTConfig(TMR5, TMR_INT_CC4, ENABLE);

	/* NVIC configuration */
	NVIC_Configuration();

	printf("\r\nBefore capture");

	/* Wait the TMR5 measuring operation to be completed */
	while (OperationComplete != 2);

	printf("\r\ncapture success");

	/* Compute the actual frequency of the LSI. (TIM5_CLK = 1 * PCLK1)  */
	if (PeriodValue != 0)
		LsiFreq = (uint32_t)((uint32_t)(RCC_Clocks.APB1CLK_Freq * 2) / (uint32_t)PeriodValue);

	printf("\r\nPeriodValue=%d, RCC_Clocks.APB1CLK_Freq=%d", PeriodValue, RCC_Clocks.APB1CLK_Freq);
	printf("\r\nLSI_Freq=%d", LsiFreq);

	/* Adjust the RTC prescaler value */
	RTC_SetDIV((LsiFreq - 1));

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

	/* Turn on LED2 */
	AT32_LEDn_ON(LED2);

	while (1)
	{
	}
}

/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
	NVIC_InitType NVIC_InitStructure;

	/* Configure one bit for preemption priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	/* Enable the RTC Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable the TMR5 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TMR5_GLOBAL_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Configures the RTC.
  * @param  None
  * @retval None
  */
void RTC_Configuration(void)
{
	/* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_PWR | RCC_APB1PERIPH_BKP, ENABLE);

	/* Allow access to BKP Domain */
	PWR_BackupAccessCtrl(ENABLE);

	/* Reset Backup Domain */
	BKP_Reset();

	/* Enable the LSI OSC */
	RCC_LSICmd(ENABLE);
	/* Wait till LSI is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSISTBL) == RESET)
	{}
	/* Select the RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSelection_LSI);

	/* Enable RTC Clock */
	RCC_RTCCLKCmd(ENABLE);

	/* Wait for RTC registers synchronization */
	RTC_WaitForSynchro();

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

	/* Enable the RTC Second */
	RTC_INTConfig(RTC_INT_PACE, ENABLE);

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

	/* Set RTC prescaler: set RTC period to 1sec */
	RTC_SetDIV(40000);

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

	/* To output second signal on Tamper pin, the tamper functionality
			 must be disabled (by default this functionality is disabled) */
	BKP_TamperPinCmd(DISABLE);

	/* Enable the RTC Second Output on Tamper Pin */
	BKP_RTCOutputConfig(BKP_RTCOutput_Second_Pulse);
}

/**
  * @brief  Increments OperationComplete variable and return its value
  *         before increment operation.
  * @param  None
  * @retval OperationComplete value before increment
  */
uint32_t IncrementVar_OperationComplete(void)
{
	OperationComplete++;

	return (uint32_t)(OperationComplete - 1);
}

/**
  * @brief  Returns OperationComplete value.
  * @param  None
  * @retval OperationComplete value
  */
uint32_t GetVar_OperationComplete(void)
{
	return (uint32_t)OperationComplete;
}

/**
  * @brief  Sets the PeriodValue variable with input parameter.
  * @param  Value: Value of PeriodValue to be set.
  * @retval None
  */
void SetVar_PeriodValue(uint32_t Value)
{
	PeriodValue = (uint32_t)(Value);
}

/**
  * @}
  */ 

/**
  * @}
  */ 

