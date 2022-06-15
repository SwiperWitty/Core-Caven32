 /**
  ******************************************************************************
  * File   : TMR/OCInactive/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 
  

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"

/** @addtogroup AT32F415_StdPeriph_Examples
  * @{
  */

/** @addtogroup TMR_OCInactive
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TMR_TimerBaseInitType  TMR_TimeBaseStructure;
TMR_OCInitType  TMR_OCInitStructure;
uint16_t CCR1_Val = 1000;
uint16_t CCR2_Val = 500;
uint16_t CCR3_Val = 250;
uint16_t CCR4_Val = 125;
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
  /* System Clocks Configuration */
  RCC_Configuration();

  /* NVIC Configuration */
  NVIC_Configuration();

  /* GPIO Configuration */
  GPIO_Configuration();

  /* ---------------------------------------------------------------
    TMR2 Configuration: 
    TMR2CLK = SystemCoreClock / 4,
    The objective is to get TMR2 counter clock at 1 KHz:
     - Prescaler = (TIM2CLK / TMR2 counter clock) - 1
    And generate 4 signals with 4 different delays:
    TMR2_CH1 delay = CCR1_Val/TMR2 counter clock = 1000 ms
    TMR2_CH2 delay = CCR2_Val/TMR2 counter clock = 500 ms
    TMR2_CH3 delay = CCR3_Val/TMR2 counter clock = 250 ms
    TMR2_CH4 delay = CCR4_Val/TMR2 counter clock = 125 ms
  --------------------------------------------------------------- */
  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) (SystemCoreClock / 4000) - 1;
  /* Time base configuration */
  TMR_TimeBaseStructInit(&TMR_TimeBaseStructure);
  TMR_TimeBaseStructure.TMR_Period = 65535;
  TMR_TimeBaseStructure.TMR_DIV = PrescalerValue;
  TMR_TimeBaseStructure.TMR_ClockDivision = 0;
  TMR_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;

  TMR_TimeBaseInit(TMR2, &TMR_TimeBaseStructure);

  /* Output Compare Active Mode configuration: Channel1 */
  TMR_OCStructInit(&TMR_OCInitStructure);
  TMR_OCInitStructure.TMR_OCMode = TMR_OCMode_Inactive;
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = CCR1_Val;
  TMR_OCInitStructure.TMR_OCPolarity = TMR_OCPolarity_High;

  TMR_OC1Init(TMR2, &TMR_OCInitStructure);

  TMR_OC1PreloadConfig(TMR2, TMR_OCPreload_Disable);

  /* Output Compare Active Mode configuration: Channel2 */
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = CCR2_Val;

  TMR_OC2Init(TMR2, &TMR_OCInitStructure);

  TMR_OC2PreloadConfig(TMR2, TMR_OCPreload_Disable);

  /* Output Compare Active Mode configuration: Channel3 */
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = CCR3_Val;

  TMR_OC3Init(TMR2, &TMR_OCInitStructure);

  TMR_OC3PreloadConfig(TMR2, TMR_OCPreload_Disable);

  /* Output Compare Active Mode configuration: Channel4 */
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = CCR4_Val;

  TMR_OC4Init(TMR2, &TMR_OCInitStructure);

  TMR_OC4PreloadConfig(TMR2, TMR_OCPreload_Disable);

  TMR_ARPreloadConfig(TMR2, ENABLE);

  /* TMR INT enable */
  TMR_INTConfig(TMR2, TMR_INT_CC1 | TMR_INT_CC2 | TMR_INT_CC3 | TMR_INT_CC4, ENABLE);

  /* Set PC.06, PC.07, PC.08 and PC.09 pins */
  GPIO_SetBits(GPIOC, GPIO_Pins_6 | GPIO_Pins_7 | GPIO_Pins_8 | GPIO_Pins_9);

  /* TMR2 enable counter */
  TMR_Cmd(TMR2, ENABLE);

  while (1)
  {}
}

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{
  /* PCLK1 = HCLK/4 */
  RCC_APB1CLKConfig(RCC_AHBCLK_Div8);

  /* TMR2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR2, ENABLE);

  /* GPIOC clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOC, ENABLE);
}

/**
  * @brief  Configure the GPIOD Pins.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure;

  /* GPIOC Configuration: Pin6, 7, 8 and 9 as output push-pull */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_6 | GPIO_Pins_7 | GPIO_Pins_8 | GPIO_Pins_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;

  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/**
  * @brief  Configure the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
  NVIC_InitType NVIC_InitStructure;

  /* Enable the TMR2 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TMR2_GLOBAL_IRQn;
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

  
