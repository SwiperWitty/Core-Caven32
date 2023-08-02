
#if 0

#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "gd32f450i_eval.h"
#include "string.h"


uint16_t array[64];
uint16_t adc_value[4];

#define PRB_Q GPIO_PIN_2      /*gpioc PC2*/
#define PRB_I GPIO_PIN_3      /*gpioc PC3*/

#define BION 0      /*gpioc*/
#define BIOP 0      /*gpioc*/

#define RF_FWD_DET GPIO_PIN_0 /*gpioB PB0*/
#define RF_RES_DET GPIO_PIN_1 /*gpioB PB1*/

void adc_config(void)
{
    /* enable GPIOA clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC0);
    /* config ADC clock */
    adc_clock_config(ADC_ADCCK_PCLK2_DIV8);
    
    /* config the GPIO as analog mode */
    gpio_mode_set(GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, PRB_Q | PRB_I);
    gpio_mode_set(GPIOB, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, RF_FWD_DET | RF_RES_DET);

    /* reset ADC */
    adc_deinit();
    /* ADC mode config */
    adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);
    /* ADC contineous function disable */
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, DISABLE);
    /* ADC scan mode disable */
    adc_special_function_config(ADC0, ADC_SCAN_MODE, DISABLE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);

    /* ADC channel length config */
    adc_channel_length_config(ADC0,ADC_ROUTINE_CHANNEL,1);
    
    /* ADC routine channel config to dma*/
//    adc_routine_channel_config(ADC0,0,ADC_CHANNEL_12,ADC_SAMPLETIME_144);
//    adc_routine_channel_config(ADC0,1,ADC_CHANNEL_13,ADC_SAMPLETIME_144);
//    adc_routine_channel_config(ADC0,2,ADC_CHANNEL_8 ,ADC_SAMPLETIME_144);
//    adc_routine_channel_config(ADC0,3,ADC_CHANNEL_9 ,ADC_SAMPLETIME_144);

    /* ADC trigger config */
    adc_external_trigger_source_config(ADC0, ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_ROUTINE_T0_CH0); 
    adc_external_trigger_config(ADC0, ADC_ROUTINE_CHANNEL, EXTERNAL_TRIGGER_DISABLE);

    /* enable ADC interface */
    adc_enable(ADC0);
    delay_1ms(1);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);
}

int adc_channel_sample(uint16_t * data,char num)
{
    uint8_t list[6] = {ADC_CHANNEL_12,ADC_CHANNEL_13,ADC_CHANNEL_8,ADC_CHANNEL_9};
    
    for(int i = 0;i < num;i ++)
    {
        if(list[i] == 0)
            return 1;
        /* ADC routine channel config */
        adc_routine_channel_config(ADC0, 0U, list[i], ADC_SAMPLETIME_15);
        /* ADC software trigger enable */
        adc_software_trigger_enable(ADC0, ADC_ROUTINE_CHANNEL);

        /* wait the end of conversion flag */
        while(!adc_flag_get(ADC0, ADC_FLAG_EOC));
        /* clear the end of conversion flag */
        adc_flag_clear(ADC0, ADC_FLAG_EOC);
        /* return regular channel sample value */
        *(data + i) = adc_routine_data_read(ADC0);
    }
    return 0;
}

/*!
    \brief    main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    systick_config();
    adc_config();
    
    while(1) 
    {
        adc_channel_sample(adc_value, 4);
        
        delay_1ms(1);
    }
}

#endif
