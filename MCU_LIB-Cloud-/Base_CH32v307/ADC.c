#include "adc.h"

char ADC_State = 0;
int Calibrattion_Val = 0;

void ADCx_Init(char ADC_x, int SET)
{
#ifdef Exist_ADC
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	ADC_InitTypeDef ADC_InitStructure = {0};

    FunctionalState temp = DISABLE;
    if (SET) temp = ENABLE;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	  // ADC 时钟
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;

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
        case ADC_8:
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
            GPIO_Init(GPIOB, &GPIO_InitStructure);
            break;
        case ADC_9:
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
            GPIO_Init(GPIOB, &GPIO_InitStructure);
            break;

        case MCU_Temp:
            ADC_TempSensorVrefintCmd(temp);
            break;
        default:
            return;
            break;
    }
    if(ADC_State == 0)
    {
        ADC_State = 1;
        RCC_ADCCLKConfig(RCC_PCLK2_Div8);

        ADC_DeInit(ADC1);
        ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                  // ADC工作模式:ADC1和ADC2工作在独立模式
        ADC_InitStructure.ADC_ScanConvMode = DISABLE;                       //模数转换工作在单通道模式
        ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;                  //模数转换工作在单次转换模式
        ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //转换由软件而不是外部触发启动
        ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;              // ADC数据右对齐
        ADC_InitStructure.ADC_NbrOfChannel = 1;                             //顺序进行规则转换的ADC通道的数目
        ADC_Init(ADC1, &ADC_InitStructure);                                 //根据ADC_InitStruct中指定的参数初始化外设ADC_x的寄存器
        ADC_Cmd(ADC1, ENABLE);      //使能指定的ADC1

        ADC_ResetCalibration(ADC1); //使能复位校准
        while (ADC_GetResetCalibrationStatus(ADC1));    //等待复位校准结束
        ADC_StartCalibration(ADC1);                     //开启AD校准
        while (ADC_GetCalibrationStatus(ADC1));         //等待校准结束
        Calibrattion_Val = Get_CalibrationValue(ADC1);  //获取校准值

//        OPA_InitTypeDef OPA_InitStructure;
//        OPA_InitStructure.OPA_NUM = OPA1;
//        OPA_InitStructure.PSEL = CHP1;
//        OPA_InitStructure.NSEL = CHN1;
//        OPA_InitStructure.Mode = OUT_IO_ADC;
//        OPA_Init(&OPA_InitStructure );
//        OPA_Cmd(OPA1,ENABLE);
    }


#endif
}

float Get_ADCx_Vol(uint8_t ADC_x)
{
	float vol = 0;
#ifdef Exist_ADC

	ADC_RegularChannelConfig(ADC1, ADC_x, 1, ADC_De_Time);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //使能指定的ADC1的软件转换启动功能
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
	vol = ADC_GetConversionValue(ADC1); //读取ADC1通道
	vol = (vol / 4095) * 3.3;
#endif
	return vol;
}

float Get_MCU_Temp(void)
{
	float temperate = 0;
#ifdef Exist_ADC
	int temp,Refer_Volt, Refer_Temper;
    ADC_RegularChannelConfig(ADC1, MCU_Temp, 1, ADC_SampleTime_239Cycles5);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE); //使能指定的ADC1的软件转换启动功能
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    temp = ADC_GetConversionValue(ADC1); //读取ADC1通道

    if((temp + Calibrattion_Val) < 0)
        temp = 0;
    else if((Calibrattion_Val + temp) > 4095)
        temp = 4095;
    else
        temp = (temp + Calibrattion_Val);       //  缝合

    temp = (temp * 3300 / 4096);
    Refer_Volt = (int)((*(int *)0x1FFFF720) & 0x0000FFFF);
    Refer_Temper = (int)(((*(int *)0x1FFFF720) >> 16) & 0x0000FFFF);
    temp = (Refer_Volt - temp) * 10 + (43 >> 1);
    temperate = temp / 43.0;
    temperate = Refer_Temper + temperate;
#endif
	return temperate;
}
