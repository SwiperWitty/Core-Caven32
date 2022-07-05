#ifndef _ADC_H_
#define _ADC_H_

//	可以跨芯片移植	需要大改底层
/*
			ADC_1        PA4  			ADC1		ADC_Channel_4
			ADC_2        PA5  			ADC1		ADC_Channel_4


												--2021.12.04
	底层
*/

#include "stm32f10x.h"

#define ADC_1 ADC_Channel_4
#define ADC_2 ADC_Channel_5
#define MCU_Temp ADC_Channel_16

#define ADC_De_Time ADC_SampleTime_239Cycles5

struct ADC_
{
	float (*ADC_x_Read_Vol)(char ADC_x);
	float (*Read_MCU_Temp)(void);
};

void ADC_x_Init(char ADC_x, FunctionalState SET);
float ADC_x_Read_Vol(char ADC_x);
float Read_MCU_Temp(void);

#endif
