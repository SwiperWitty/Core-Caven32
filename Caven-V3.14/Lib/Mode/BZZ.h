#ifndef _BZZ_H_
#define _BZZ_H_

#include "stm32f10x.h"

#define BZZ 		GPIO_Pin_12
#define BZZ_GPIO	GPIOA
#define BZZ_GPIO_TIM	RCC_APB2Periph_GPIOA

#define BZZ_ON 		1
#define BZZ_OF 		0

void BZZ_Init(void);
void BZZ_Control (char OUT);

#endif
