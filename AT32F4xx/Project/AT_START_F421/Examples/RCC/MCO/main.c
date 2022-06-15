/**
  ******************************************************************************
  * File   : RCC/MCO/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "at32f4xx.h"
#include "at32_board.h"

/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */

/** @addtogroup RCC_MCO
  * @{
  */ 

  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  MCO configuration program
  * @param  None
  * @retval None
  */
void MCO_config(void)
{
  GPIO_InitType GPIO_InitStructure;
  
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA,ENABLE); 
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_8; 
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OutType = GPIO_OutType_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  RCC_CLKOUTConfig(RCC_CLKOUT_PLL_Div4, RCC_MCOPRE_1);
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  AT32_Board_Init();

  MCO_config();
  
  for(;;)
  {
    AT32_LEDn_Toggle(LED4);
    Delay_ms(200);
  }
}


/**
  * @}
  */

/**
  * @}
  */

