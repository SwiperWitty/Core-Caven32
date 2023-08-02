#ifndef _BASE__ADC_H
#define _BASE__ADC_H

/*base_adc.h*/

#include "gd32f4xx.h"
#include <stdio.h>

#define PRB_Q GPIO_PIN_2      /*gpioc PC2*/
#define PRB_I GPIO_PIN_3      /*gpioc PC3*/

#define BION 0      /*gpioc*/
#define BIOP 0      /*gpioc*/

#define RF_FWD_DET GPIO_PIN_0 /*gpioB PB0*/
#define RF_RES_DET GPIO_PIN_1 /*gpioB PB1*/

void adc_config(void);
int adc_channel_sample(uint16_t * data,char num);


#endif

