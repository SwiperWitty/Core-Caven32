/**
  **************************************************************************
  * File   : time.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Init the TMR3.
  **************************************************************************
  */

#include "timer.h"
#include "led.h"

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup CortexM4_FPU_LCD_Display
  * @{
  */ 
  
extern u8 timeout;//overflow times.

/**
  * @brief  Configures the TMR3 .
  * @param  arr : The TMR3 auto-reload  value.
  *         psc : The TMR3 prescaler value.
  * @retval None
  */
void TMR3_Int_Init(u16 arr,u16 psc)
{
	TMR_TimerBaseInitType TIM_TimeBaseInitStructure;
	NVIC_InitType NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR3,ENABLE); 
	
  TIM_TimeBaseInitStructure.TMR_Period = arr; 	
	TIM_TimeBaseInitStructure.TMR_DIV=psc;  
	TIM_TimeBaseInitStructure.TMR_CounterMode=TMR_CounterDIR_Up; 
	TIM_TimeBaseInitStructure.TMR_ClockDivision=TMR_CKD_DIV1; 
	
	TMR_TimeBaseInit(TMR3,&TIM_TimeBaseInitStructure);
	
	TMR_INTConfig(TMR3,TMR_INT_Overflow,ENABLE); 
	TMR_Cmd(TMR3,ENABLE); 
	
	NVIC_InitStructure.NVIC_IRQChannel=TMR3_GLOBAL_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; 
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

/**
  * @brief  This function handles TMR3 Trigger and commutation interrupts
  *         requests.
  * @param  None
  * @retval None
  */
void TMR3_GLOBAL_IRQHandler(void)
{
	if(TMR_GetINTStatus(TMR3,TMR_INT_Overflow)==SET) 
	{
			timeout++;
	}
  TMR3->STS = 0x0;
	TMR_ClearITPendingBit(TMR3,TMR_INT_Overflow); 
}

/**
  * @}
  */ 

/**
  * @}
  */ 
  