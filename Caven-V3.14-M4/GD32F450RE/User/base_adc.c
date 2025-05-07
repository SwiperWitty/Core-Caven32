
#include "base_adc.h"

//static    struct
static uint8_t adc_list[6] = {ADC_CHANNEL_12,ADC_CHANNEL_13,ADC_CHANNEL_8,ADC_CHANNEL_9,};   //PRB_Q PRB_I   RF_FWD_DET  RF_RES_DET

int adc_config(void)
{
  int retval = 0;
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
//    delay_1ms(1);
//    Base_Delay (1000,10);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);
    
    return retval;
}


int adc_channel_sample(uint16_t * data,char num)
{
    int retval = 0;
    if(data == NULL)
    {
        retval = 1;
        return retval;
    }
    for(int i = 0;i < num;i ++)
    {
        if(adc_list[i] == 0)
            return 1;
        /* ADC routine channel config */
        adc_routine_channel_config(ADC0, 0U, adc_list[i], ADC_SAMPLETIME_15);
        /* ADC software trigger enable */
        adc_software_trigger_enable(ADC0, ADC_ROUTINE_CHANNEL);

        /* wait the end of conversion flag */
        while(!adc_flag_get(ADC0, ADC_FLAG_EOC));
        /* clear the end of conversion flag */
        adc_flag_clear(ADC0, ADC_FLAG_EOC);
        /* return regular channel sample value */
        *(data + i) = adc_routine_data_read(ADC0);
    }
    return retval;
}

/*      ...     */
void adc_get_value(unsigned short *i,unsigned short *q)         //i & q
{
  uint16_t adc_array[4];
  adc_channel_sample(adc_array,2);

  *i = adc_array[1];
  *q = adc_array[0];
}

void adc_get_fr_value(unsigned short *FWD,unsigned short *RES)         //FWD & RES
{
  uint16_t adc_array[4];
  adc_channel_sample(adc_array,4);

  *FWD = adc_array[2];
  *RES = adc_array[3];
}

void adc_get_value_i(unsigned short *dat){

}

void adc_get_value_q(unsigned short *dat){

}

void adc_rf_res_det(unsigned short *i){

}
