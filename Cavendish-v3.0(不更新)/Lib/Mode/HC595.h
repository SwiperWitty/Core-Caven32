#ifndef _HC595_H_
#define _HC595_H_
#include "stm32f10x.h"

/*
	本文档库是 Otavia用于 控制 Zeno的 74CH595驱动程序、其兼容了级联输出（多个缓冲器）
*/

#define RCLK  	 GPIO_Pin_0
#define SRCLK 	 GPIO_Pin_1
#define SER_Data GPIO_Pin_12

#define HC595_GPIO	GPIOB
#define HC595_GPIO_TIM	RCC_APB2Periph_GPIOB


void HC595_Init (void);
void HC595_Byte (const char Dat);
void HC595 (const char *Dat,char num);


#endif
