#ifndef _KEY_H
#define _KEY_H

#define KEY0	GPIO_Pin_10
#define KEY1	GPIO_Pin_11
#define KEY2	GPIO_Pin_13

#include "stm32f10x.h"

void KEY_Init(void);
u16 KEY_Input (void);			//放中断

#endif

