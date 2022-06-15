/**
  ******************************************************************************
  * File   : WWDG/WWDG_Reset/main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */

#include "at32f4xx.h"
#include "at32_board.h"
/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */

/** @addtogroup WWDG_Reset
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t EXTI_flag = 0;

/* Private function prototypes -----------------------------------------------*/
void KEY_Init(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  /* Initialize LED and KEY*/
  AT32_Board_Init();

  KEY_Init();

  /* Check if the system has resumed from WWDG reset */
  if (RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != RESET)
  {
    /* IWDGRST flag set */
    /* Turn on LED4 */
    AT32_LEDn_ON(LED4);

    /* Clear reset flags */
    RCC_ClearFlag();
  }
  else
  {
    /* IWDGRST flag is not set */
    /* Turn off LED4 */
    AT32_LEDn_OFF(LED4);
  }

  /* WWDG configuration */
  /* Enable WWDG clock */
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_WWDG, ENABLE);

  /* On other devices, WWDG clock counter = (PCLK1(72MHz)/4096)/8 = 3662 Hz (~273 us)  */
  WWDG_SetPrescaler(WWDG_Psc_8);

  /* Set Window value to 80; WWDG counter should be refreshed only when the counter
    is below 80 (and greater than 64) otherwise a reset will be generated */
  WWDG_SetWindowCounter(80);

  /* Enable WWDG and set counter value to 127, WWDG timeout = ~273 us * 64 = 17.47 ms
    In this case the refresh window is: 12.83 ms(~273 us * (127-80)) < refresh window < 17.47ms
  */
  WWDG_Enable(127);

  while (1)
  {
    /* Toggle LED3 */
    AT32_LEDn_Toggle(LED3);

    /* Insert 25ms delay */
    Delay_ms(15);

    /* Check if EXTI_flag is set */
    if(EXTI_flag)
    {
      while(1);
    }

    /* Update WWDG counter */
    WWDG_SetCounter(127);
  }
}

/**
  * @brief  Initialize the KEY as EXTI Line0.
  * @param  None
  * @retval None
  */
void KEY_Init(void)
{
  EXTI_InitType   EXTI_InitStructure;
  GPIO_InitType   GPIO_InitStructure;
  NVIC_InitType   NVIC_InitStructure;

  /* Enable GPIOA clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA, ENABLE);

  /* Configure PA.00 pin as input floating */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Connect EXTI0 Line to PA.00 pin */
  SYSCFG_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinsSource0);

  /* Configure EXTI0 line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineEnable = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI0 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
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

