# Base_ADC

这个大小，每个单片机的差异就会很大，所以这里分为通用、DMA版。

一个ADCx上的通道一般是能同时读多个的（同时配置）。

也就是说，一次AD转换可以得到好几个数据。

### 通用版

![image-20220920144342235](https://raw.githubusercontent.com/SwiperWitty/img/main/img/image-20220920144342235.png)

#### 初始化

- ADC模式我们就采用独立模式【ADC_Mode_Independent】
- 接下来我们会看到是否使用扫描模式【ADC_ScanConvMode】
- 是否使用连续模式【ADC_ContinuousConvMode】，在多通道的时候，我们会考虑扫描模式，就是多通道按照顺序一个个扫描过来，一般的我们会结合DMA一起使用，连续模式就是，在触发一次后是否进行连续读取ADC的值。



~~~C
#define ADC_0 ADC_Channel_0
#define ADC_1 ADC_Channel_1
#define ADC_2 ADC_Channel_2
#define ADC_3 ADC_Channel_3
#define MCU_Temp ADC_Channel_TempSensor
//

char ADC_State = 0;
int Calibrattion_Val = 0;
void ADCx_Init(int ADC_x, int SET)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    FunctionalState temp = DISABLE;
    if (SET) temp = ENABLE;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);		//adC对应GPIO的时钟

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
        #ifdef Use_ADC_DMA
        ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                 
        #else
        ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
        #endif
        ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//ADC_Resolution_12b分辨率
        ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	// ADC数据右对齐
        ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; //不使用外部触发转换  
        ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
        ADC_Init(ADC1, &ADC_InitStructure);
        
        ADC_VrefintCmd(ENABLE);     //使能内部参考电压
        ADC_Cmd(ADC1,ENABLE); 		//使能ADC1
        Calibrattion_Val = ADC_GetCalibrationFactor(ADC1);  //must here
        #ifdef Use_ADC_DMA
        DMA_Configuration(void);
        #endif
        
        ADC_ResetCalibration(ADC1);          
        while(ADC_GetResetCalibrationStatus(ADC1));     
        ADC_StartCalibration(ADC1); 					//校准过程
        while(ADC_GetCalibrationStatus(ADC1)); 
        Calibrattion_Val = Get_CalibrationValue(ADC1);	//获取校准值
    }
}
~~~



#### 切换、获取ADC值

单通道

~~~c
#define ADC_Sample_Time ADC_SampleTime_7_5Cycles
int Get_ADCx_Num (int Channel)
{
    int num = 0;
#ifdef Exist_ADC
//    ADC1->CHSELR = 0;                                   //通道复位  !!!
	ADC_RegularChannelConfig(ADC1, Channel, 1, ADC_Sample_Time);//这里面的【1】很重要！
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);				//使能软件触发
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));		//等待转换完成
    num = ADC_GetConversionValue(ADC1); 				//获取转换值
#endif
    return (num);
}
~~~



多通道

~~~c
char set_channle = 0;
int ADC_Somes_NUM[ADC_CNT];
int Get_ADCx_Num (int Channel)
{
	int num = 0;
#ifdef Exist_ADC
    if(set_channle == 0)
    {
        set_channle = 1;
        ADC_RegularChannelConfig(ADC1, ADC_0, 1, ADC_Sample_Time);
        ADC_RegularChannelConfig(ADC1, ADC_1, 2, ADC_Sample_Time);
        ADC_RegularChannelConfig(ADC1, ADC_2, 3, ADC_Sample_Time);
        ADC_RegularChannelConfig(ADC1, MCU_Temp, 4, ADC_Sample_Time);
    }
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);				//使能软件触发
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));		//等待转换完成
    ADC_Somes_NUM[0] = ADC_GetConversionValue(ADC1);	//获取转换值
	ADC_Somes_NUM[1] = ADC_GetConversionValue(ADC1);	//获取转换值
    ADC_Somes_NUM[2] = ADC_GetConversionValue(ADC1);	//获取转换值
    ADC_Somes_NUM[3] = ADC_GetConversionValue(ADC1);	//获取转换值
	switch (Channel)
    {
        case ADC_0:
            num = ADC_Somes_NUM[0];
            break;

        case MCU_Temp:
            num = ADC_Somes_NUM[3];
            break;
        default:
            break;
    }
#endif
    return (num);
}

~~~



### DMA版

![image-20220920155933139](https://raw.githubusercontent.com/SwiperWitty/img/main/img/image-20220920155933139.png)

#### 初始化

DMA

~~~~c
#define ADC_DMA_CNT 4
int ADC_Somes_NUM[ADC_CNT];
void DMA_Configuration(void) 
{
    /* ADC1  DMA1 Channel Config */  
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); //使能DMA时钟
 
    DMA_DeInit(DMA1_Channel1);   //将DMA的通道1寄存器重设为缺省值：ADC1连接DMA通道1
    DMA_InitStructure.DMA_PeripheralBaseAddr =  (u32)&ADC1->DR;	//DMA外设ADC基地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ADC_Somes_NUM;	//DMA内存基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //内存作为数据传输的目的地
    DMA_InitStructure.DMA_BufferSize = ADC_DMA_CNT;  //此值为完整一轮DMA传输的次数
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址不变
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //外设数据位宽度16位，即DMA传输尺寸
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //数据宽度16位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //工作在循环缓存模式，一轮结束后自动开始下轮传输
    DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMA通道 x拥有高优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x禁止内存到内存
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);  //根据DMA_InitStruct中参数DMA通道
}

~~~~



获取参考——获取多通道



### ADC值转换为电压函数

~~~c
//To obtain the voltage
#define VDDA 3.30
float ADC_Num_to_Vol(int num)
{
	float vol = num;
	vol = (vol / 4095) * VDDA;
	return (vol);
}
~~~

获取温度

~~~c
float Get_MCU_Temp(void)
{
	float temperature = 0;
#ifdef Exist_ADC
    temperature = ADC_Num_to_Vol(Get_ADCx_Num(MCU_Temp));
    temperature = (1.43 - temperature) / 0.0043 + 25;
#endif
	return (temperature);
}
~~~

