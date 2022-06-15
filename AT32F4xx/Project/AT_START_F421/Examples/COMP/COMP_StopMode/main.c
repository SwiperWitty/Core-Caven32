/**
  ******************************************************************************
  * File   : COMP/COMP_StopMode/main.c
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

/** @addtogroup COMP_StopMode
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ErrorStatus HSEStartUpStatus;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void COMP_Config(void);
void COMP_EXTI_Init(void);
void SYSCLKConfig_STOP(void);
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
  
  while (1)
  {
    printf("\r\nENTER STOP MODE...\r\n");
    /* Request to enter STOP mode with regulator in low power */
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
    SYSCLKConfig_STOP();
    printf("Wakeup from stop mode by COMP, MCU is running...\r\n");
    Delay_sec(1);
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
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource6, GPIO_AF_7);

  /* Configure PA1: PA1 is used as COMP1 non inveting input */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

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

  /* TMR1 Configuration in input capture mode */
  COMP_EXTI_Init();
}

void COMP_EXTI_Init(void)
{
  EXTI_InitType   EXTI_InitStructure;
  NVIC_InitType   NVIC_InitStructure = {0};

  EXTI_InitStructure.EXTI_Line = EXTI_Line21;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineEnable = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  EXTI_ClearIntPendingBit(EXTI_Line21);

  NVIC_InitStructure.NVIC_IRQChannel = ADC_COMP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void SYSCLKConfig_STOP(void)
{
  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ENABLE);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStable();

  if(HSEStartUpStatus == SUCCESS)
  {

    /* Enable PLL */
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLSTBL) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSelction_PLL);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSelction() != 0x08)
    {
    }
  }
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

