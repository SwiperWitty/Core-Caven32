 /**
  ******************************************************************************
  * File   : TMR/Input_Capture/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include <stdio.h>
#include "at32_board.h"

/** @addtogroup AT32F415_StdPeriph_Examples
  * @{
  */

/** @addtogroup TMR_Input_Capture
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TMR_ICInitType  TMR_ICInitStructure;

__IO uint32_t TMR3Freq = 0;
__IO uint32_t Sys_Counter = 0;
__IO uint16_t CaptureNumber = 0;
/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
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
  /* System Clocks Configuration */
  RCC_Configuration();

  /* NVIC configuration */
  NVIC_Configuration();

  
  /* Configure the GPIO ports */
  GPIO_Configuration();

  /* Board.c init */
  AT32_Board_Init();
  UART_Print_Init(115200);
  
  /* TMR3 configuration: Input Capture mode ---------------------
     The external signal is connected to TMR3 CH2 pin (PA.07)  
     The Rising edge is used as active edge,
     The TMR3 CCR2 is used to compute the frequency value 
  ------------------------------------------------------------ */

  TMR_ICStructInit(&TMR_ICInitStructure);
  TMR_ICInitStructure.TMR_Channel = TMR_Channel_2;
  TMR_ICInitStructure.TMR_ICPolarity = TMR_ICPolarity_Rising;
  TMR_ICInitStructure.TMR_ICSelection = TMR_ICSelection_DirectTI;
  TMR_ICInitStructure.TMR_ICDIV = TMR_ICDIV_DIV1;
  TMR_ICInitStructure.TMR_ICFilter = 0x0;

  TMR_ICInit(TMR3, &TMR_ICInitStructure);
  
  /* TMR enable counter */
  TMR_Cmd(TMR3, ENABLE);

  /* Enable the CC2 Interrupt Request */
  TMR_INTConfig(TMR3, TMR_INT_CC2, ENABLE);

  while (1)
  {
    Delay_ms(1000);
    printf("The external signal frequece is : %d\r\n",TMR3Freq);
    TMR3Freq = 0;
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

  /* GPIOA and GPIOB clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);
}

/**
  * @brief  Configure the GPIOD Pins.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure;

  /* TMR3 channel 2 pin (PA.07) configuration */
  GPIO_InitStructure.GPIO_Pins =  GPIO_Pins_7;
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

