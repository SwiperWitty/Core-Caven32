 /**
  ******************************************************************************
  * File   : TMR/TMR15_ComplementarySignals/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 


#include "at32f4xx.h"

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup TMR_TMR15_ComplementarySignals
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TMR_TimerBaseInitType  TMR_TMReBaseStructure;
TMR_OCInitType  TMR_OCInitStructure;
TMR_BRKDTInitType TMR_BDTRInitStructure;
uint16_t CCR1_Val = 32767;

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

  /* -----------------------------------------------------------------------
  TMR15 Configuration to:

  1/ Generate a complementary PWM signals with 50% duty cycles:
  TMR15CLK = 192 MHz, Prescaler = 0, TMR15 counter clock = 192 MHz
  TMR15 frequency = TMR15CLK/(TMR15_Period + 1) = 2.928 kHz

  TMR15 Channel1 duty cycle = TMR15->CCR1 / TMR15_Period = 50%
  TMR15 Channel1N duty cycle = (TMR15_Period - TMR15_CCR1) / (TMR15_Period + 1) = 50%

  2/ Insert a dead TMRe equal to 200 ns
  3/ Configure the break feature, active at High level, and using the automatic
     output enable feature
  4/ Use the Locking parameters level1.
  ----------------------------------------------------------------------- */

  /* TMRe Base configuration */
  TMR_TimeBaseStructInit(&TMR_TMReBaseStructure);
  TMR_TMReBaseStructure.TMR_DIV = 0;
  TMR_TMReBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;
  TMR_TMReBaseStructure.TMR_Period = 65535;
  TMR_TMReBaseStructure.TMR_ClockDivision = 0;
  TMR_TMReBaseStructure.TMR_RepetitionCounter = 0;

  TMR_TimeBaseInit(TMR15, &TMR_TMReBaseStructure);

  /* Channel 1 Configuration in PWM mode */
  TMR_OCStructInit(&TMR_OCInitStructure);
  TMR_OCInitStructure.TMR_OCMode = TMR_OCMode_PWM2;
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_OutputNState = TMR_OutputNState_Enable;
  TMR_OCInitStructure.TMR_Pulse = CCR1_Val;
  TMR_OCInitStructure.TMR_OCPolarity = TMR_OCPolarity_Low;
  TMR_OCInitStructure.TMR_OCNPolarity = TMR_OCNPolarity_Low;
  TMR_OCInitStructure.TMR_OCIdleState = TMR_OCIdleState_Set;
  TMR_OCInitStructure.TMR_OCNIdleState = TMR_OCIdleState_Reset;

  TMR_OC1Init(TMR15, &TMR_OCInitStructure);

  /* Automatic Output enable, Break, dead TMRe and lock configuration*/
  TMR_BRKDTStructInit(&TMR_BDTRInitStructure);
  TMR_BDTRInitStructure.TMR_OSIMRState = TMR_OSIMRState_Enable;
  TMR_BDTRInitStructure.TMR_OSIMIState = TMR_OCIdleState_Set;
  TMR_BDTRInitStructure.TMR_LOCKgrade = TMR_LOCKgrade_1;
  TMR_BDTRInitStructure.TMR_DeadTime = 39;
  TMR_BDTRInitStructure.TMR_Break = TMR_Break_Enable;
  TMR_BDTRInitStructure.TMR_BreakPolarity = TMR_BreakPolarity_High;
  TMR_BDTRInitStructure.TMR_AutomaticOutput = TMR_AutomaticOutput_Enable;

  TMR_BRKDTConfig(TMR15, &TMR_BDTRInitStructure);

  /* TMR15 counter enable */
  TMR_Cmd(TMR15, ENABLE);

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
  /* TMR15, GPIOF and AFIO clocks enable */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR15 | RCC_APB2PERIPH_GPIOF | RCC_APB2PERIPH_GPIOB , ENABLE);
}

/**
  * @brief  Configure the TMR1 Pins.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure;

  /* GPIOA Configuration: Channel 1 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_0 | GPIO_Pins_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz ;
  GPIO_Init(GPIOF, &GPIO_InitStructure);

  /* GPIOA Configuration: BKIN pin */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PD;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
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
  

