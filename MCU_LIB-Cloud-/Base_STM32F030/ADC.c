#include "adc.h"

char ADC_State = 0;
int Calibrattion_Val = 0;
uint16_t ADC_Somes_NUM[2];

void ADCx_Init(int ADC_x, int SET)
{
#ifdef Exist_ADC
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	ADC_InitTypeDef ADC_InitStructure = {0};
    
    FunctionalState temp = DISABLE;
    if (SET) temp = ENABLE;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	  // ADC 时钟
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;        //输出
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽    OD开漏（有阻）
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;    //上下拉
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //高速
    
	switch (ADC_x)
	{
        case ADC_0:
	        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	        GPIO_Init(GPIOA, &GPIO_InitStructure);
	        break;
	    case ADC_1:
	        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	        GPIO_Init(GPIOA, &GPIO_InitStructure);
	        break;

        case ADC_2:
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
            GPIO_Init(GPIOA, &GPIO_InitStructure);
            break;
        case ADC_3:
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
            GPIO_Init(GPIOA, &GPIO_InitStructure);
            break;

        case MCU_Temp:
            ADC_TempSensorCmd(temp);                        //内部温度传感器
            break;
        default:
            break;
    }
    if(ADC_State == 0)
    {
        ADC_State = 1;
        RCC_ADCCLKConfig(RCC_ADCCLK_PCLK_Div4);             //ADC时钟分频
        
        ADC_DeInit(ADC1);
        ADC_StructInit(&ADC_InitStructure);
        ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;              //ADC_Resolution_12b    分辨率
        #ifdef Use_ADC_DMA
        ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                 
        #else
        ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;                 //模数转换工作在单次转换模式
        #endif
        ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;              // ADC数据右对齐
        ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; //不使用外部触发转换  
        ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
        ADC_Init(ADC1, &ADC_InitStructure);
        
        ADC_VrefintCmd(ENABLE);     //使能内部参考电压
        Calibrattion_Val = ADC_GetCalibrationFactor(ADC1);  //must here
        
        #ifdef Use_ADC_DMA
        
        ADC_ChannelConfig(ADC1,ADC_0, ADC_Sample_Time);
        ADC_ChannelConfig(ADC1,ADC_1, ADC_Sample_Time);
        ADC_ChannelConfig(ADC1,ADC_2, ADC_Sample_Time);
        ADC_ChannelConfig(ADC1,MCU_Temp, ADC_Sample_Time);
        
        DMA_InitTypeDef DMA_InitStructure = {0};
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);
        
        DMA_DeInit(DMA1_Channel1);	/* DMA1 Channel1 Config */
        DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1->DR;               //外设地址
        DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_Somes_NUM;              //内存地址
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//外设作为数据传输的来源
        DMA_InitStructure.DMA_BufferSize = ADC_DMA_CNT;//指定DMA通道的DMA缓存的大小
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址寄存器不变
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//内存地址寄存器不变
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//数据宽度为16位
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//数据宽度为16位
        DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
        DMA_InitStructure.DMA_Priority = DMA_Priority_High;//DMA_Priority设定DMA通道x的软件优先级
        DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//DMA通道x没有设置为内存到内存传输
        DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
        DMA_Cmd(DMA1_Channel1, ENABLE);/* DMA1 Channel1 enable */	
        ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);
        ADC_DMACmd(ADC1,ENABLE);		
        #endif
        
        ADC_Cmd(ADC1, ENABLE);      //使能指定的ADC1
        while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_ADEN));      //等待AD使能
        ADC_StartOfConversion(ADC1);
    }

#endif
}

//To obtain the integer 使用这个方式需要加延迟，不然数据不对
int Get_ADCx_Num (int Channel)
{
//    ADC1->CHSELR = 0;
    int num = 0;
#ifdef Exist_ADC
    #ifdef Use_ADC_DMA
    
    while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));

    switch (Channel)
    {
        case ADC_0:
            num = ADC_Somes_NUM[0];
            break;
        case ADC_1:
            num = ADC_Somes_NUM[1];
            break;
        case ADC_2:
            num = ADC_Somes_NUM[2];
            break;
        case MCU_Temp:
            num = ADC_Somes_NUM[3];
            break;
        default:
            break;
    }
    #else
    
    ADC1->CHSELR = 0;                                   //通道复位  !!!
    ADC_ChannelConfig(ADC1,Channel,ADC_Sample_Time);    //通道选择
    
    ADC_StartOfConversion(ADC1);                    //开始
    while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));
    //这个标志位读取之后，他会自动归0
    
    num = ADC_GetConversionValue(ADC1);             //读取
    ADC_StopOfConversion(ADC1);                     //结束
    #endif
    
#endif
    return (num);
}

//To obtain the voltage
float ADC_Conversion_Vol(int num)
{
	float vol = num;

	vol = (vol / 4095) * VDDA;

	return (vol);
}

//To obtain the temperature (℃)  
float ADC_Get_Temperature(void)
{
	float temperature = 0;
#ifdef Exist_ADC
    temperature = ADC_Conversion_Vol(Get_ADCx_Num(MCU_Temp));
    temperature = (1.43 - temperature) / 0.0043 + 25;
#endif
	return (temperature);
}


