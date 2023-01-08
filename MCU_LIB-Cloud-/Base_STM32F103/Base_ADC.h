#ifndef BASE_ADC__H_
#define BASE_ADC__H_

#ifdef DEFAULT
#include "items.h"              //默认功能
#endif

#ifndef DEFAULT
#include "User_items.h"         //自行设置功能，一般出现在本地文件的User中
#endif

/****************/


#define ADC_VREF                         (3.3)
#define ADC_TEMP_BASE                    (1.26)
#define ADC_TEMP_SLOPE                   (-0.00423)


/*
    使用DMA来读ADC的好处是一次可以获取所有的数据。
    而且快速
*/

#define ADC_Speed ADC_SAMPLETIME_239_5

#define ADC_IO_PA0  ADC_CHANNEL_0
#define ADC_IO_PA1  ADC_CHANNEL_1
#define ADC_IO_PA2  ADC_CHANNEL_2
// #define ADC_IO_PA3  ADC_CHANNEL_3
// #define ADC_IO_PA4  ADC_CHANNEL_4
// #define ADC_IO_PA5  ADC_CHANNEL_5
// #define ADC_IO_PA6  ADC_CHANNEL_6
// #define ADC_IO_PA7  ADC_CHANNEL_7
#define ADC_IO_PB0  ADC_CHANNEL_8
#define ADC_IO_PB1  ADC_CHANNEL_9
#define ADC_Temp    ADC_CHANNEL_16      //温传


void ADC_Start_Init(int Set);
void ADC_Get_List(int *Target);
float ADC_Get_Temperature(void);

#endif 
