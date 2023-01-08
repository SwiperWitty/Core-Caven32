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
#define MCU_Temp ADC_Channel_TempSensor

#define ADC_Sample_Time ADC_SampleTime_7_5Cycles
#define ADC_DMA_CNT 4

//#define Use_ADC_DMA
#define VDDA 3.30

extern uint16_t ADC_Somes_NUM[];

struct ADC_
{
    int (*Get_ADCx_Num) (int Channel);
    
    float (*ADC_Conversion_Vol) (int Channel);
    float (*ADC_Get_Temperature) (void);
};

void ADCx_Init(int ADC_x, int SET);

int Get_ADCx_Num (int Channel);

float ADC_Conversion_Vol(int num);
float ADC_Get_Temperature(void);

#endif
