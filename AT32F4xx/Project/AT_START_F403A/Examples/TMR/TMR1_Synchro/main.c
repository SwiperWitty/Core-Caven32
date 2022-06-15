 /**
  ******************************************************************************
  * File   : TMR/TMR1_Synchro/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */


#include "at32f4xx.h"

/** @addtogroup AT32F403A_StdPeriph_Examples
  * @{
  */

/** @addtogroup TMR_TMR1_Synchro
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TMR_TimerBaseInitType  TMR_TMReBaseStructure;
TMR_OCInitType  TMR_OCInitStructure;
TMR_BRKDTInitType TMR_BDTRInitStructure;

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
  /*!< At this stage the microcontroller clock setting is already configured,
       this is done through SystemInit() function which is called from startup
       file (startup_at32f403_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_at32f4xx.c file
     */

  /* System Clocks Configuration */
  RCC_Configuration();

  /* GPIO Configuration */
  GPIO_Configuration();

  /* TMR1 and TMRers(TMR3 and TMR4) synchronisation in parallel mode -----------
     1/TMR1 is configured as Master TMRer:
     - PWM Mode is used
     - The TMR1 Update event is used as Trigger Output

     2/TMR3 and TMR4 are slaves for TMR1,
     - PWM Mode is used
     - The ITR0(TMR1) is used as input trigger for both slaves
     - Gated mode is used, so starts and stops of slaves counters
       are controlled by the Master trigger output signal(update event).

    The Master TMRer TMR1 is running at:
    TMR1 frequency = TMR1 counter clock / (TMR1_Period + 1) = 750 KHz
    and the duty cycle is equal to: TMR1_CCR1/(TMR1_ARR + 1) = 50%

    The TMR3 is running at:
    (TMR1 frequency)/ ((TMR3 period +1)* (Repetition_Counter+1)) = 49.95 KHz and
    a duty cycle equal to TMR3_CCR1/(TMR3_ARR + 1) = 33.3%

    The TMR4 is running at:
    (TMR1 frequency)/ ((TMR4 period +1)* (Repetition_Counter+1)) = 74.96 KHz and
    a duty cycle equal to TMR4_CCR1/(TMR4_ARR + 1) = 50%
  --------------------------------------------------------------------------- */

  /* TMR3 Peripheral Configuration ----------------------------------------*/
  /* TMR3 Slave Configuration: PWM1 Mode */
  TMR_TimeBaseStructInit(&TMR_TMReBaseStructure);
  TMR_TMReBaseStructure.TMR_Period = 2;
  TMR_TMReBaseStructure.TMR_DIV = 0;
  TMR_TMReBaseStructure.TMR_ClockDivision = 0;
  TMR_TMReBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;

  TMR_TimeBaseInit(TMR3, &TMR_TMReBaseStructure);

  TMR_OCStructInit(&TMR_OCInitStructure);
  TMR_OCInitStructure.TMR_OCMode = TMR_OCMode_PWM1;
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = 1;

  TMR_OC1Init(TMR3, &TMR_OCInitStructure);

  /* Slave Mode selection: TMR3 */
  TMR_SelectSlaveMode(TMR3, TMR_SlaveMode_Gate);
  TMR_SelectInputTrigger(TMR3, TMR_TRGSEL_ITR0);

  /* TMR4 Peripheral Configuration ----------------------------------------*/
  /* TMR4 Slave Configuration: PWM1 Mode */
  TMR_TMReBaseStructure.TMR_Period = 1;
  TMR_TMReBaseStructure.TMR_DIV = 0;
  TMR_TMReBaseStructure.TMR_ClockDivision = 0;
  TMR_TMReBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;

  TMR_TimeBaseInit(TMR4, &TMR_TMReBaseStructure);

  TMR_OCInitStructure.TMR_OCMode = TMR_OCMode_PWM1;
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = 1;

  TMR_OC1Init(TMR4, &TMR_OCInitStructure);

  /* Slave Mode selection: TMR4 */
  TMR_SelectSlaveMode(TMR4, TMR_SlaveMode_Gate);
  TMR_SelectInputTrigger(TMR4, TMR_TRGSEL_ITR0);

  /* TMR1 Peripheral Configuration ----------------------------------------*/
  /* TMRe Base configuration */
  TMR_TMReBaseStructure.TMR_DIV = 0;
  TMR_TMReBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;
  TMR_TMReBaseStructure.TMR_Period = 319;
  TMR_TMReBaseStructure.TMR_ClockDivision = 0;
  TMR_TMReBaseStructure.TMR_RepetitionCounter = 4;

  TMR_TimeBaseInit(TMR1, &TMR_TMReBaseStructure);

  /* Channel 1 Configuration in PWM mode */
  TMR_OCInitStructure.TMR_OCMode = TMR_OCMode_PWM2;
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_OutputNState = TMR_OutputNState_Enable;
  TMR_OCInitStructure.TMR_Pulse = 127;
  TMR_OCInitStructure.TMR_OCPolarity = TMR_OCPolarity_Low;
  TMR_OCInitStructure.TMR_OCNPolarity = TMR_OCNPolarity_Low;
  TMR_OCInitStructure.TMR_OCIdleState = TMR_OCIdleState_Set;
  TMR_OCInitStructure.TMR_OCNIdleState = TMR_OCIdleState_Reset;

  TMR_OC1Init(TMR1, &TMR_OCInitStructure);

  /* Automatic Output enable, Break, dead TMRe and lock configuration*/
  TMR_BRKDTStructInit(&TMR_BDTRInitStructure);
  TMR_BDTRInitStructure.TMR_OSIMRState = TMR_OSIMRState_Enable;
  TMR_BDTRInitStructure.TMR_OSIMIState = TMR_OSIMIState_Enable;
  TMR_BDTRInitStructure.TMR_LOCKgrade = TMR_LOCKgrade_1;
  TMR_BDTRInitStructure.TMR_DeadTime = 5;
  TMR_BDTRInitStructure.TMR_Break = TMR_Break_Disable;
  TMR_BDTRInitStructure.TMR_BreakPolarity = TMR_BreakPolarity_High;
  TMR_BDTRInitStructure.TMR_AutomaticOutput = TMR_AutomaticOutput_Disable;

  TMR_BRKDTConfig(TMR1, &TMR_BDTRInitStructure);

  /* Master Mode selection */
  TMR_SelectOutputTrigger(TMR1, TMR_TRGOSource_Update);

  /* Select the Master Slave Mode */
  TMR_SelectMasterSlaveMode(TMR1, TMR_MasterSlaveMode_Enable);

  /* TMR1 counter enable */
  TMR_Cmd(TMR1, ENABLE);

  /* TMR enable counter */
  TMR_Cmd(TMR3, ENABLE);
  TMR_Cmd(TMR4, ENABLE);

  /* Main Output Enable */
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
  /* TMR1, GPIOA and GPIOB clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR1 | RCC_APB2PERIPH_GPIOA | RCC_APB2PERIPH_GPIOE |
                         RCC_APB2PERIPH_GPIOC | RCC_APB2PERIPH_GPIOB | RCC_APB2PERIPH_AFIO, ENABLE);

  /* TMR3 and TMR4 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR3 | RCC_APB1PERIPH_TMR4, ENABLE);
}

/**
  * @brief  Configures TMR1, TMR3 and TMR4 Pins.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure;

  /* GPIOA Configuration: TMR1 Channel1 and TMR3 Channel1 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_6 | GPIO_Pins_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* GPIOB Configuration: TMR4 Channel1 as alternate function push-pull */
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
  
  
