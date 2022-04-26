#ifndef _BZZ_H_
#define _BZZ_H_

#define BZZ         GPIO_Pin_12
#define BZZ_GPIO	GPIOA

#include "stm32f10x.h"


//以下是移植F4需要更改内容
#define BZZ_PeriphClockCmd(SET)		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, SET)	//时钟(F1)
//#define IIC_PeriphClockCmd(SET)	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,SET); //时钟(F4)

#define BZZ_GPIO_Init()									\
do														\
{														\
	GPIO_InitTypeDef GPIO_InitStructure;				\
    BZZ_PeriphClockCmd(SET);                            \
    GPIO_InitStructure.GPIO_Pin = BZZ;	                \
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		\
														\
	GPIO_Init(BZZ_GPIO, &GPIO_InitStructure);			\
}while(0);

#define BZZ_GPIO_Exit()									\
do														\
{														\
	GPIO_InitTypeDef GPIO_InitStructure;				\
    GPIO_InitStructure.GPIO_Pin = BZZ;            \
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		\
	GPIO_Init(BZZ_GPIO, &GPIO_InitStructure);			\
}while(0);

#define BZZ_H() BZZ_GPIO->BSRR = BZZ            //置高电平
#define BZZ_L() BZZ_GPIO->BRR = BZZ             //置低电平

#define BZZ_ON 		1
#define BZZ_OF 		0

void BZZ_Init(FunctionalState SET);
void BZZ_Control (char OUT);

#endif
