#include "Base_adc.h"


int Channel_NUM;
__IO uint16_t dma_trans_complete_flag = 0;
uint16_t ADC1_valuetab_list[20];

void ADC_GPIO_Init(int Set)
{
#ifdef Exist_ADC
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    if (Set == 0)
    {
        return ;
    }
    
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);  //你都开ADC了，你还在乎省不省电？

    gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init_struct.gpio_pins = 0;                 //清除

    #ifdef ADC_IO_PA0
    gpio_init_struct.gpio_pins |= GPIO_PINS_0;
    #endif
    #ifdef ADC_IO_PA1
    gpio_init_struct.gpio_pins |= GPIO_PINS_1;
    #endif
    #ifdef ADC_IO_PA2
    gpio_init_struct.gpio_pins |= GPIO_PINS_2;
    #endif
    #ifdef ADC_IO_PA3
    gpio_init_struct.gpio_pins |= GPIO_PINS_3;
    #endif
    #ifdef ADC_IO_PA4
    gpio_init_struct.gpio_pins |= GPIO_PINS_4;
    #endif
    #ifdef ADC_IO_PA5
    gpio_init_struct.gpio_pins |= GPIO_PINS_5;
    #endif
    #ifdef ADC_IO_PA6
    gpio_init_struct.gpio_pins |= GPIO_PINS_6;
    #endif
    #ifdef ADC_IO_PA7
    gpio_init_struct.gpio_pins |= GPIO_PINS_7;
    #endif
    gpio_init(GPIOA, &gpio_init_struct);

    gpio_init_struct.gpio_pins = 0;                 //清除
    #ifdef ADC_IO_PB0
    gpio_init_struct.gpio_pins |= GPIO_PINS_0;
    #endif
    #ifdef ADC_IO_PB1
    gpio_init_struct.gpio_pins |= GPIO_PINS_1;
    #endif
    gpio_init(GPIOB, &gpio_init_struct);

#endif
}

void ADC1_DMA_Config (const void *DMA_Buffer,int BufferSize)
{
#ifdef Exist_ADC
    dma_init_type dma_init_struct;
    dma_default_para_init(&dma_init_struct);
    crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
    nvic_irq_enable(DMA1_Channel1_IRQn, 0, 0);
    dma_reset(DMA1_CHANNEL1);

    dma_init_struct.buffer_size = BufferSize;
    dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_base_addr = (uint32_t)DMA_Buffer;
    dma_init_struct.peripheral_base_addr = (uint32_t)&(ADC1->odt);
    dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
    dma_init_struct.memory_inc_enable = TRUE;                   //目标地址递增
    dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
    dma_init_struct.peripheral_inc_enable = FALSE;              //固定寄存器地址
    dma_init_struct.priority = DMA_PRIORITY_HIGH;
    dma_init_struct.loop_mode_enable = TRUE;
    dma_init(DMA1_CHANNEL1, &dma_init_struct);

    dma_interrupt_enable(DMA1_CHANNEL1, DMA_FDT_INT, TRUE);
    dma_channel_enable(DMA1_CHANNEL1, TRUE);

#endif
}

void ADC_Start_Init(int Set)
{
#ifdef Exist_ADC
    adc_base_config_type adc_base_struct;
    crm_periph_clock_enable(CRM_ADC1_PERIPH_CLOCK, TRUE);
    crm_adc_clock_div_set(CRM_ADC_DIV_6);                       //ADC是时钟的6分频
    ADC_GPIO_Init(Set);                                         //GPIO

    int list = 1;
    #ifdef ADC_IO_PA0
    adc_ordinary_channel_set(ADC1, ADC_IO_PA0, list++, ADC_Speed);
    #endif
    #ifdef ADC_IO_PA1
    adc_ordinary_channel_set(ADC1, ADC_IO_PA1, list++, ADC_Speed);
    #endif
    #ifdef ADC_IO_PA2
    adc_ordinary_channel_set(ADC1, ADC_IO_PA2, list++, ADC_Speed);
    #endif
    #ifdef ADC_IO_PA3
    adc_ordinary_channel_set(ADC1, ADC_IO_PA3, list++, ADC_Speed);
    #endif
    #ifdef ADC_IO_PA4
    adc_ordinary_channel_set(ADC1, ADC_IO_PA4, list++, ADC_Speed);
    #endif
    #ifdef ADC_IO_PA5
    adc_ordinary_channel_set(ADC1, ADC_IO_PA5, list++, ADC_Speed);
    #endif
    #ifdef ADC_IO_PA6
    adc_ordinary_channel_set(ADC1, ADC_IO_PA6, list++, ADC_Speed);
    #endif
    #ifdef ADC_IO_PA7
    adc_ordinary_channel_set(ADC1, ADC_IO_PA7, list++, ADC_Speed);
    #endif
    #ifdef ADC_IO_PB0
    adc_ordinary_channel_set(ADC1, ADC_IO_PB0, list++, ADC_Speed);
    #endif
    #ifdef ADC_IO_PB1
    adc_ordinary_channel_set(ADC1, ADC_IO_PB1, list++, ADC_Speed);
    #endif
    #ifdef ADC_Temp
    adc_ordinary_channel_set(ADC1, ADC_Temp, list++, ADC_Speed);
    adc_tempersensor_vintrv_enable(TRUE);                           //开温传
    #endif

    Channel_NUM = list - 1;
    ADC1_DMA_Config (ADC1_valuetab_list, Channel_NUM);

    adc_base_default_para_init(&adc_base_struct);
    adc_base_struct.sequence_mode = TRUE;
    adc_base_struct.repeat_mode = FALSE;                    //  不需要重复
    adc_base_struct.data_align = ADC_RIGHT_ALIGNMENT;
    adc_base_struct.ordinary_channel_length = Channel_NUM;            //  Channel_NUM
    adc_base_config(ADC1, &adc_base_struct);
    adc_ordinary_conversion_trigger_set(ADC1, ADC12_ORDINARY_TRIG_SOFTWARE, TRUE);  //普通通道组转换的触发模式使能及触发事件选择为软件
    adc_dma_mode_enable(ADC1, TRUE);                //ADC  DMA
    
    adc_enable(ADC1, TRUE);
    adc_calibration_init(ADC1);                         //校准
    while(adc_calibration_init_status_get(ADC1));       //等待复位校准结束
    adc_calibration_start(ADC1);                        //开启AD校准
    while(adc_calibration_status_get(ADC1));
#endif
}


void ADC_Get_List(int *Target)
{
    if (Channel_NUM > 18)
    {
        return ;
    }
    for (int i = 0; i < Channel_NUM; i++)
    {
        ADC1_valuetab_list[i] = 0;
    }
    
    adc_ordinary_software_trigger_enable(ADC1, TRUE);
    while(dma_trans_complete_flag == 0);
    dma_trans_complete_flag = 0;

    for (int i = 0; i < Channel_NUM; i++)
    {
        Target[i] = ADC1_valuetab_list[i];
    }

}

float ADC_Get_Temperature(void)
{
    float Temp = 0;
#ifdef ADC_Temp
    Temp = ADC1_valuetab_list[Channel_NUM-1];                                     //温传是最后一个
    Temp = (ADC_TEMP_BASE - Temp * ADC_VREF / 4096) / ADC_TEMP_SLOPE + 25;
#endif
    return Temp;
}

void DMA1_Channel1_IRQHandler(void)
{
    if(dma_flag_get(DMA1_FDT1_FLAG) != RESET)
    {
        dma_flag_clear(DMA1_FDT1_FLAG);
        dma_trans_complete_flag = 1;
    }
}

