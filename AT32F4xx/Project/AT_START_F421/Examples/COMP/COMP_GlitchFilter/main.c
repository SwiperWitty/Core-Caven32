/**
  ******************************************************************************
  * File   : COMP/COMP_GlitchFilter/main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */

#include "at32f4xx.h"
#include "at32_board.h"
#include <stdio.h>

/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */

/** @addtogroup COMP_GlitchFilter
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void COMP_Config(void);
void TMR1_Init(void);

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  AT32_Board_Init();

  UART_Print_Init(115200);

  /* COMP1 Configuration */
  COMP_Config();

  /* TMR1 Configuration in input capture mode */
  TMR1_Init();

  printf("COMP glitch filter test..\r\n");  
  
  while (1)
  {
  }
}

/**
  * @brief  Configures COMP1: PA1 to COMP1 inverting input
  *                           and COMP1 output to TMR1 IC1.
  * @param  None
  * @retval None
  */
void COMP_Config(void)
{
  /* Init Structure definition */
  COMP_InitType COMP_InitStructure;
  GPIO_InitType GPIO_InitStructure;

  /* GPIOA Peripheral clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA, ENABLE);

  /* Configure PA1: PA1 is used as COMP1 non inveting input */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource0, GPIO_AF_7);
  
  /* COMP Peripheral clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_SYSCFGCOMP, ENABLE);

  /* COMP1 Init: PA1 is used COMP1 inverting input */
  COMP_InitStructure.COMP_INMInput = COMP_INMInput_VREFINT;
  COMP_InitStructure.COMP_Output = COMP_Output_None;
  COMP_InitStructure.COMP_OutPolarity = COMP_OutPolarity_NonInverted;
  COMP_InitStructure.COMP_Mode = COMP_Mode_Fast;
  COMP_InitStructure.COMP_Hysteresis = COMP_Hysteresis_No;
  COMP_Init(COMP1_Selection, &COMP_InitStructure);

  COMP_SCAL_BRGConfig(COMP_SCAL_BRG_11);
  
  /* Enable COMP1 */
  COMP_Cmd(COMP1_Selection, ENABLE);
  
  /* Filter out 64 PCLK(~533.33ns), each PCLK is 8.333ns */
  COMP_FilterConfig(63, 0x00, ENABLE);
}

void TMR1_Init(void)
{
  TMR_TimerBaseInitType  TMR_TimeBaseStructure;
  TMR_OCInitType  TMR_OCInitStructure;
  GPIO_InitType GPIO_InitStructure;

  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR1, ENABLE);
  
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource8, GPIO_AF_2);

  TMR_TimeBaseStructure.TMR_Period = 0x19;
  TMR_TimeBaseStructure.TMR_DIV = 0;
  TMR_TimeBaseStructure.TMR_ClockDivision = 0;
  TMR_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;
  TMR_TimeBaseStructure.TMR_RepetitionCounter = 0;
  TMR_TimeBaseInit(TMR1, &TMR_TimeBaseStructure);
  TMR_ClearFlag(TMR1, TMR_FLAG_Update);

  TMR_OCStructInit(&TMR_OCInitStructure);
  TMR_OCInitStructure.TMR_OCMode = TMR_OCMode_PWM2;
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_OutputNState = TMR_OutputNState_Enable;
  TMR_OCInitStructure.TMR_Pulse = 0x9;
  TMR_OCInitStructure.TMR_OCPolarity = TMR_OCPolarity_Low;
  TMR_OCInitStructure.TMR_OCNPolarity = TMR_OCNPolarity_High;
  TMR_OCInitStructure.TMR_OCIdleState = TMR_OCIdleState_Set;
  TMR_OCInitStructure.TMR_OCNIdleState = TMR_OCIdleState_Reset;

  TMR_OC1Init(TMR1, &TMR_OCInitStructure);

  /* TMR1 counter enable */
  TMR_Cmd(TMR1, ENABLE);

  /* TMR1 Main Output Enable */
  TMR_CtrlPWMOutputs(TMR1, ENABLE);
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

  /* Infinite loop */
  while (1)
  {
  }
}

#endif

/**
  * @}
  */

/**
  * @}
  */

