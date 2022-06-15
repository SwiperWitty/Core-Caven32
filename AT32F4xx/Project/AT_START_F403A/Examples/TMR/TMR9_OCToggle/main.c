 /**
  ******************************************************************************
  * File   : TMR/TMR9_OCToggle/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 


#include "at32f4xx.h"

/** @addtogroup AT32F403A_StdPeriph_Examples
  * @{
  */

/** @addtogroup TMR_TMR9_OCToggle
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TMR_TimerBaseInitType  TMR_TMReBaseStructure;
TMR_OCInitType  TMR_OCInitStructure;
__IO uint16_t CCR1Val = 32768;
__IO uint16_t CCR2Val = 16384;
uint16_t PrescalerValue = 0;

/* Private function prototypes -----------------------------------------------*/
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
       file (startup_at32f403_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_at32f4xx.c file
     */

  /* Configure TMR9 pins */
  GPIO_Configuration();

  /* NVIC Configuration */
  NVIC_Configuration();

  /* ---------------------------------------------------------------------------
    TMR9 Configuration: Output Compare Toggle Mode:
    TMR9CLK = SystemCoreClock (240MHz),
    The objective is to get TMR9 counter clock at 24 MHz:
     - Prescaler = (TMR9CLK / TMR9 counter clock) - 1
    CC1 update rate = TMR9 counter clock / CCR1Val = 732.4 Hz
    CC2 update rate = TMR9 counter clock / CCR2Val = 1464.8 Hz
  ----------------------------------------------------------------------------*/
  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) (SystemCoreClock / 24000000) - 1;

  /* TMRe base configuration */
  TMR_TimeBaseStructInit(&TMR_TMReBaseStructure);
  TMR_TMReBaseStructure.TMR_Period = 65535;
  TMR_TMReBaseStructure.TMR_DIV = PrescalerValue;
  TMR_TMReBaseStructure.TMR_ClockDivision = 0;
  TMR_TMReBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;

  TMR_TimeBaseInit(TMR9, &TMR_TMReBaseStructure);

  /* Output Compare Toggle Mode configuration: Channel1 */
  TMR_OCStructInit(&TMR_OCInitStructure);
  TMR_OCInitStructure.TMR_OCMode = TMR_OCMode_Toggle;
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = CCR1Val;
  TMR_OCInitStructure.TMR_OCPolarity = TMR_OCPolarity_Low;
  TMR_OC1Init(TMR9, &TMR_OCInitStructure);

  TMR_OC1PreloadConfig(TMR9, TMR_OCPreload_Disable);

  /* Output Compare Toggle Mode configuration: Channel2 */
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = CCR2Val;

  TMR_OC2Init(TMR9, &TMR_OCInitStructure);

  TMR_OC2PreloadConfig(TMR9, TMR_OCPreload_Disable);

  /* TMR enable counter */
  TMR_Cmd(TMR9, ENABLE);

  /* TMR IT enable */
  TMR_INTConfig(TMR9, TMR_INT_CC1 | TMR_INT_CC2, ENABLE);

  while (1)
  {
  }
}

/**
  * @brief  Configure TMR9 pins.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure;

  /* Enable TMR9 and GPIOA clock  */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR9 | RCC_APB2PERIPH_GPIOA, ENABLE);

  /* GPIOA Configuration:TMR9 Channel1 and 2 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_2 | GPIO_Pins_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
  * @brief  Configure the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
  NVIC_InitType NVIC_InitStructure;

  /* Enable the TMR9 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel =  TMR1_BRK_TMR9_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
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
  

