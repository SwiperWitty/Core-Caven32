 /**
  **************************************************************************
  * File   : PWR/SLEEP_USART/main.c
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

/** @addtogroup PWR_SLEEP_USART
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
NVIC_InitType NVIC_InitStructure;
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void USART1_NVIC_Config(void);

/* Private functions ---------------------------------------------------------*/

/**
 *	@brief  main function
 *	@param  None
 *	@retval None
 */
int main(void)
{
  uint32_t i;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_PWR, ENABLE);
  AT32_Board_Init();      ///<Initialize LEDs
  UART_Print_Init(115200);
  USART1_NVIC_Config();
  AT32_LEDn_ON(LED2);
  AT32_LEDn_ON(LED3);
  AT32_LEDn_ON(LED4);
  printf("Start\r\n");
  while (1)
  {
    AT32_LEDn_OFF(LED2);                             ///<Turn off LED2
    AT32_LEDn_OFF(LED3);                             ///<Turn off LED2
    printf("Enter Sleep Mode\r\n");
    SysTick->CTRL &= 0xFFFFFFFE;

    PWR_EnterSleepMode(PWR_SLEEPEntry_WFI);///Request to enter STOP mode with regulator in low power mode
    SysTick->CTRL |= 0x1;                            ///<Configures system clock after wake-up from STOP
    printf("Wakeup From Sleepmode by USART\r\n");
    AT32_LEDn_ON(LED2);                              ///<Turn on LED2
    for(i=0;i<500000;i++);
  }
}

void USART1_NVIC_Config(void)
{
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @}
  */ 

/**
  * @}
  */ 

