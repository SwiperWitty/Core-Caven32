 /**
  ******************************************************************************
  * File   : TMR/OnePulse/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 


#include "at32f4xx.h"

/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */


/** @addtogroup TMR_OnePulse
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TMR_TimerBaseInitType  TMR_TMReBaseStructure;
TMR_ICInitType  TMR_ICInitStructure;
TMR_OCInitType  TMR_OCInitStructure;
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

  /* Configure the GPIO ports */
  GPIO_Configuration();

  /* TMR15 configuration: One Pulse mode ------------------------
     The external signal is connected to TMR4_CH2 pin (PB.07),
     The Rising edge is used as active edge,
     The One Pulse signal is output on TMR4_CH1 pin (PB.06)
     The TMR_Pulse defines the delay value
     The (TMR_Period -  TMR_Pulse) defines the One Pulse value.
     TMR2CLK = SystemCoreClock, we want to get TMR2 counter clock at 24 MHz:
     - Prescaler = (TMR2CLK / TMR2 counter clock) - 1
     The Autoreload value is 65535 (TMR15->ARR), so the maximum frequency value
     to trigger the TMR15 input is 24000000/65535 = 300 Hz.

     The TMR_Pulse defines the delay value, the delay value is fixed
     to 682.6 us:
     delay =  CCR1/TMR15 counter clock = 682.6 us.
     The (TMR_Period - TMR_Pulse) defines the One Pulse value,
     the pulse value is fixed to 2.048 ms:
     One Pulse value = (TMR_Period - TMR_Pulse) / TMR15 counter clock = 2.048 ms.

  * SystemCoreClock is set to 72 MHz for Low-density, Medium-density, High-density
    and Connectivity line devices and to 24 MHz for Value line devices
  ------------------------------------------------------------ */

  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) (SystemCoreClock / 24000000) - 1;
  /* TMRe base configuration */
  TMR_TimeBaseStructInit(&TMR_TMReBaseStructure);
  TMR_TMReBaseStructure.TMR_Period = 65535;
  TMR_TMReBaseStructure.TMR_DIV = PrescalerValue;
  TMR_TMReBaseStructure.TMR_ClockDivision = 0;
  TMR_TMReBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;

  TMR_TimeBaseInit(TMR15, &TMR_TMReBaseStructure);

  /* TMR15 PWM2 Mode configuration: Channel1 */
  TMR_OCStructInit(&TMR_OCInitStructure);
  TMR_OCInitStructure.TMR_OCMode = TMR_OCMode_PWM2;
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_Pulse = 16383;
  TMR_OCInitStructure.TMR_OCPolarity = TMR_OCPolarity_High;

  TMR_OC1Init(TMR15, &TMR_OCInitStructure);

  /* TMR15 configuration in Input Capture Mode */

  TMR_ICStructInit(&TMR_ICInitStructure);

  TMR_ICInitStructure.TMR_Channel = TMR_Channel_2;
  TMR_ICInitStructure.TMR_ICPolarity = TMR_ICPolarity_Rising;
  TMR_ICInitStructure.TMR_ICSelection = TMR_ICSelection_DirectTI;
  TMR_ICInitStructure.TMR_ICDIV = TMR_ICDIV_DIV1;
  TMR_ICInitStructure.TMR_ICFilter = 0;

  TMR_ICInit(TMR15, &TMR_ICInitStructure);

  /* One Pulse Mode selection */
  TMR_SelectOnePulseMode(TMR15, TMR_OPMode_Once);

  /* Input Trigger selection */
  TMR_SelectInputTrigger(TMR15, TMR_TRGSEL_TI2FP2);

  /* Slave Mode selection: Trigger Mode */
  TMR_SelectSlaveMode(TMR15, TMR_SlaveMode_Trigger);
  
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
  /* TMR15 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR15, ENABLE);

  /* GPIOB clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA, ENABLE);
}

/**
  * @brief  Configure the GPIOD Pins.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure;

  /* TMR15_CH1 pin (PA.02) configuration */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* TMR15_CH2 pin (PA.03) configuration */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_3;
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

