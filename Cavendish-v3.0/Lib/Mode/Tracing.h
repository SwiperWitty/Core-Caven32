#ifndef _Tracing_h_
#define _Tracing_h_

//	可以跨芯片移植	需要大改底层
/*

			循迹不要启用 ADC ！
			Attention ：Using Tracing enable ADC
												--2021.12.04
*/

#include "stm32f10x.h"
						
#define Trac_1 			GPIO_Pin_2
#define Trac_2			GPIO_Pin_5
#define Trac_3 			GPIO_Pin_4
#define Trac_4			GPIO_Pin_5
#define Trac_5 			GPIO_Pin_4
						
#define Trac_1_GPIO		GPIOB
#define Trac_2_GPIO		GPIOC
#define Trac_3_GPIO		GPIOC
#define Trac_4_GPIO		GPIOA
#define Trac_5_GPIO		GPIOA
							
#define Trac_1_GPIO_TIM		RCC_APB2Periph_GPIOB
#define Trac_2_GPIO_TIM		RCC_APB2Periph_GPIOA
#define Trac_3_GPIO_TIM		RCC_APB2Periph_GPIOA
#define Trac_4_GPIO_TIM		RCC_APB2Periph_GPIOC
#define Trac_5_GPIO_TIM		RCC_APB2Periph_GPIOC


void Tracing_Init (void);
char * Tracing_Find (void);

#endif
