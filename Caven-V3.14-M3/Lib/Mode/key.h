#ifndef _KEY_H
#define _KEY_H

#define KEY_Touch	GPIO_Pin_13
#define KEY_GPIO	GPIOC

#include "stm32f10x.h"


//以下是移植F4需要更改内容
#define KEY_PeriphClockCmd(SET)		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, SET)	//时钟(F1)
//#define IIC_PeriphClockCmd(SET)	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,SET); //时钟(F4)

#define KEY_GPIO_Init()									\
do														\
{														\
	GPIO_InitTypeDef GPIO_InitStructure;				\
    KEY_PeriphClockCmd(SET);                            \
    GPIO_InitStructure.GPIO_Pin = KEY_Touch;	        \
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		\
														\
	GPIO_Init(KEY_GPIO, &GPIO_InitStructure);			\
}while(0);

#define KEY_GPIO_Exit()									\
do														\
{														\
	GPIO_InitTypeDef GPIO_InitStructure;				\
    GPIO_InitStructure.GPIO_Pin = KEY_Touch;            \
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		\
	GPIO_Init(KEY_GPIO, &GPIO_InitStructure);			\
}while(0);

#define KEY_Touch_IN()  KEY_GPIO->IDR &KEY_Touch        //读取引脚电平

void KEY_Init(FunctionalState SET);
// u16 KEY_Input (void);			//放中断

#endif

