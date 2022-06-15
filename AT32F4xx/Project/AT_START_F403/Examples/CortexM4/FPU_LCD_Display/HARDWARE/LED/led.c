/**
  **************************************************************************
  * File   : led.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Init the LED.
  **************************************************************************
  */
#include "led.h"

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup CortexM4_FPU_LCD_Display
  * @{
  */ 
  
/**
  * @brief  GPIO Initialize For LED.
  * @param  None
  * @retval None
  */
void LED_Init(void)
{
  GPIO_InitType GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOD, ENABLE);

  /*PD13->LED4 PD14->LED2 PD15->LED3*/
  GPIO_InitStructure.GPIO_Pins = LED2_GPIO_PIN | LED3_GPIO_PIN | LED4_GPIO_PIN;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
  GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);

  /* LED2, LED3, LED4 OFF*/
  GPIO_SetBits(LED_GPIO_PORT, LED2_GPIO_PIN);
  GPIO_SetBits(LED_GPIO_PORT, LED3_GPIO_PIN);
  GPIO_SetBits(LED_GPIO_PORT, LED4_GPIO_PIN);
}
/**
  * @}
  */ 

/**
  * @}
  */ 
  
