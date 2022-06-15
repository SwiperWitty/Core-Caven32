 /**
  ******************************************************************************
  * File   : TMR/OCActive/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */


/** @addtogroup TMR_OCActive
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

  /* Configure the GPIO ports */
  GPIO_Configuration();

  /* ---------------------------------------------------------------
    TMR3 Configuration: 
    TMR3CLK = SystemCoreClock / 4,
    The objective is to get TMR3 counter clock at 1 KHz:
     - Prescaler = (TIM3CLK / TMR3 counter clock) - 1
    And generate 4 signals with 4 different delays:
    TMR3_CH1 delay = CCR1_Val/TMR3 counter clock = 1000 ms
    TMR3_CH2 delay = CCR2_Val/TMR3 counter clock = 500 ms
    TMR3_CH3 delay = CCR3_Val/TMR3 counter clock = 250 ms
    TMR3_CH4 delay = CCR4_Val/TMR3 counter clock = 125 ms
  --------------------------------------------------------------- */
  /*Compute the prescaler value */
  PrescalerValue = (uint16_t) (SystemCoreClock / 4000) - 1;
  /* Time base configuration */
  TMR_TimeBaseStructInit(&TMR_TimeBaseStructure);  
  TMR_TimeBaseStructure.TMR_Period = 65535;
  TMR_TimeBaseStructure.TMR_DIV = PrescalerValue;
  TMR_TimeBaseStructure.TMR_ClockDivision = 0;
  TMR_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;

  TMR_TimeBaseInit(TMR3, &TMR_TimeBaseStructure);

  /* Output Compare Active Mode configuration: Channel1 */
  TMR_OCStructInit(&TMR_OCInitStructure);
  TMR_OCInitStructure.TMR_OCMode = TMR_OCMode_Active;
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = CCR1_Val;
  TMR_OCInitStructure.TMR_OCPolarity = TMR_OCPolarity_High;
  TMR_OC1Init(TMR3, &TMR_OCInitStructure);

  TMR_OC1PreloadConfig(TMR3, TMR_OCPreload_Disable);

  /* Output Compare Active Mode configuration: Channel2 */
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = CCR2_Val;

  TMR_OC2Init(TMR3, &TMR_OCInitStructure);

  TMR_OC2PreloadConfig(TMR3, TMR_OCPreload_Disable);

  /* Output Compare Active Mode configuration: Channel3 */
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = CCR3_Val;

  TMR_OC3Init(TMR3, &TMR_OCInitStructure);

  TMR_OC3PreloadConfig(TMR3, TMR_OCPreload_Disable);

  /* Output Compare Active Mode configuration: Channel4 */
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = CCR4_Val;

  TMR_OC4Init(TMR3, &TMR_OCInitStructure);

  TMR_OC4PreloadConfig(TMR3, TMR_OCPreload_Disable);

  TMR_ARPreloadConfig(TMR3, ENABLE);

  /* Set PC.06 pin */
  GPIO_SetBits(GPIOC, GPIO_Pins_6);

  /* TMR3 enable counter */
  TMR_Cmd(TMR3, ENABLE);

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

  /* TMR3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR3, ENABLE);

  /* GPIOA and GPIOC clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA | RCC_APB2PERIPH_GPIOD | RCC_APB2PERIPH_GPIOB |
                         RCC_APB2PERIPH_GPIOC | RCC_APB2PERIPH_AFIO, ENABLE);
}

/**
  * @brief  Configure the TMR3 and the GPIOE Pins.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure;

  /* GPIOA Configuration:TMR3 Channel1, 2, 3 and 4 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_6 | GPIO_Pins_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_0 | GPIO_Pins_1;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* GPIOC Configuration: Pin6 an Output push-pull */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;

  GPIO_Init(GPIOC, &GPIO_InitStructure);
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
  



