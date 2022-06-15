 /**
  ******************************************************************************
  * File   : TMR/Parallel_Synchro/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */


#include "at32f4xx.h"

/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */


/** @addtogroup TMR_Parallel_Synchro
  * @{ 
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
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

  /* TMRers synchronisation in parallel mode ----------------------------
     1/TMR15 is configured as Master TMRer:
     - PWM Mode is used
     - The TMR15 Update event is used as Trigger Output
     2/TMR3 and TMR1 are slaves for TMR15,
     - PWM Mode is used
     - The ITR1(TMR15) is used as input trigger for both slaves
     - Gated mode is used, so starts and stops of slaves counters
      are controlled by the Master trigger output signal(update event).

     The TMRxCLK is fixed to 120 MHz, the TMR15 counter clock is 120 MHz.
     The Master TMRer TMR15 is running at 500 KHz and the duty cycle
     is equal to 25%
     The TMR3 is running:
     - At (TMR15 frequency)/ (TMR3 period + 1) = 50 KHz and a duty cycle
     equal to TMR3_CCR1/(TMR3_ARR + 1) = 30%
     The TMR1 is running:
     - At (TMR15 frequency)/ (TMR1 period + 1) = 100 KHz and a duty cycle
     equal to TMR1_CCR1/(TMR1_ARR + 1) = 60%

  -------------------------------------------------------------------- */

  /* TMRe base configuration */
  TMR_TimeBaseStructInit(&TMR_TMReBaseStructure);
  TMR_TMReBaseStructure.TMR_Period = 239;
  TMR_TMReBaseStructure.TMR_DIV = 0;
  TMR_TMReBaseStructure.TMR_ClockDivision = 0;
  TMR_TMReBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;

  TMR_TimeBaseInit(TMR15, &TMR_TMReBaseStructure);

  TMR_TMReBaseStructure.TMR_Period = 9;
  TMR_TimeBaseInit(TMR3, &TMR_TMReBaseStructure);

  TMR_TMReBaseStructure.TMR_Period = 4;
  TMR_TimeBaseInit(TMR1, &TMR_TMReBaseStructure);

  /* Master Configuration in PWM1 Mode */
  TMR_OCStructInit(&TMR_OCInitStructure);
  TMR_OCInitStructure.TMR_OCMode = TMR_OCMode_PWM1;
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = 64;
  TMR_OCInitStructure.TMR_OCPolarity = TMR_OCPolarity_High;

  TMR_OC1Init(TMR15, &TMR_OCInitStructure);

  /* Select the Master Slave Mode */
  TMR_SelectMasterSlaveMode(TMR15, TMR_MasterSlaveMode_Enable);

  /* Master Mode selection */
  TMR_SelectOutputTrigger(TMR15, TMR_TRGOSource_Update);

  /* Slaves Configuration: PWM1 Mode */
  TMR_OCInitStructure.TMR_OCMode = TMR_OCMode_PWM1;
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = 3;

  TMR_OC1Init(TMR3, &TMR_OCInitStructure);

  TMR_OC1Init(TMR1, &TMR_OCInitStructure);

  /* Slave Mode selection: TMR3 */
  TMR_SelectSlaveMode(TMR3, TMR_SlaveMode_Gate);
  TMR_SelectInputTrigger(TMR3, TMR_TRGSEL_ITR2);

  /* Slave Mode selection: TMR1 */
  TMR_SelectSlaveMode(TMR1, TMR_SlaveMode_Gate);
  TMR_SelectInputTrigger(TMR1, TMR_TRGSEL_ITR0);

  /* TMR enable counter */
  TMR_Cmd(TMR3, ENABLE);
  TMR_Cmd(TMR15, ENABLE);
  TMR_Cmd(TMR1, ENABLE);

  /* Main Output Enable */
  TMR_CtrlPWMOutputs(TMR1, ENABLE);
  
  /* Main Output Enable */
  TMR_CtrlPWMOutputs(TMR15, ENABLE);
  
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
  /* TMR1, TMR3 and TMR15 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR1 | RCC_APB2PERIPH_TMR15, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR3, ENABLE);

  /* GPIOA, GPIOB clocks enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA | RCC_AHBPERIPH_GPIOB, ENABLE);
}

/**
  * @brief  Configure the GPIO Pins.
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
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
/* GPIOA Configuration: PA6(TMR3 CH1) as alternate function push-pull */
  GPIO_StructInit(&GPIO_InitStructure);
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

  
