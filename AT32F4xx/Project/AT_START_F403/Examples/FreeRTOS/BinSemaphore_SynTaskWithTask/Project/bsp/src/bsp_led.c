/**
  ******************************************************************************
  * File   : FreeRTOS/bsp/bsp_led.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Bsp Service Routines.
  ******************************************************************************
  */ 
  
/* Includes ------------------------------------------------------------------*/
#include "bsp.h"
#include "at32_board.h"

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup FreeRTOS_BinSemaphore_SynTaskWithTask
  * @{
  */ 
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/ 
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initialize led io port
  * @param  None
  * @retval None
  */
void bsp_InitLed(void)
{
	GPIO_InitType GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(LED2_GPIO_RCC_CLK, ENABLE);
	RCC_APB2PeriphClockCmd(LED3_GPIO_RCC_CLK, ENABLE);
	RCC_APB2PeriphClockCmd(LED4_GPIO_RCC_CLK, ENABLE);  
 
	bsp_LedOff(2);
	bsp_LedOff(3);
	bsp_LedOff(4);

	GPIO_InitStructure.GPIO_MaxSpeed    = GPIO_MaxSpeed_50MHz;
	GPIO_InitStructure.GPIO_Mode        = GPIO_Mode_OUT_PP;	

	GPIO_InitStructure.GPIO_Pins = LED2_PIN;
	GPIO_Init(LED2_GPIO, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pins = LED3_PIN;
	GPIO_Init(LED3_GPIO, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pins = LED4_PIN;
	GPIO_Init(LED4_GPIO, &GPIO_InitStructure);
}

/**
  * @brief  Turn on led
  * @param  _no:Led number
  * @retval None
  */
void bsp_LedOn(uint8_t _no)
{
	switch(_no)
	{
	    case 2:
		    LED2_GPIO->BRE = LED2_PIN;
		    break;
		case 3:
		    LED3_GPIO->BRE = LED3_PIN;
		    break;
		case 4:
		    LED4_GPIO->BRE = LED4_PIN;
		    break;
		default:
		    break;
	}

}

/**
  * @brief  Turn off led
  * @param  _no:Led number
  * @retval None
  */
void bsp_LedOff(uint8_t _no)
{
	switch(_no)
	{
	  case 2:
		    LED2_GPIO->BSRE = LED2_PIN;
		    break;
		case 3:
		    LED3_GPIO->BSRE = LED3_PIN;
		    break;
		case 4:
		    LED4_GPIO->BSRE = LED4_PIN;
		    break;
		default:
		    break;
	}
}

/**
  * @brief  Toggle led
  * @param  _no:Led number
  * @retval None
  */
void bsp_LedToggle(uint8_t _no)
{
	switch(_no)
	{
	    case 2:
		    LED2_GPIO->OPTDT ^= LED2_PIN;
		    break;
		case 3:
		    LED3_GPIO->OPTDT ^= LED3_PIN;
		    break;
		case 4:
		    LED4_GPIO->OPTDT ^= LED4_PIN;
		    break;
		default:
		    break;
	}

}


/**
  * @}
  */ 

/**
  * @}
  */ 

