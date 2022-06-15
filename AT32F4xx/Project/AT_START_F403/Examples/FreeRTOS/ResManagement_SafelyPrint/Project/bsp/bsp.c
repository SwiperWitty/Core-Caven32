/**
  ******************************************************************************
  * File   : FreeRTOS/bsp/bsp.c 
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

/** @addtogroup FreeRTOS_ResManagement_SafelyPrint
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void bsp_Init(void)
{
    /* It is recommended to assign all the priority bits to be preempt 
     * priority bits, leaving no priority bits as subpriority bits
     */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	bsp_InitUart(); 	/* initialize USART1 */
	bsp_InitLed(); 		/* initialize LED */
	bsp_InitKey();		/* initialize KEY */
  bsp_InitHardTimer();/* initialize TMR2 */
}

/**
  * @}
  */ 

/**
  * @}
  */ 
  
