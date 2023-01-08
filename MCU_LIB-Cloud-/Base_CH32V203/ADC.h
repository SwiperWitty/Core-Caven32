#ifndef _ADC_H_
#define _ADC_H_

//	可以跨芯片移植	需要大改底层
/*
			ADC_1        PA4  			ADC1		ADC_Channel_4
			ADC_2        PA5  			ADC1		ADC_Channel_4


												--2021.12.04
*/

/*
    SDK->Items->GPIO(Exist_GPIO)->BASE->
                                         \\
                                          -->[ADC]->MODE
                                         //
                    C(Lib)->Caven->API->
*/

#include "Items.h"

#define ADC_0 ADC_Channel_0
#define ADC_1 ADC_Channel_1
#define ADC_2 ADC_Channel_2
#define ADC_3 ADC_Channel_3

#define ADC_8 ADC_Channel_8
#define ADC_9 ADC_Channel_9

#define MCU_Temp ADC_Channel_TempSensor

#define ADC_De_Time ADC_SampleTime_28Cycles5


struct ADC_
{
	float (*Get_Vol)(char ADC_x);
	float (*Get_MCU_Temp)(void);
};

void ADCx_Init(char ADC_x, int SET);
float Get_ADCx_Vol(uint8_t ADC_x);
float Get_MCU_Temp(void);

#endif
