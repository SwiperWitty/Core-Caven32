 /**
  **************************************************************************
  * File   : PWR/STOP/main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  **************************************************************************
  */
 
 /* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "at32f4xx.h"
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
GPIO_InitType GPIO_InitStructure;
EXTI_InitType EXTI_InitStructure;
NVIC_InitType NVIC_InitStructure = {0};
ErrorStatus HSEStartUpStatus;

/* Private function prototypes -----------------------------------------------*/
void SYSCLKConfig_STOP(void);
void EXTI_Configuration(void);
void RTC_Configuration(void);
void NVIC_Configuration(void);

/* Private functions ---------------------------------------------------------*/

/**
 *	@brief  main function
 *	@param  None
 *	@retval None
 */
int main(void)
{
	uint32_t i;
	GPIO_StructInit(&GPIO_InitStructure);
	EXTI_StructInit(&EXTI_InitStructure);      
	AT32_Board_Init();      ///<Initialize LEDs
	AT32_LEDn_ON(LED2);
	AT32_LEDn_ON(LED3);
	AT32_LEDn_ON(LED4);
	
	RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_PWR | RCC_APB1PERIPH_BKP, ENABLE); ///<Enable PWR and BKP clock
	EXTI_Configuration();   ///<Configure RTC alarm interrupt
	RTC_Configuration();    ///<Configure RTC clock source and prescaler
	NVIC_Configuration();   ///<NVIC configuration
  while (1)
	{
		RTC_ClearFlag(RTC_FLAG_PACE);
		while(RTC_GetFlagStatus(RTC_FLAG_PACE) == RESET);///<Wait till RTC Second event occurs
		RTC_SetAlarmValue(RTC_GetCounter()+ 3);          ///<Alarm in 3 second
		RTC_WaitForLastTask();                           ///<Wait until last write operation on RTC registers has finished
    SysTick->CTRL &= 0xFFFFFFFE;
		AT32_LEDn_OFF(LED2);                             ///<Turn off LED2

		PWR_EnterSTOPMode(PWR_STOPEntry_WFI);///Request to enter STOP mode
    SysTick->CTRL |= 0x1;
		SYSCLKConfig_STOP();                             ///<Configures system clock after wake-up from STOP
		AT32_LEDn_ON(LED2);                              ///<Turn on LED2
		for(i=0;i<500000;i++);
	}
}

/**
  * @brief  Configures system clock after wake-up from STOP: enable HSE, PLL and select PLL as system clock source.
  * @param  None
  * @retval None
  */
void SYSCLKConfig_STOP(void)
{
  uint32_t i;
  RCC_HSEConfig(RCC_HSE_ENABLE);   ///<Enable HSE
  for(i=0;i<20000;i++);
  HSEStartUpStatus = RCC_WaitForHSEStable();
  if(HSEStartUpStatus == SUCCESS)
  {
    RCC_PLLCmd(ENABLE);   ///<Enable PLL
    while(RCC_GetFlagStatus(RCC_FLAG_PLLSTBL) == RESET)  ///<Wait till PLL is ready
    {
    }
    RCC_SYSCLKConfig(RCC_SYSCLKSelction_PLL);            ///<Select PLL as system clock source
    while(RCC_GetSYSCLKSelction() != 0x08)               ///<Wait till PLL is used as system clock source
    {
    }
  }
}

/**
  * @brief  Configures EXTI Line 17(RTC Alarm) to generate an interrupt on rising edge.
  * @param  None
  * @retval None
  */
void EXTI_Configuration(void)
{
    EXTI_ClearIntPendingBit(EXTI_Line17);
    EXTI_InitStructure.EXTI_Line = EXTI_Line17;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineEnable = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

/**
  * @brief  Configures RTC clock source and prescaler.
  * @param  None
  * @retval None
  */
void RTC_Configuration(void)
{
	PWR_BackupAccessCtrl(ENABLE);   ///< Allow access to BKP Domain
	BKP_Reset();   ///<Reset Backup Domain
	#if 0	
		RCC_LSEConfig(RCC_LSE_ENABLE);   ///<Enable the LSE OSC
		while(RCC_GetFlagStatus(RCC_FLAG_LSESTBL) == RESET)  ///<Wait till LSE is ready
		{
		}
		RCC_RTCCLKConfig(RCC_RTCCLKSelection_LSE);	   ///<Select the RTC Clock Source
	#else
		RCC_LSICmd(ENABLE);   ///<Enable the LSI OSC
		while(RCC_GetFlagStatus(RCC_FLAG_LSISTBL) == RESET)  ///<Wait till LSI is ready
		{
		}
		RCC_RTCCLKConfig(RCC_RTCCLKSelection_LSI);   ///<Select the RTC Clock Source
	#endif
	RCC_RTCCLKCmd(ENABLE);   ///<Enable the RTC Clock

	/* RTC configuration -------------------------------------------------------*/
	RTC_WaitForSynchro();   ///<Wait for RTC APB registers synchronisation
	RTC_SetDIV(32767);     ///<Set the RTC time base to 1s
	RTC_WaitForLastTask();   ///<Wait until last write operation on RTC registers has finished
	RTC_INTConfig(RTC_INT_ALA, ENABLE);   ///<Enable the RTC Alarm interrupt
	RTC_WaitForLastTask();  ///<Wait until last write operation on RTC registers has finished
}

/**
  * @brief  Configures NVIC and Vector Table base location.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


/**
  * @}
  */ 

/**
  * @}
  */ 

