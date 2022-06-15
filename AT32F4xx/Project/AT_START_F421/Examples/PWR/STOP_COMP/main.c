 /**
  **************************************************************************
  * File   : PWR/STOP_COMP/main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  **************************************************************************
  */
 
 /* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "at32f4xx.h"
#include "at32_board.h"

/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */

/** @addtogroup PWR_STOP
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ERTC_CLOCK_SOURCE_LSI
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
GPIO_InitType GPIO_InitStructure;
EXTI_InitType EXTI_InitStructure;
NVIC_InitType NVIC_InitStructure = {0};
ErrorStatus HSEStartUpStatus;

/* Private function prototypes -----------------------------------------------*/
void SYSCLKConfig_STOP(void);
void EXTI_Configuration(void);
void NVIC_Configuration(void);
void COMP_Config(void);
void EXTI21_Config(void);

/* Private functions ---------------------------------------------------------*/

/**
 *	@brief  main function
 *	@param  None
 *	@retval None
 */
int main(void)
{
  uint32_t i;
  GPIO_StructInit(&GPIO_InitStructure);
  EXTI_StructInit(&EXTI_InitStructure);
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_PWR, ENABLE);
  AT32_Board_Init();      ///<Initialize LEDs
  UART_Print_Init(115200);
  AT32_LEDn_ON(LED2);
  AT32_LEDn_ON(LED3);
  AT32_LEDn_ON(LED4);
  printf("Start\r\n");
  COMP_Config();
  EXTI21_Config();

  while (1)
  {                          ///<Wait until last write operation on RTC registers has finished
    SysTick->CTRL &= 0xFFFFFFFE;
    AT32_LEDn_OFF(LED2);                             ///<Turn off LED2

    PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);///Request to enter STOP mode with regulator in low power mode
    SysTick->CTRL |= 0x1;
    SYSCLKConfig_STOP();                             ///<Configures system clock after wake-up from STOP
    printf("Wakeup From Stopmode by COMP\r\n");
    AT32_LEDn_ON(LED2);                              ///<Turn on LED2
    for(i=0;i<500000;i++);
  }
}

/**
  * @brief  Configures system clock after wake-up from STOP: enable HSE, PLL and select PLL as system clock source.
  * @param  None
  * @retval None
  */
void SYSCLKConfig_STOP(void)
{
    uint32_t i;
    RCC_HSEConfig(RCC_HSE_ENABLE);   ///<Enable HSE
    for(i=0;i<20000;i++);
    HSEStartUpStatus = RCC_WaitForHSEStable();
    if(HSEStartUpStatus == SUCCESS)
    {
        RCC_PLLCmd(ENABLE);   ///<Enable PLL
        while(RCC_GetFlagStatus(RCC_FLAG_PLLSTBL) == RESET)  ///<Wait till PLL is ready
        {
        }
        RCC_SYSCLKConfig(RCC_SYSCLKSelction_PLL);            ///<Select PLL as system clock source
        while(RCC_GetSYSCLKSelction() != 0x08)               ///<Wait till PLL is used as system clock source
        {
        }
    }
}

/**
  * @brief  Configures COMP1: PA1 to COMP1 inverting input
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
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_Pull = GPIO_Pull_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* COMP Peripheral clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_SYSCFGCOMP, ENABLE);

  /* COMP1 Init: PA1 is used COMP1 inverting input */
  COMP_InitStructure.COMP_INMInput = COMP_INMInput_1_4VREFINT;
  COMP_InitStructure.COMP_Output = COMP_Output_TMR1IC1;
  COMP_InitStructure.COMP_OutPolarity = COMP_OutPolarity_NonInverted;
  COMP_InitStructure.COMP_Mode = COMP_Mode_Fast;
  COMP_InitStructure.COMP_Hysteresis = COMP_Hysteresis_No;
  COMP_Init(COMP1_Selection, &COMP_InitStructure);
  /* Enable COMP1 */
  COMP_Cmd(COMP1_Selection, ENABLE);
}

/**
  * @brief  EXTI19 Config.Configure PA0 in interrupt mode
  * @param  None
  * @retval None
  */
void EXTI21_Config(void)
{
	EXTI_InitStructure.EXTI_Line = EXTI_Line21;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineEnable = ENABLE;
	EXTI_Init(&EXTI_InitStructure);                              ///<Configure EXTI0 line
	
	NVIC_InitStructure.NVIC_IRQChannel = ADC_COMP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);                              ///<Enable and set EXTI0 Interrupt to the lowest priority
}
/**
  * @}
  */ 

/**
  * @}
  */ 

