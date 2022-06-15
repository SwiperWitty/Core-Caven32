 /**
  ******************************************************************************
  * File   : TMR/TMR10_PWMOutput/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 


#include "at32f4xx.h"

/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */

/** @addtogroup TMR_TMR10_PWMOutput
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TMR_TimerBaseInitType  TMR_TMReBaseStructure;
TMR_OCInitType  TMR_OCInitStructure;
uint16_t CCR1Val = 249;
uint16_t PrescalerValue = 0;

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
       file (startup_at32f413_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_at32f4xx.c file
     */

  /* GPIO Configuration */
  GPIO_Configuration();

  /* -----------------------------------------------------------------------
    TMR16 Configuration: generate 1 PWM signal.
    The TMR10CLK frequency is set to SystemCoreClock (120 MHz), to get TMR16 counter
    clock at 120 MHz the Prescaler is computed as following:
     - Prescaler = (TMR16CLK / TMR16 counter clock) - 1

    The TMR16 is running at 36 KHz: TMR16 Frequency = TMR16 counter clock/(ARR + 1)
                                                  = 120 MHz / 666 = 36 KHz
    TMR16 Channel1 duty cycle = (TMR16_CCR1/ TMR16_ARR)* 100 = 37.5%
  ----------------------------------------------------------------------- */
  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) (SystemCoreClock / 24000000) - 1;
  /* TMRe base configuration */
  TMR_TimeBaseStructInit(&TMR_TMReBaseStructure);
  TMR_TMReBaseStructure.TMR_Period = 665;
  TMR_TMReBaseStructure.TMR_DIV = PrescalerValue;
  TMR_TMReBaseStructure.TMR_ClockDivision = 0;
  TMR_TMReBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;

  TMR_TimeBaseInit(TMR16, &TMR_TMReBaseStructure);

  /* PWM1 Mode configuration: Channel1 */
  TMR_OCStructInit(&TMR_OCInitStructure);
  TMR_OCInitStructure.TMR_OCMode = TMR_OCMode_PWM1;
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = CCR1Val;
  TMR_OCInitStructure.TMR_OCPolarity = TMR_OCPolarity_High;

  TMR_OC1Init(TMR16, &TMR_OCInitStructure);

  TMR_OC1PreloadConfig(TMR16, TMR_OCPreload_Enable);

  TMR_ARPreloadConfig(TMR16, ENABLE);

  /* TMR16 enable counter */
  TMR_Cmd(TMR16, ENABLE);
  
  /* Main Output Enable */
  TMR_CtrlPWMOutputs(TMR16, ENABLE);

  while (1)
  {}
}

/**
  * @brief  Configure TMR16 pin.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure;

  /* Enable TMR16 and GPIOB clock */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR16, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOB, ENABLE);

  /* GPIOB Configuration: TMR16 Channel1 as alternate function push-pull */
  GPIO_PinAFConfig(GPIOB, GPIO_PinsSource8, GPIO_AF_2);
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;

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

