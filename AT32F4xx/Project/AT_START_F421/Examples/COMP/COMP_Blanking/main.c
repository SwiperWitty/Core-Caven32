/**
  ******************************************************************************
  * File   : COMP/COMP_Out/main.c
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

/** @addtogroup COMP_Out
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t Capture = 0;
__IO uint32_t TIM1Freq;
extern __IO uint16_t CaptureNumber;
extern __IO uint32_t IC1ReadValue1; 
extern __IO uint32_t IC1ReadValue2;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void COMP_Config(void);
void TIM1_PWM_Init(void);

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
  TIM1_PWM_Init();
  
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
  COMP_InitStructure.COMP_INMInput = COMP_INMInput_1_4VREFINT;
  COMP_InitStructure.COMP_Output = COMP_Output_None;
  COMP_InitStructure.COMP_OutPolarity = COMP_OutPolarity_NonInverted;
  COMP_InitStructure.COMP_Mode = COMP_Mode_Fast;
  COMP_InitStructure.COMP_Hysteresis = COMP_Hysteresis_No;
  COMP_Init(COMP1_Selection, &COMP_InitStructure);

  COMP_SCAL_BRGConfig(COMP_SCAL_BRG_11);
  
  COMP_BlankingConfig(COMP_Blanking_TMR1OC4);

  /* Enable COMP1 */
  COMP_Cmd(COMP1_Selection, ENABLE);
}

void TIM1_PWM_Init(void)
{
  GPIO_InitType GPIO_InitStructure;
  TMR_TimerBaseInitType  TIM_TimeBaseStructure;
  TMR_OCInitType  TIM_OCInitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR1, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA|RCC_AHBPERIPH_GPIOB, ENABLE);
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource11, GPIO_AF_2);

  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_11;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Time Base configuration */
  TIM_TimeBaseStructure.TMR_DIV = 99;
  TIM_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;
  TIM_TimeBaseStructure.TMR_Period = 400;
  TIM_TimeBaseStructure.TMR_ClockDivision = 0;
  TIM_TimeBaseStructure.TMR_RepetitionCounter = 0;
  TMR_TimeBaseInit(TMR1, &TIM_TimeBaseStructure);

  /* Channel 4 Configuration in PWM mode */
  TIM_OCInitStructure.TMR_OCMode = TMR_OCMode_PWM1;
  TIM_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TIM_OCInitStructure.TMR_OutputNState = TMR_OutputNState_Enable;
  TIM_OCInitStructure.TMR_Pulse = 100;
  TIM_OCInitStructure.TMR_OCPolarity = TMR_OCPolarity_High;
  TIM_OCInitStructure.TMR_OCNPolarity = TMR_OCNPolarity_High;
  TIM_OCInitStructure.TMR_OCIdleState = TMR_OCIdleState_Set;
  TIM_OCInitStructure.TMR_OCNIdleState = TMR_OCNIdleState_Set;
  TMR_OC4Init(TMR1, &TIM_OCInitStructure);
  TMR_OC4PreloadConfig(TMR1, TMR_OCPreload_Enable);

  /* TIMx counter enable */
  TMR_Cmd(TMR1, ENABLE);

  /* Main Output Enable */
  TMR_CtrlPWMOutputs(TMR1, ENABLE);

  TMR_CCxCmd(TMR1, TMR_Channel_4, TMR_CCx_Enable);
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

