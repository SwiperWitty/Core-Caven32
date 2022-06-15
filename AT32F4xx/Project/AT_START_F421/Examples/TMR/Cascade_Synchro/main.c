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

/** @addtogroup AT32F421_StdPeriph_Examples
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
     1/TMR1 is configured as Master Timer:
     - PWM Mode is used
     - The TMR1 Update event is used as Trigger Output  

     2/TMR3 is slave for TMR1 and Master for TMR15,
     - PWM Mode is used
     - The ITR1(TMR1) is used as input trigger 
     - Gated mode is used, so start and stop of slave counter
      are controlled by the Master trigger output signal(TMR1 update event).
      - The TMR3 Update event is used as Trigger Output. 

      3/TMR15 is slave for TMR3,
     - PWM Mode is used
     - The ITR2(TMR3) is used as input trigger
     - Gated mode is used, so start and stop of slave counter
      are controlled by the Master trigger output signal(TMR3 update event).

   *  The TIMxCLK is fixed to 120 MHz, the TMR1 counter clock is 120 MHz.

      The Master Timer TMR1 is running at TMR1 frequency :
      TMR1 frequency = (TMR1 counter clock)/ (TMR1 period + 1) = 468.75 KHz 
      and the duty cycle = TMR2_CCR1/(TMR2_ARR + 1) = 25%.

      The TMR3 is running at:
      (TMR1 frequency)/ (TMR3 period + 1) = 117.19 KHz and a duty cycle equal 
      to TMR3_CCR1/(TMR3_ARR + 1) = 25%

      The TMR15 is running at:
      (TMR3 frequency)/ (TMR15 period + 1) = 29.29 KHz and a duty cycle equal 
      to TMR4_CCR1/(TMR4_ARR + 1) = 25%  
  -------------------------------------------------------------------- */

  /* Time base configuration */
  TMR_TimeBaseStructInit(&TMR_TimeBaseStructure);
  TMR_TimeBaseStructure.TMR_Period = 255;
  TMR_TimeBaseStructure.TMR_DIV = 0;
  TMR_TimeBaseStructure.TMR_ClockDivision = 0;
  TMR_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;

  TMR_TimeBaseInit(TMR1, &TMR_TimeBaseStructure);

  TMR_TimeBaseStructure.TMR_Period = 3;
  TMR_TimeBaseInit(TMR3, &TMR_TimeBaseStructure);

  TMR_TimeBaseStructure.TMR_Period = 3;
  TMR_TimeBaseInit(TMR15, &TMR_TimeBaseStructure);

  /* Master Configuration in PWM1 Mode */
  TMR_OCStructInit(&TMR_OCInitStructure);
  TMR_OCInitStructure.TMR_OCMode = TMR_OCMode_PWM1;
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = 64;
  TMR_OCInitStructure.TMR_OCPolarity = TMR_OCPolarity_High;

  TMR_OC1Init(TMR1, &TMR_OCInitStructure);

  /* Select the Master Slave Mode */
  TMR_SelectMasterSlaveMode(TMR1, TMR_MasterSlaveMode_Enable);

  /* Master Mode selection */
  TMR_SelectOutputTrigger(TMR1, TMR_TRGOSource_Update);

  /* Slaves Configuration: PWM1 Mode */
  TMR_OCInitStructure.TMR_OCMode = TMR_OCMode_PWM1;
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = 1;

  TMR_OC1Init(TMR3, &TMR_OCInitStructure);

  TMR_OC1Init(TMR15, &TMR_OCInitStructure);

  /* Slave Mode selection: TMR3 */
  TMR_SelectSlaveMode(TMR3, TMR_SlaveMode_Gate);
  TMR_SelectInputTrigger(TMR3, TMR_TRGSEL_ITR0);

  /* Select the Master Slave Mode */
  TMR_SelectMasterSlaveMode(TMR3, TMR_MasterSlaveMode_Enable);

  /* Master Mode selection: TMR3 */
  TMR_SelectOutputTrigger(TMR3, TMR_TRGOSource_Update);

  /* Slave Mode selection: TMR15 */
  TMR_SelectSlaveMode(TMR15, TMR_SlaveMode_Gate);
  TMR_SelectInputTrigger(TMR15, TMR_TRGSEL_ITR1);
  
  /* TMR enable counter */
  TMR_Cmd(TMR3, ENABLE);
  TMR_Cmd(TMR1, ENABLE);
  TMR_Cmd(TMR15, ENABLE);
  
  /* Main Output Enable */
  TMR_CtrlPWMOutputs(TMR1, ENABLE);
  
  /* Main Output Enable */
  TMR_CtrlPWMOutputs(TMR15, ENABLE);

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
  /* TMR1, TMR3 and TMR15 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR1 | RCC_APB2PERIPH_TMR15, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR3, ENABLE);

  /* GPIOA, GPIOB clocks enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA | RCC_AHBPERIPH_GPIOB, ENABLE);
}

/**
  * @brief  Configure the GPIOD Pins.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure;

  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource8, GPIO_AF_2);
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource6, GPIO_AF_1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource2, GPIO_AF_0);

  /* GPIOA Configuration: PA8(TMR1 CH1) as alternate function push-pull */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
/* GPIOA Configuration: PA6(TMR3 CH1) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* GPIOB Configuration: PA2(TMR15 CH1) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_2;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
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

  
