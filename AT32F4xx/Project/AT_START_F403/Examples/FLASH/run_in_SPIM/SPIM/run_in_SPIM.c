/**
  ******************************************************************************
  * File   : FLASH/run_in_SPIM/SPIM/run_in_SPIM.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file contains the function SPIM_run used to check the LED toggle in SPIM
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "run_in_SPIM.h"
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
  * @brief  Check the LED toggle in SPIM
  * @param  None
  * @retval None
  */
void SPIM_run(void)
{
  /* Init LED */
  AT32_Board_Init(); 
  while(1)
  {
    /* Toggle LED2 */
    AT32_LEDn_Toggle(LED2);
    Delay_ms(100);
    led_run();
  }
}


/**
  * @}
  */ 

/**
  * @}
  */ 
  
