 /**
  ******************************************************************************
  * File   : TMR/PWM_Input/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */


#include "at32f4xx.h"
#include "stdio.h"
#include "at32_board.h"

/** @addtogroup AT32F415_StdPeriph_Examples
  * @{
  */

/** @addtogroup TMR_PWM_Input
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TMR_ICInitType  TMR_ICInitStructure;
USART_InitType  USART_InitStructure;

extern uint16_t DutyCycle;
extern uint32_t Frequency;

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void USART1_Init(USART_InitType* USART_InitStruct);

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

  /* System Clocks Configuration */
  RCC_Configuration();

  /* NVIC configuration */
  NVIC_Configuration();

  /* Configure the GPIO ports */
  GPIO_Configuration();

  /* Board.c init */
  UART_Print_Init(115200);
  
  /* TMR3 configuration: PWM Input mode ------------------------
     The external signal is connected to TMR3 CH2 pin (PA.01),
     The Rising edge is used as active edge,
     The TMR3 CCR2 is used to compute the frequency value
     The TMR3 CCR1 is used to compute the duty cycle value
  ------------------------------------------------------------ */


  TMR_ICStructInit(&TMR_ICInitStructure);
  TMR_ICInitStructure.TMR_Channel = TMR_Channel_2;
  TMR_ICInitStructure.TMR_ICPolarity = TMR_ICPolarity_Rising;
  TMR_ICInitStructure.TMR_ICSelection = TMR_ICSelection_DirectTI;
  TMR_ICInitStructure.TMR_ICDIV = TMR_ICDIV_DIV1;
  TMR_ICInitStructure.TMR_ICFilter = 0x0;

  TMR_PWMIConfig(TMR3, &TMR_ICInitStructure);

  /* Select the TMR3 Input Trigger: TI2FP2 */
  TMR_SelectInputTrigger(TMR3, TMR_TRGSEL_TI2FP2);

  /* Select the slave Mode: Reset Mode */
  TMR_SelectSlaveMode(TMR3, TMR_SlaveMode_Reset);

  /* Enable the Master/Slave Mode */
  TMR_SelectMasterSlaveMode(TMR3, TMR_MasterSlaveMode_Enable);

  /* TMR enable counter */
  TMR_Cmd(TMR3, ENABLE);

  /* Enable the CC2 Interrupt Request */
  TMR_INTConfig(TMR3, TMR_INT_CC2, ENABLE);

  while (1)
  {
    printf("Frequency=%dHZ,Dutycycle=%d%%\r\n",Frequency,DutyCycle);
  }
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

  /* GPIOA clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);
}

/**
  * @brief  Configure the GPIO Pins.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure;

  /* TMR3 channel 2 pin (PA.07) configuration */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
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

  /* Enable the TMR3 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TMR3_GLOBAL_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
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

