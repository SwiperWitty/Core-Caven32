 /**
  ******************************************************************************
  * File   : TMR/TMR2_Plus/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 


#include "at32f4xx.h"
/** @addtogroup AT32F415_StdPeriph_Examples
  * @{
  */


/** @addtogroup TMR_TMR2_Plus
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TMR_TimerBaseInitType  TMR_TMReBaseStructure;
TMR_OCInitType  TMR_OCInitStructure;
uint32_t CCR1_Val = 0X7FFFF;
uint32_t CCR2_Val = 0X3FFFF;
uint32_t CCR3_Val = 0X1FFFF;
uint32_t CCR4_Val = 0XFFFF;
uint16_t PrescalerValue = 0;

/* Private function prototypes -----------------------------------------------*/
void GPIO_Configuration(void);
void RCC_Configuration(void);
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

  /* In this case , CH1 CH2 CH3 CH4 frequency = TMRclock / (Period+1) / (DIV+1) = 183 HZ  Systemclock = 192Mhz
                    CH1 dutycycle = 50%
                    CH2 dutycycle = 25%
                    CH3 dutycycle = 12.5%
                    CH4 dutycycle = 6.25%             */

  /* System Clocks Configuration */
  RCC_Configuration();

  /* GPIO Configuration */
  GPIO_Configuration();
  
  /* Enable Plus Mode */
  TMR_SelectPlusMode(TMR2,TMR_Plus_Mode_Enable);
  
  /* TMRe base configuration */
  TMR_TimeBaseStructInit(&TMR_TMReBaseStructure);
  TMR_TMReBaseStructure.TMR_Period = 0xFFFFF;
  TMR_TMReBaseStructure.TMR_DIV = PrescalerValue;
  TMR_TMReBaseStructure.TMR_ClockDivision = 0;
  TMR_TMReBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;

  TMR_TimeBaseInit(TMR2, &TMR_TMReBaseStructure);

  /* PWM1 Mode configuration: Channel1 */
  TMR_OCStructInit(&TMR_OCInitStructure);
  TMR_OCInitStructure.TMR_OCMode = TMR_OCMode_PWM1;
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = CCR1_Val;
  TMR_OCInitStructure.TMR_OCPolarity = TMR_OCPolarity_High;

  TMR_OC1Init(TMR2, &TMR_OCInitStructure);

  TMR_OC1PreloadConfig(TMR2, TMR_OCPreload_Enable);

  /* PWM1 Mode configuration: Channel2 */
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = CCR2_Val;

  TMR_OC2Init(TMR2, &TMR_OCInitStructure);

  TMR_OC2PreloadConfig(TMR2, TMR_OCPreload_Enable);

  /* PWM1 Mode configuration: Channel3 */
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = CCR3_Val;

  TMR_OC3Init(TMR2, &TMR_OCInitStructure);

  TMR_OC3PreloadConfig(TMR2, TMR_OCPreload_Enable);

  /* PWM1 Mode configuration: Channel4 */
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = CCR4_Val;

  TMR_OC4Init(TMR2, &TMR_OCInitStructure);

  TMR_OC4PreloadConfig(TMR2, TMR_OCPreload_Enable);

  TMR_ARPreloadConfig(TMR2, ENABLE);

  /* TMR3 enable counter */
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
  /* TMR3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR2, ENABLE);

  /* GPIOA and GPIOB clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA , ENABLE);
}

/**
  * @brief  Configure the TMR3 Ouput Channels.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure;

  /* GPIOA Configuration:TMR3 Channel1, 2, 3 and 4 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_0 | GPIO_Pins_1 | GPIO_Pins_2 | GPIO_Pins_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;

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

