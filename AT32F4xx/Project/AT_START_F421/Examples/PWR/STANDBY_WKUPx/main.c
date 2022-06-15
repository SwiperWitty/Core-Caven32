/**
  **************************************************************************
  * File   : PWR/STANDBY_WKUPx/main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  **************************************************************************
  */
 
 /* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "at32_board.h"

/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */

/** @addtogroup PWR_STANDBY_WKUPx
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

GPIO_InitType GPIO_InitStructure;

/* Private function prototypes -----------------------------------------------*/

/**
  * @brief  Configures All GPIOs to Anaog Mode.
  * @param  None
  * @retval None
  */
void GPIO_Config_ALL_AIN(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA | RCC_AHBPERIPH_GPIOB
					 | RCC_AHBPERIPH_GPIOC| RCC_AHBPERIPH_GPIOF, ENABLE);

	GPIO_InitStructure.GPIO_Pins = GPIO_Pins_All;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Pull = GPIO_Pull_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pins = GPIO_Pins_All;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pins = GPIO_Pins_All;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pins = GPIO_Pins_All;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
}

/**
  * @brief  Main Function.
  * @param  None
  * @retval None
  */
int main(void)
{
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_PWR, ENABLE);
  AT32_Board_Init();
  AT32_LEDn_OFF(LED2);
  AT32_LEDn_OFF(LED3);
  AT32_LEDn_OFF(LED4);
  if(PWR_GetFlagStatus(PWR_FLAG_SBF) != RESET)
  {
    AT32_LEDn_ON(LED2);
    PWR_ClearFlag(PWR_FLAG_SBF);
  }
  if(PWR_GetFlagStatus(PWR_FLAG_WUF) != RESET)
  {
    AT32_LEDn_ON(LED3);
    PWR_ClearFlag(PWR_FLAG_WUF);
  }
  Delay_sec(3);
  AT32_LEDn_ON(LED4);
  Delay_sec(8);
  PWR_WakeUpPinCtrl(ENABLE);
  PWR_WakeUpPinCtrl2(ENABLE);
  PWR_WakeUpPinCtrl6(ENABLE);
  PWR_WakeUpPinCtrl7(ENABLE);

  GPIO_Config_ALL_AIN();     ///<All gpio config into analog mode

  PWR_EnterSTANDBYMode();    ///<Enter standby mode
  while(1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif


/**
  * @}
  */ 

/**
  * @}
  */ 

