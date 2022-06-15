/**
  **************************************************************************
  * File   : led.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : none
  **************************************************************************
  */
#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
/* Private macro -------------------------------------------------------------*/
#define LED2_GPIO_PIN    GPIO_Pins_13
#define LED3_GPIO_PIN    GPIO_Pins_14
#define LED4_GPIO_PIN    GPIO_Pins_15
#define LED_GPIO_PORT    GPIOD 

#define LED2 PDout(13)// PD13
#define LED3 PDout(14)// PD14	
#define LED4 PDout(15)// PD15	

void LED_Init(void);
		 				    
#endif
