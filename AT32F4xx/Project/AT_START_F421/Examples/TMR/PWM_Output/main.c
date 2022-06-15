 /**
  ******************************************************************************
  * File   : TMR/PWM_Output/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 

#include "at32f4xx.h"

/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */

/** @addtogroup TMR_PWM_Output
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TMR_TimerBaseInitType  TMR_TMReBaseStructure;
TMR_OCInitType  TMR_OCInitStructure;
uint16_t CCR1_Val = 333;
uint16_t CCR2_Val = 249;
uint16_t CCR3_Val = 166;
uint16_t CCR4_Val = 83;
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
  /*!< At this stage the microcontroller clock setting is already configured,
       this is done through SystemInit() function which is called from startup
       file (startup_at32f413_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_at32f4xx.c file
     */

  /* System Clocks Configuration */
  RCC_Configuration();

  /* GPIO Configuration */
  GPIO_Configuration();

  /* -----------------------------------------------------------------------
    TMR3 Configuration: generate 4 PWM signals with 4 different duty cycles:
    The TMR3CLK frequency is set to SystemCoreClock (Hz), to get TMR3 counter
    clock at 24 MHz the Prescaler is computed as following:
     - Prescaler = (TMR3CLK / TMR3 counter clock) - 1

    The TMR3 is running at 36 KHz: TMR3 Frequency = TMR3 counter clock/(ARR + 1)
                                                  = 24 MHz / 666 = 36 KHz
    TMR3 Channel1 duty cycle = (TMR3_CCR1/ TMR3_ARR)* 100 = 50%
    TMR3 Channel2 duty cycle = (TMR3_CCR2/ TMR3_ARR)* 100 = 37.5%
    TMR3 Channel3 duty cycle = (TMR3_CCR3/ TMR3_ARR)* 100 = 25%
    TMR3 Channel4 duty cycle = (TMR3_CCR4/ TMR3_ARR)* 100 = 12.5%
  ----------------------------------------------------------------------- */
  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) (SystemCoreClock / 24000000) - 1;
  /* TMRe base configuration */
  TMR_TimeBaseStructInit(&TMR_TMReBaseStructure);
  TMR_TMReBaseStructure.TMR_Period = 665;
  TMR_TMReBaseStructure.TMR_DIV = PrescalerValue;
  TMR_TMReBaseStructure.TMR_ClockDivision = 0;
  TMR_TMReBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;

  TMR_TimeBaseInit(TMR3, &TMR_TMReBaseStructure);

  /* PWM1 Mode configuration: Channel1 */
  TMR_OCStructInit(&TMR_OCInitStructure);
  TMR_OCInitStructure.TMR_OCMode = TMR_OCMode_PWM1;
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = CCR1_Val;
  TMR_OCInitStructure.TMR_OCPolarity = TMR_OCPolarity_High;

  TMR_OC1Init(TMR3, &TMR_OCInitStructure);

  TMR_OC1PreloadConfig(TMR3, TMR_OCPreload_Enable);

  /* PWM1 Mode configuration: Channel2 */
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = CCR2_Val;

  TMR_OC2Init(TMR3, &TMR_OCInitStructure);

  TMR_OC2PreloadConfig(TMR3, TMR_OCPreload_Enable);

  /* PWM1 Mode configuration: Channel3 */
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = CCR3_Val;

  TMR_OC3Init(TMR3, &TMR_OCInitStructure);

  TMR_OC3PreloadConfig(TMR3, TMR_OCPreload_Enable);

  /* PWM1 Mode configuration: Channel4 */
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = CCR4_Val;

  TMR_OC4Init(TMR3, &TMR_OCInitStructure);

  TMR_OC4PreloadConfig(TMR3, TMR_OCPreload_Enable);

  TMR_ARPreloadConfig(TMR3, ENABLE);

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
  /* TMR3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR3, ENABLE);

  /* GPIOA and GPIOB clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA | RCC_AHBPERIPH_GPIOB, ENABLE);
}

/**
  * @brief  Configure the TMR3 Ouput Channels.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure;

  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource6, GPIO_AF_1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource7, GPIO_AF_1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinsSource0, GPIO_AF_1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinsSource1, GPIO_AF_1);
  
  /* GPIOA Configuration:TMR3 Channel1, 2, 3 and 4 as alternate function push-pull */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_6 | GPIO_Pins_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_0 | GPIO_Pins_1;
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

