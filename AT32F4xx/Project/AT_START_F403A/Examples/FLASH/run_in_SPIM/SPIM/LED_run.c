/**
  ******************************************************************************
  * File   : FLASH/run_in_SPIM/SPIM/LED_run.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file contains the function led_run used to check the LED toggle in SPIM
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "LED_run.h"
#include "at32f4xx.h"  
#include "at32_board.h"

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup FLASH_Run_In_SPIM
  * @{
  */

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  led_run
  * @param  None
  * @retval None
  */
void led_run(void)
{
  /* Toggle LED3 */
  AT32_LEDn_Toggle(LED3);
  Delay_ms(100);

}


/**
  * @}
  */ 

/**
  * @}
  */ 
  
