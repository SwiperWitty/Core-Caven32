 /**
  ******************************************************************************
  * File   : TMR/Gated_Mode/main.c 
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

/** @addtogroup TMR_Gated_Mode
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TMR_TimerBaseInitType  TMR_TimeBaseStructure = {0};
TMR_ICInitType  TMR_ICInitStructure = {0};
TMR_OCInitType  TMR_OCInitStructure = {0};

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
  /* System Clocks Configuration */
  RCC_Configuration();

  /* Configure the GPIO ports */
  GPIO_Configuration();

  /* Gated mode test -----
    1/TMR1 is configured as Master Timer:
     - Toggle Mode is used

    2/TMR1 is configured as Slave Timer for an external Trigger connected
     to TMR1 TI2 pin (TMR1 CH2 configured as input pin):
     - The TMR1 TI2FP2 is used as Trigger Input
     - Rising edge is used to start and stop the TMR1: Gated Mode.

    * The TIMxCLK is fixed to 120 MHZ, the Prescaler is equal to 2 so the TMRx clock 
      counter is equal to 40 MHz.
      The Three Timers are running at: 
      TMRx frequency = TMRx clock counter/ 2*(TIMx_Period + 1) = 270.27 KHz. 
  */
  /* Time base configuration */
  TMR_TimeBaseStructInit(&TMR_TimeBaseStructure);
  TMR_TimeBaseStructure.TMR_Period = 73;
  TMR_TimeBaseStructure.TMR_DIV = 2;
  TMR_TimeBaseStructure.TMR_ClockDivision = 0;
  TMR_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;

  TMR_TimeBaseInit(TMR1, &TMR_TimeBaseStructure);

  /* Master Configuration in Toggle Mode */
  TMR_OCStructInit(&TMR_OCInitStructure);
  TMR_OCInitStructure.TMR_OCMode = TMR_OCMode_Toggle;
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = 64;
  TMR_OCInitStructure.TMR_OCPolarity = TMR_OCPolarity_High;

  TMR_OC1Init(TMR1, &TMR_OCInitStructure);

  /* TMR1 Input Capture Configuration */
  TMR_ICStructInit(&TMR_ICInitStructure);
  TMR_ICInitStructure.TMR_Channel = TMR_Channel_2;
  TMR_ICInitStructure.TMR_ICPolarity = TMR_ICPolarity_Rising;
  TMR_ICInitStructure.TMR_ICSelection = TMR_ICSelection_DirectTI;
  TMR_ICInitStructure.TMR_ICDIV = TMR_ICDIV_DIV1;
  TMR_ICInitStructure.TMR_ICFilter = 0;

  TMR_ICInit(TMR1, &TMR_ICInitStructure);

  /* TMR1 Input trigger configuration: External Trigger connected to TI2 */
  TMR_SelectInputTrigger(TMR1, TMR_TRGSEL_TI2FP2);
  TMR_SelectSlaveMode(TMR1, TMR_SlaveMode_Gate);

  /* TMR1 Main Output Enable */
  TMR_CtrlPWMOutputs(TMR1, ENABLE);

  /* TMR enable counter */
  TMR_Cmd(TMR1, ENABLE);

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

  /* TMR1, GPIOA, and GPIOB clocks enable */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR1, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA | RCC_AHBPERIPH_GPIOB, ENABLE);
}

/**
  * @brief  Configure the GPIO Pins.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure = {0};

  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource8, GPIO_AF_2);
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource9, GPIO_AF_2);
  
  /* GPIOA Configuration: PA.08(TMR1 CH1) as alternate function push-pull */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_8 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* GPIOA Configuration: PA.09(TMR1 CH2) */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

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
  
