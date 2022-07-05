#include "adc.h"
#include "sys_time.h"

struct ADC_ ADC;

void ADC_x_Init(char ADC_x, FunctionalState SET)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	ADC_InitTypeDef ADC_InitStructure;
	RCC_ADCCLKConfig(RCC_PCLK2_Div6); //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  //复用时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // GPIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	  // ADC 时钟
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		  /* 配置中断优先级分组(设置抢占优先级和子优先级的分配)，在函数在misc.c */

	ADC.ADC_x_Read_Vol = ADC_x_Read_Vol;
	ADC.Read_MCU_Temp = Read_MCU_Temp;
	//	ADC_DeInit(ADC1);  //将外设 ADC1 的全部寄存器重设为缺省值
	if (ADC_x == ADC_1)
	{
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(GPIOA, &GPIO_InitStruct);
	}
	else if (ADC_x == ADC_2)
	{
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(GPIOA, &GPIO_InitStruct);
	}
	else if (ADC_x == MCU_Temp)
		ADC_TempSensorVrefintCmd(SET);
	else
		return;

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;					// ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;						//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;					//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;				// ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;								//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);									//根据ADC_InitStruct中指定的参数初始化外设ADC_x的寄存器
	if (SET)
	{
		ADC_Cmd(ADC1, ENABLE);		//使能指定的ADC1
		ADC_ResetCalibration(ADC1); //使能复位校准
		Delay_ms(20);
		while (ADC_GetResetCalibrationStatus(ADC1))
			;						//等待复位校准结束
		ADC_StartCalibration(ADC1); //开启AD校准
		while (ADC_GetCalibrationStatus(ADC1))
			; //等待校准结束

		ADC_SoftwareStartConvCmd(ADC1, ENABLE); //使能指定的ADC1的软件转换启动功能
	}
}

float ADC_x_Read_Vol(char ADC_x)
{
	float vol = 0;
	ADC_RegularChannelConfig(ADC1, ADC_x, 1, ADC_De_Time);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //使能指定的ADC1的软件转换启动功能
	Delay_ms(1);
	while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
		; //等待转换结束
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	vol = ADC1->DR; //读取ADC1通道
	vol = (vol / 4096) * 3.3;
	return vol;
}

float Read_MCU_Temp(void)
{
	float temperate;
	temperate = ADC_x_Read_Vol(MCU_Temp);		//读取通道16
	temperate = (1.43 - temperate) / 0.43 + 25; //转换为温度值
	if (temperate < 0)
	{
		temperate = -temperate;
	}
	return temperate;
}
