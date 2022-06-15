 /**
  ******************************************************************************
  * File   : TMR/7PWM_OUTPUT/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 
/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"

/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */

/** @addtogroup TMR_7PWM_Output
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TMR_TimerBaseInitType  TMR_TimeBaseStructure = {0};
TMR_OCInitType  TMR_OCInitStructure = {0};
uint16_t TimerPeriod = 0;
uint16_t Channel1Pulse = 0, Channel2Pulse = 0, Channel3Pulse = 0, Channel4Pulse = 0;

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
int main(void)
{    
  /* System Clocks Configuration */
  RCC_Configuration();

  /* GPIO Configuration */
  GPIO_Configuration();
    
  /* TMR1 Configuration ---------------------------------------------------
   Generate 7 PWM signals with 4 different duty cycles:
   TMR1CLK = SystemCoreClock, Prescaler = 0, TMR1 counter clock = SystemCoreClock
   SystemCoreClock is set to 72 MHz for Low-density, Medium-density, High-density
   and Connectivity line devices and to 24 MHz for Low-Density Value line and
   Medium-Density Value line devices
   
   The objective is to generate 7 PWM signal at 17.57 KHz:
     - TIM1_Period = (SystemCoreClock / 17570) - 1
   The channel 1 and channel 1N duty cycle is set to 50%
   The channel 2 and channel 2N duty cycle is set to 37.5%
   The channel 3 and channel 3N duty cycle is set to 25%
   The channel 4 duty cycle is set to 12.5%
   The Timer pulse is calculated as follows:
     - ChannelxPulse = DutyCycle * (TIM1_Period - 1) / 100
  ----------------------------------------------------------------------- */
  /* Compute the value to be set in ARR regiter to generate signal frequency at 17.57 Khz */
  TimerPeriod = (SystemCoreClock / 17570 ) - 1;
  /* Compute CCR1 value to generate a duty cycle at 50% for channel 1 and 1N */
  Channel1Pulse = (uint16_t) (((uint32_t) 5 * (TimerPeriod - 1)) / 10);
  /* Compute CCR2 value to generate a duty cycle at 37.5%  for channel 2 and 2N */
  Channel2Pulse = (uint16_t) (((uint32_t) 375 * (TimerPeriod - 1)) / 1000);
  /* Compute CCR3 value to generate a duty cycle at 25%  for channel 3 and 3N */
  Channel3Pulse = (uint16_t) (((uint32_t) 25 * (TimerPeriod - 1)) / 100);
  /* Compute CCR4 value to generate a duty cycle at 12.5%  for channel 4 */
  Channel4Pulse = (uint16_t) (((uint32_t) 125 * (TimerPeriod- 1)) / 1000);

  /* Time Base configuration */
  TMR_TimeBaseStructInit(&TMR_TimeBaseStructure);
  TMR_TimeBaseStructure.TMR_DIV = 0;
  TMR_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;
  TMR_TimeBaseStructure.TMR_Period = TimerPeriod;
  TMR_TimeBaseStructure.TMR_ClockDivision = 0;
  TMR_TimeBaseStructure.TMR_RepetitionCounter = 0;

  TMR_TimeBaseInit(TMR1, &TMR_TimeBaseStructure);

  /* Channel 1, 2,3 and 4 Configuration in PWM mode */
  TMR_OCStructInit(&TMR_OCInitStructure);
  TMR_OCInitStructure.TMR_OCMode = TMR_OCMode_PWM2;
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_OutputNState = TMR_OutputNState_Enable;
  TMR_OCInitStructure.TMR_Pulse = Channel1Pulse;
  TMR_OCInitStructure.TMR_OCPolarity = TMR_OCPolarity_Low;
  TMR_OCInitStructure.TMR_OCNPolarity = TMR_OCNPolarity_High;
  TMR_OCInitStructure.TMR_OCIdleState = TMR_OCIdleState_Set;
  TMR_OCInitStructure.TMR_OCNIdleState = TMR_OCIdleState_Reset;

  TMR_OC1Init(TMR1, &TMR_OCInitStructure);

  TMR_OCInitStructure.TMR_Pulse = Channel2Pulse;
  TMR_OC2Init(TMR1, &TMR_OCInitStructure);

  TMR_OCInitStructure.TMR_Pulse = Channel3Pulse;
  TMR_OC3Init(TMR1, &TMR_OCInitStructure);

  TMR_OCInitStructure.TMR_Pulse = Channel4Pulse;
  TMR_OC4Init(TMR1, &TMR_OCInitStructure);

  /* TMR1 counter enable */
  TMR_Cmd(TMR1, ENABLE);

  /* TMR1 Main Output Enable */
  TMR_CtrlPWMOutputs(TMR1, ENABLE);

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
  /* TMR1, GPIOA, GPIOB, GPIOE and AFIO clocks enable */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR1, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA | RCC_AHBPERIPH_GPIOB, ENABLE);
}

/**
  * @brief  Configure the TMR1 Pins.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure = {0};

  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource8, GPIO_AF_2);
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource9, GPIO_AF_2);
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource10, GPIO_AF_2);
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource11, GPIO_AF_2);
  GPIO_PinAFConfig(GPIOB, GPIO_PinsSource13, GPIO_AF_2);
  GPIO_PinAFConfig(GPIOB, GPIO_PinsSource14, GPIO_AF_2);
  GPIO_PinAFConfig(GPIOB, GPIO_PinsSource15, GPIO_AF_2);
  
  /* GPIOA Configuration: Channel 1, 2 and 3 as alternate function push-pull */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_8 | GPIO_Pins_9 | GPIO_Pins_10 | GPIO_Pins_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* GPIOB Configuration: Channel 1N, 2N and 3N as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_13 | GPIO_Pins_14 | GPIO_Pins_15;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

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
  
