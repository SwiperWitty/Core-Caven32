/**
  **************************************************************************
  * File   : PWR/PVD/main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  **************************************************************************
  * @brief     : Initializes and turn on LED.Configure PVD and EXTI.
  *              Then if monitor a PVD event,an interrupt on PVD will happen,and it will cause a Toggle on LED2 and LED3 and LED4.
  */
 
 /* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "at32f4xx.h"
#include "at32_board.h"

/** @addtogroup AT32F415_StdPeriph_Examples
  * @{
  */

/** @addtogroup PWR_PVD
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
GPIO_InitType GPIO_InitStructure;
EXTI_InitType EXTI_InitStructure;
NVIC_InitType NVIC_InitStructure = {0};

/* Private function prototypes -----------------------------------------------*/
void EXTI_Configuration(void);
void NVIC_Configuration(void);

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
	AT32_Board_Init();                          ///<Initialize LEDs
	AT32_LEDn_ON(LED2);
	AT32_LEDn_ON(LED3);
	AT32_LEDn_ON(LED4);
	
	RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_PWR, ENABLE); ///<Enable PWR and BKP clock
	PWR_PVDLevelConfig(PWR_PVDS_2V9);          ///<Configure the PVD Level to 2.9V
	PWR_PVDCtrl(ENABLE);                       ///<Enable the PVD Output

	EXTI_Configuration();                      ///<Configure EXTI Line to generate an interrupt on rising and falling edge
	for(i=0;i<2000;i++);                       ///<some delay

	EXTI_ClearIntPendingBit(EXTI_Line16);
	NVIC_ClearPendingIRQ(PVD_IRQn);
	NVIC_Configuration();                      ///<NVIC configuration
	while (1)
	{
	}
}

/**
  * @brief  EXTI Configuration.Configures EXTI Line 16.
  * @param  None
  * @retval None
  */
void EXTI_Configuration(void)
{
    EXTI_ClearIntPendingBit(EXTI_Line16);
    EXTI_InitStructure.EXTI_Line        = EXTI_Line16;
    EXTI_InitStructure.EXTI_Mode        = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger     = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineEnable  = ENABLE;
    EXTI_Init(&EXTI_InitStructure);   ///<Configure EXTI Line16(PVD Output) to generate an interrupt on rising and falling edges
}

/**
  * @brief  NVIC Configuration.Configures NVIC and Vector Table base location.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);   ///<Configure one bit for preemption priority
    NVIC_InitStructure.NVIC_IRQChannel                      = PVD_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority           = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
  * @}
  */ 

/**
  * @}
  */ 

