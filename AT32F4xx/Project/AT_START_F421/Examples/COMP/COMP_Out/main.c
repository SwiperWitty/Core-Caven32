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

  printf("COMP output test..\r\n");  
  
  while (1)
  {
    if(CaptureNumber > 1)
    {
      /* Capture computation */
      if (IC1ReadValue2 > IC1ReadValue1)
      {
        Capture = (IC1ReadValue2 - IC1ReadValue1); 
      }
      else
      {
        Capture = ((0xFFFF - IC1ReadValue1) + IC1ReadValue2); 
      }
      /* Frequency computation */ 
      TIM1Freq = (uint32_t)SystemCoreClock / Capture;
      printf("F1=%d\r\n",TIM1Freq); 
      CaptureNumber=0;
    }
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
  COMP_InitStructure.COMP_Output = COMP_Output_TMR1IC1;
  COMP_InitStructure.COMP_OutPolarity = COMP_OutPolarity_NonInverted;
  COMP_InitStructure.COMP_Mode = COMP_Mode_Fast;
  COMP_InitStructure.COMP_Hysteresis = COMP_Hysteresis_No;
  COMP_Init(COMP1_Selection, &COMP_InitStructure);

  COMP_SCAL_BRGConfig(COMP_SCAL_BRG_11);

  /* Enable COMP1 */
  COMP_Cmd(COMP1_Selection, ENABLE);
}

void TMR1_Init(void)
{
  TMR_TimerBaseInitType  TMR_TimeBaseStructure;
  TMR_ICInitType  TMR_ICInitStructure;
  NVIC_InitType NVIC_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR1, ENABLE);

  TMR_TimeBaseStructure.TMR_Period = 0xffff;
  TMR_TimeBaseStructure.TMR_DIV = 0;
  TMR_TimeBaseStructure.TMR_ClockDivision = 0;
  TMR_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;
  TMR_TimeBaseStructure.TMR_RepetitionCounter = 0;
  TMR_TimeBaseInit(TMR1, &TMR_TimeBaseStructure);
  TMR_ClearFlag(TMR1, TMR_FLAG_Update);

  TMR_ICInitStructure.TMR_ICPolarity = TMR_ICPolarity_Rising;
  TMR_ICInitStructure.TMR_ICSelection = TMR_ICSelection_DirectTI;
  TMR_ICInitStructure.TMR_ICDIV = TMR_ICDIV_DIV1;
  TMR_ICInitStructure.TMR_ICFilter = 0x00;

  TMR_ICInitStructure.TMR_Channel = TMR_Channel_1;
  TMR_ICInit(TMR1, &TMR_ICInitStructure);
  TMR_INTConfig(TMR1, TMR_INT_CC1, ENABLE);
  NVIC_InitStructure.NVIC_IRQChannel = TMR1_CC_IRQn;

  /* TIM2 IRQChannel enable */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the TIMx counter */
  TMR_Cmd(TMR1, ENABLE);

  /* Reset the flags */
  TMR1->STS &= 0;
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

