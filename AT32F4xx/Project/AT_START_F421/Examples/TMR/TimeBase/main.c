 /**
  ******************************************************************************
  * File   : TMR/TimeBase/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 


#include "at32f4xx.h"

/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */

/** @addtogroup TMR_TimeBase
  * @{
  */
  
TMR_TimerBaseInitType  TMR_TMReBaseStructure;
TMR_OCInitType  TMR_OCInitStructure;
__IO uint16_t CCR1_Val = 40961;
__IO uint16_t CCR2_Val = 27309;
__IO uint16_t CCR3_Val = 13654;
__IO uint16_t CCR4_Val = 6826;
uint16_t PrescalerValue = 0;

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured,
       this is done through SystemInit() function which is called from startup
       file (startup_at32f413_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_at32f4xx.c file
     */

  /* System Clocks Configuration */
  RCC_Configuration();

  /* NVIC Configuration */
  NVIC_Configuration();

  /* GPIO Configuration */
  GPIO_Configuration();

  /* ---------------------------------------------------------------
    TMR3 Configuration: Output Compare TMRing Mode:
    TMR3 counter clock at 12 MHz
    CC1 update rate = TMR3 counter clock / CCR1_Val / 2 = 146.48 Hz
    CC2 update rate = TMR3 counter clock / CCR2_Val / 2 = 219.7 Hz
    CC3 update rate = TMR3 counter clock / CCR3_Val / 2 = 439.4 Hz
    CC4 update rate = TMR3 counter clock / CCR4_Val / 2 = 878.9 Hz
  --------------------------------------------------------------- */

  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) (SystemCoreClock / 12000000) - 1;

  /* TMRe base configuration */
  TMR_TimeBaseStructInit(&TMR_TMReBaseStructure);
  TMR_TMReBaseStructure.TMR_Period = 65535;
  TMR_TMReBaseStructure.TMR_DIV = 0;
  TMR_TMReBaseStructure.TMR_ClockDivision = 0;
  TMR_TMReBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;

  TMR_TimeBaseInit(TMR3, &TMR_TMReBaseStructure);

  /* Prescaler configuration */
  TMR_DIVConfig(TMR3, PrescalerValue, TMR_DIVReloadMode_Immediate);

  /* Output Compare TMRing Mode configuration: Channel1 */
  TMR_OCStructInit(&TMR_OCInitStructure);
  TMR_OCInitStructure.TMR_OCMode = TMR_OCMode_Timing;
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = CCR1_Val;
  TMR_OCInitStructure.TMR_OCPolarity = TMR_OCPolarity_High;

  TMR_OC1Init(TMR3, &TMR_OCInitStructure);

  TMR_OC1PreloadConfig(TMR3, TMR_OCPreload_Disable);

  /* Output Compare TMRing Mode configuration: Channel2 */
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = CCR2_Val;

  TMR_OC2Init(TMR3, &TMR_OCInitStructure);

  TMR_OC2PreloadConfig(TMR3, TMR_OCPreload_Disable);

  /* Output Compare TMRing Mode configuration: Channel3 */
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = CCR3_Val;

  TMR_OC3Init(TMR3, &TMR_OCInitStructure);

  TMR_OC3PreloadConfig(TMR3, TMR_OCPreload_Disable);

  /* Output Compare TMRing Mode configuration: Channel4 */
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = CCR4_Val;

  TMR_OC4Init(TMR3, &TMR_OCInitStructure);

  TMR_OC4PreloadConfig(TMR3, TMR_OCPreload_Disable);

  /* TMR IT enable */
  TMR_INTConfig(TMR3, TMR_INT_CC1 | TMR_INT_CC2 | TMR_INT_CC3 | TMR_INT_CC4, ENABLE);

  /* TMR3 enable counter */
  TMR_Cmd(TMR3, ENABLE);

  while (1);
}

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{
  /* TMR3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR3, ENABLE);

  /* GPIOC clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOB, ENABLE);
}

/**
  * @brief  Configure the GPIO Pins.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure;

  /* GPIOC Configuration:Pin6, 7, 8 and 9 as alternate function push-pull */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_6 | GPIO_Pins_7 | GPIO_Pins_8 | GPIO_Pins_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;

  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
  * @brief  Configure the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
  NVIC_InitType NVIC_InitStructure;

  /* Enable the TMR3 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TMR3_GLOBAL_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
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


