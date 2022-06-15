 /**
  ******************************************************************************
  * File   : IRTMR/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 

#include "at32f4xx.h"

/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */

/** @addtogroup IRTMR
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#define TIMER_DIV    1200
#define TIMER16_ARR  10000
#define TIMER17_ARR  1000

TMR_TimerBaseInitType  TMR_TMReBaseStructure;
TMR_OCInitType  TMR_OCInitStructure;

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

  /* TMR17 base configuration */
  TMR_TimeBaseStructInit(&TMR_TMReBaseStructure);
  TMR_TMReBaseStructure.TMR_Period = TIMER17_ARR;
  TMR_TMReBaseStructure.TMR_DIV = TIMER_DIV;
  TMR_TMReBaseStructure.TMR_ClockDivision = 0;
  TMR_TMReBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;
  TMR_TimeBaseInit(TMR17, &TMR_TMReBaseStructure);
  /* TMR16 base configuration */
  TMR_TMReBaseStructure.TMR_Period = TIMER16_ARR;
  TMR_TimeBaseInit(TMR16, &TMR_TMReBaseStructure);
  
  /* PWM1 Mode configuration: TMR17 Channel1 */
  TMR_OCStructInit(&TMR_OCInitStructure);
  TMR_OCInitStructure.TMR_OCMode = TMR_OCMode_PWM1;
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = TIMER17_ARR/2;
  TMR_OCInitStructure.TMR_OCPolarity = TMR_OCPolarity_High;
  TMR_OC1Init(TMR17, &TMR_OCInitStructure);
  TMR_OC1PreloadConfig(TMR17, TMR_OCPreload_Enable);

  /* PWM1 Mode configuration: TMR16 Channel1 */
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = TIMER16_ARR/2;
  TMR_OC1Init(TMR16, &TMR_OCInitStructure);
  TMR_OC1PreloadConfig(TMR16, TMR_OCPreload_Enable);
  
  /* Config IRTMR output */
  SYSCFG_IRTMRConfig(SYSCFG_IRTMR_Mode_TIM16,SYSCFG_CFGR1_Pol_IRTMR_HIGH);
  
  /* TMR16/17 enable counter */
  TMR_CtrlPWMOutputs(TMR16, ENABLE);
  TMR_CtrlPWMOutputs(TMR17, ENABLE);
  TMR_Cmd(TMR16, ENABLE);
  TMR_Cmd(TMR17, ENABLE);

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
  /* SYSCFG clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_SYSCFGCOMP, ENABLE);
  /* TMR16/17 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR16|RCC_APB2PERIPH_TMR17, ENABLE);
  /* GPIOA/GPIOB clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA|RCC_AHBPERIPH_GPIOB , ENABLE);
}

/**
  * @brief  Configure the TMR16/17/IRTMR Ouput Channels.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure;
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource6, GPIO_AF_5);
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource7, GPIO_AF_5);

  /* GPIOA Configuration:TMR16 Channel1,TMR17 Channel1  */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_6|GPIO_Pins_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
	GPIO_InitStructure.GPIO_OutType = GPIO_OutType_PP;
	GPIO_InitStructure.GPIO_Pull = GPIO_Pull_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOB, GPIO_PinsSource9, GPIO_AF_0);
  /* GPIOB Configuration:IRTMR Output */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_9;
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

