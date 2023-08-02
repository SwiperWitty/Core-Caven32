#ifndef _BASE__ADC_H
#define _BASE__ADC_H

/*base_adc.h*/

#include "Items.h"

#define PRB_Q GPIO_PIN_2      /*gpioc PC2   ADC0_in12*/
#define PRB_I GPIO_PIN_3      /*gpioc PC3   ADC0_in13*/

#define BION 0      /*gpiox*/
#define BIOP 0      /*gpiox*/

#define RF_FWD_DET GPIO_PIN_0 /*gpioB PB0   ADC0_in8*/
#define RF_RES_DET GPIO_PIN_1 /*gpioB PB1   ADC0_in9*/


#define ADC_MAX                          4096   
#define ADC_VREF                         3.30

int adc_config(void);
int adc_channel_sample(uint16_t * data,char num);

void adc_get_value(unsigned short *i,unsigned short *q);
void adc_get_fr_value(unsigned short *FWD,unsigned short *RES);


#endif

