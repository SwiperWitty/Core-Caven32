/**
  ******************************************************************************
  * File   : RTC/Calendar//main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 
	
/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "at32_board.h"
#include "rtc.h"
#include <stdio.h>

/** @addtogroup AT32F403A_StdPeriph_Examples
  * @{
  */

/** @addtogroup RTC_Calendar
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
char const weekday_table[7][10]={ "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

/* Private function prototypes -----------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	/* USART configuration */
	UART_Print_Init(115200);

	/* RTC configuration */
	RTC_Init();

	/* print initial message */
	printf("\r\n\nInitial ok");

	while(1)
	{
		if(RTC_GetFlagStatus(RTC_FLAG_PACE) != RESET)	
		{
			/* Update current time */
			RTC_Get();
			
			/* print the current time */
			printf("\r\n");
			printf("%d/%d/%d ", calendar.w_year, calendar.w_month, calendar.w_date);
			printf("%02d:%02d:%02d %s", calendar.hour, calendar.min, calendar.sec, weekday_table[calendar.week]);
		
			/* Clear the RTC Second flag */
			RTC_ClearFlag(RTC_FLAG_PACE);
			
			/* Wait until last write operation on RTC registers has finished */
			RTC_WaitForLastTask();
		}
	}
}

/**
  * @}
  */ 

/**
  * @}
  */ 

