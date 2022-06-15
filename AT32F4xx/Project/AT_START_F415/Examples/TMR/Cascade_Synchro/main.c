 /**
  ******************************************************************************
  * File   : TMR/Cascade_Synchro/main.c 
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
  
/** @addtogroup TMR_Cascade_Synchro
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TMR_TimerBaseInitType  TMR_TimeBaseStructure = {0};
TMR_OCInitType  TMR_OCInitStructure = {0};

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

  /* GPIO Configuration */
  GPIO_Configuration();

  /* Timers synchronisation in cascade mode ----------------------------
     1/TMR2 is configured as Master Timer:
     - PWM Mode is used
     - The TMR2 Update event is used as Trigger Output  

     2/TMR3 is slave for TMR2 and Master for TMR4,
     - PWM Mode is used
     - The ITR1(TMR2) is used as input trigger 
     - Gated mode is used, so start and stop of slave counter
      are controlled by the Master trigger output signal(TMR2 update event).
      - The TMR3 Update event is used as Trigger Output. 

      3/TMR4 is slave for TMR3,
     - PWM Mode is used
     - The ITR2(TMR3) is used as input trigger
     - Gated mode is used, so start and stop of slave counter
      are controlled by the Master trigger output signal(TMR3 update event).

   *  The TIMxCLK is fixed to 144 MHz, the TMR2 counter clock is 144 MHz.

      The Master Timer TMR2 is running at TMR2 frequency :
      TIM2 frequency = (TMR2 counter clock)/ (TMR2 period + 1) = 561.8 KHz 
      and the duty cycle = TMR2_CCR1/(TMR2_ARR + 1) = 25%.

      The TMR3 is running at:
      (TMR2 frequency)/ (TMR3 period + 1) = 140.4 KHz and a duty cycle equal 
      to TMR3_CCR1/(TMR3_ARR + 1) = 25%

      The TMR4 is running at:
      (TMR3 frequency)/ (TMR4 period + 1) = 35.16 KHz and a duty cycle equal 
      to TMR4_CCR1/(TMR4_ARR + 1) = 25%  

    o The TIMxCLK is fixed to 192 MHz, the TMR2 counter clock is 192 MHz.
      So TMR2 frequency = 750 KHz,
      TMR3 is running at 187.5 KHz,
      and TMR4 is running at 46.875 KHz
  -------------------------------------------------------------------- */

  /* Time base configuration */
  TMR_TimeBaseStructInit(&TMR_TimeBaseStructure);
  TMR_TimeBaseStructure.TMR_Period = 255;
  TMR_TimeBaseStructure.TMR_DIV = 0;
  TMR_TimeBaseStructure.TMR_ClockDivision = 0;
  TMR_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;

  TMR_TimeBaseInit(TMR2, &TMR_TimeBaseStructure);

  TMR_TimeBaseStructure.TMR_Period = 3;
  TMR_TimeBaseInit(TMR3, &TMR_TimeBaseStructure);

  TMR_TimeBaseStructure.TMR_Period = 3;
  TMR_TimeBaseInit(TMR4, &TMR_TimeBaseStructure);

  /* Master Configuration in PWM1 Mode */
  TMR_OCStructInit(&TMR_OCInitStructure);
  TMR_OCInitStructure.TMR_OCMode = TMR_OCMode_PWM1;
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = 64;
  TMR_OCInitStructure.TMR_OCPolarity = TMR_OCPolarity_High;

  TMR_OC1Init(TMR2, &TMR_OCInitStructure);

  /* Select the Master Slave Mode */
  TMR_SelectMasterSlaveMode(TMR2, TMR_MasterSlaveMode_Enable);

  /* Master Mode selection */
  TMR_SelectOutputTrigger(TMR2, TMR_TRGOSource_Update);

  /* Slaves Configuration: PWM1 Mode */
  TMR_OCInitStructure.TMR_OCMode = TMR_OCMode_PWM1;
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = 1;

  TMR_OC1Init(TMR3, &TMR_OCInitStructure);

  TMR_OC1Init(TMR4, &TMR_OCInitStructure);

  /* Slave Mode selection: TMR3 */
  TMR_SelectSlaveMode(TMR3, TMR_SlaveMode_Gate);
  TMR_SelectInputTrigger(TMR3, TMR_TRGSEL_ITR1);

  /* Select the Master Slave Mode */
  TMR_SelectMasterSlaveMode(TMR3, TMR_MasterSlaveMode_Enable);

  /* Master Mode selection: TMR3 */
  TMR_SelectOutputTrigger(TMR3, TMR_TRGOSource_Update);

  /* Slave Mode selection: TMR4 */
  TMR_SelectSlaveMode(TMR4, TMR_SlaveMode_Gate);
  TMR_SelectInputTrigger(TMR4, TMR_TRGSEL_ITR2);
  
  /* TMR enable counter */
  TMR_Cmd(TMR3, ENABLE);
  TMR_Cmd(TMR2, ENABLE);
  TMR_Cmd(TMR4, ENABLE);

  while (1)
  {
  }
}

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{  
  /* TMR2, TMR3 and TMR4 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR2 | RCC_APB1PERIPH_TMR3 |
                         RCC_APB1PERIPH_TMR4, ENABLE);

  /* GPIOA, GPIOB, GPIOC and AFIO clocks enable */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA | RCC_APB2PERIPH_GPIOB |
                         RCC_APB2PERIPH_GPIOC | RCC_APB2PERIPH_AFIO, ENABLE);
}

/**
  * @brief  Configure the GPIOD Pins.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure;

/* GPIOA Configuration: PA6(TMR3 CH1) as alternate function push-pull */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* GPIOA Configuration: PA0(TMR2 CH1) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* GPIOB Configuration: PB6(TMR4 CH1) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_6;

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

  
