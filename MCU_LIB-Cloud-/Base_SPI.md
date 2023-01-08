# Base_SPI

~~~c
/*
			SCK    ->  
                        /----\
            MISO   ->   |SPIx|    <-    NSS/NSS2  (Serial) 
                        \----/
            MOSI   ->
*/
~~~

SPI一般系统只会使用一个（一主多从）。

作为从机，也只会使用一个（一从多主）。

当然有可能因为布线的原因，SPI变成了一对一。

所以，Base_SPI只有**一个SPI**接口（控制多个从机\响应多个主机）。

但是可以主动选择使用哪个SPI。

![image-20220917180312415](https://raw.githubusercontent.com/SwiperWitty/img/main/img/image-20220917180312415.png)

### 软\硬切换

~~~c
#define SPI_Software	//屏蔽就是硬件模式

#ifdef SPI_Software		//软件模式下的GPIO配置
    #define SPI_MODE_IN    GPIO_Mode_OUT
    #define SPI_MODE_OUT   GPIO_Mode_OUT
#else                   //硬件SPI模式下的GPIO模式配置
    #define SPI_MODE_IN    GPIO_Mode_OUT
    #define SPI_MODE_OUT   GPIO_Mode_AF
    #define SPI_Speed   SPI_BaudRatePrescaler_8		//16-9MHZ   8-18MHZ     4-36MHZ     2-72MHZ
    #define SPI_Size    SPI_DataSize_8b				//8b   16b
#endif
~~~



### 硬件模式下是否启动DMA

~~~C
#define SPI_DMA			//屏蔽就是普通模式
~~~

![image-20220929090208550](https://raw.githubusercontent.com/SwiperWitty/img/main/img/image-20220929090208550.png)

### 初始化

​	**GPIO（可以切换SPI1、SPI2）**

~~~c
#define SPI_X   1

#if (SPI_X == 1)
#define SPI1_NSS        GPIO_Pin_4      //(CS)
#define SPI1_SCK        GPIO_Pin_5
#define SPI1_MISO       GPIO_Pin_6
#define SPI1_MOSI       GPIO_Pin_7
#define GPIO_SPI1       GPIOA

#define SPI1_SCK_H()  GPIO_SPI1->BSRR = SPI1_SCK		//置高电平
#define SPI1_SCK_L()  GPIO_SPI1->BRR = SPI1_SCK 		//置低电平
#define SPI1_MOSI_H() GPIO_SPI1->BSRR = SPI1_MOSI
#define SPI1_MOSI_L() GPIO_SPI1->BRR = SPI1_MOSI

#define SPI1_MISO_IN() GPIO_SPI1->IDR & SPI1_MISO      //读取引脚电平

//SPI2
#elif (SPI_X == 2)

#endif

void SPI1_GPIO_Init(int SET)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    if (SET)
    {
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = SPI1_SCK | SPI1_MOSI;
        GPIO_InitStructure.GPIO_Mode = SPI_MODE_OUT;        //输出模式（备用）
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //PP推挽    OD开漏（有阻）
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;    //上下拉
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //高速
        GPIO_Init(GPIO_SPI1, &GPIO_InitStructure);
        
        GPIO_InitStructure.GPIO_Pin = SPI1_NSS;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;       //输出模式（输出）
        GPIO_Init(GPIO_SPI1, &GPIO_InitStructure);
        
        GPIO_InitStructure.GPIO_Pin = SPI1_MISO;
        GPIO_InitStructure.GPIO_Mode = SPI_MODE_IN;
        GPIO_Init(GPIO_SPI1, &GPIO_InitStructure);

    }
    else
    {
        GPIO_InitStructure.GPIO_Pin = SPI1_NSS|SPI1_SCK|SPI1_MOSI|SPI1_MISO;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;        //模拟输入
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;    //上下拉
        GPIO_Init(GPIO_SPI1, &GPIO_InitStructure);
    }
}
~~~

​	**SPI**

~~~C
void SPI2_DMA_Config (uint8_t *DMA_TX_Buffer,int BufferSize)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA1,ENABLE);
    DMA_InitType  DMA_InitStructure;
    NVIC_InitType NVIC_InitStructure;

    DMA_Reset(SPI_Tx_DMA_Channel);
    DMA_DefaultInitParaConfig(&DMA_InitStructure);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI2->DT;     //外设地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)DMA_TX_Buffer; //内存地址
    DMA_InitStructure.DMA_BufferSize = BufferSize;      //长度
    DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALDST;    //传输方向，从内存到外设
    DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;    //外设地址不变
    DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE; //内存地址每次加1
    DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_BYTE;   //字节传输增量
    DMA_InitStructure.DMA_MemoryDataWidth = DMA_MEMORYDATAWIDTH_BYTE;   //字节传输宽度
    DMA_InitStructure.DMA_Mode = DMA_MODE_NORMAL;           //非循环模式
    DMA_InitStructure.DMA_Priority = DMA_PRIORITY_MEDIUM;   //设置优先级--中
    DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;      //不是内存到内存
    DMA_Init(SPI_Tx_DMA_Channel, &DMA_InitStructure);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    SPI_I2S_DMAEnable(SPI2,SPI_I2S_DMA_TX, ENABLE);
}
~~~



~~~~C
#define SPI_X   1
#define HOST_MODE
#define SPI_Software	//屏蔽就是硬件模式

void SPI_Start_Init(int Set)
{
#ifdef Exist_SPI
    FunctionalState set = DISABLE;
    if (Set)
        set = ENABLE;
    #if (SPI_X == 1)
        SPI1_GPIO_Init(Set);
    #elif (SPI_X == 2)
        SPI2_GPIO_Init(Set);
    #endif
    
    #ifndef SPI_Software
        #if (SPI_X == 1)
        SPI_InitType SPI_InitStructure = {0};
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_SPI1, ENABLE);
        
            #ifdef HOST_MODE
            SPI_InitStructure.SPI_Mode = SPI_MODE_MASTER;
            #else
            SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
            #endif
        SPI_InitStructure.SPI_TransMode = SPI_TRANSMODE_FULLDUPLEX;
        SPI_InitStructure.SPI_FrameSize = SPI_Size;
        SPI_InitStructure.SPI_CPOL = SPI_CPOL_HIGH;          //空闲时SCK高电平
        SPI_InitStructure.SPI_CPHA = SPI_CPHA_2EDGE;        //偶数边沿采样
        SPI_InitStructure.SPI_NSSSEL = SPI_NSSSEL_SOFT;   //SPI_NSS_Hard/SPI_NSS_Soft
        SPI_InitStructure.SPI_MCLKP = SPI_Speed;  //分频
        SPI_InitStructure.SPI_FirstBit = SPI_FIRSTBIT_MSB;  //高位先行
        SPI_InitStructure.SPI_CPOLY = 7;
        SPI_Init(SPI1, &SPI_InitStructure);

//      SPI_SSOutputCmd(SPI1,ENABLE);

        SPI_Enable(SPI1,set);
        #endif
        #if (SPI_X == 2)
        SPI_InitType SPI_InitStructure = {0};
        RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_SPI2, ENABLE);
        
            #ifdef HOST_MODE
            SPI_InitStructure.SPI_Mode = SPI_MODE_MASTER;
            #else
            SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
            #endif
        SPI_InitStructure.SPI_TransMode = SPI_TRANSMODE_FULLDUPLEX;
        SPI_InitStructure.SPI_FrameSize = SPI_Size;
        SPI_InitStructure.SPI_CPOL = SPI_CPOL_HIGH;         //空闲时SCK高电平
        SPI_InitStructure.SPI_CPHA = SPI_CPHA_2EDGE;        //偶数边沿采样
        SPI_InitStructure.SPI_NSSSEL = SPI_NSSSEL_SOFT;     //SPI_NSS_Hard/SPI_NSS_Soft
        SPI_InitStructure.SPI_MCLKP = SPI_Speed;  //分频
        SPI_InitStructure.SPI_FirstBit = SPI_FIRSTBIT_MSB;  //高位先行
        SPI_InitStructure.SPI_CPOLY = 7;
        SPI_Init(SPI2, &SPI_InitStructure);
//        SPI_NSSHardwareOutputEnable(SPI2,ENABLE);
        #ifdef SPI_DMA
        NVIC_InitType NVIC_InitStructure;
        RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA1,ENABLE);
        NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

        NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
        SPI_I2S_DMAEnable(SPI2,SPI_I2S_DMA_TX, ENABLE);
        DMA_Reset(SPI_Tx_DMA_Channel);
        SPI2_DMA_Config (SPI_DMA_RX_Buffer,SPI_BufferSize);     //固定内存和长度（长度随时可变）
        DMA_INTConfig(SPI_Tx_DMA_Channel,DMA_INT_TC,ENABLE);

        #endif
        SPI_Enable(SPI2,set);

        #endif

    #endif
#endif
}
~~~~



中断

~~~~c
#ifdef SPI_DMA
void DMA1_Channel5_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_FLAG_TC5) == SET)   //DMA发完了
    {
        DMA_ChannelEnable(SPI_Tx_DMA_Channel,DISABLE);          //停止DMA_5
        while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_BUSY) != RESET);
        SPI_CS_Set(1,DISABLE);                                  //取消片选
        DMA_ClearITPendingBit(DMA1_FLAG_TC5);
    }
}
#endif
~~~~



### 发送/接收

​	**必要的延迟**

~~~c
static void SPI_Delay (int time)
{
    #if MCU_SYS_Freq >= 72000000 
    volatile int temp;
    for (int i = 0; i < time; ++i)
    {
        temp = 10;            //SET
        while((temp--) > 0);
    }
    #else
    while((time--) > 0);
    #endif
}
~~~



​	**片选NSS**

~~~c
void SPI_CS_Set(char Serial,char State)
{
#if (SPI_X == 1)
    switch (Serial)
    {
        case 1:
            if (State) {
                GPIOA->BRR = SPI1_NSS;      //  Low
            }
            else {
                GPIOA->BSRR = SPI1_NSS;     //  High
            }
            break;
        case 2:

            break;
        default:
            break;
    }
#endif
#if (SPI_X == 2)

#endif

}
~~~



​	**SPI发送**

基础层      

~~~c
void SPI1_Send_DATA(const uint16_t DATA)	//发送数据，不考虑片选
{
    /*
     * 写标志位
     * 写数据
     * 等忙位
     */
    
#if(SPI_MODE == HOST_MODE)

    #ifdef SPI_Software
    char temp;
    for (int i = 0; i < 8; i++)
    {
        SPI1_SCK_H();           //预备上升沿
        temp = (DATA << i) & 0x80;
        if (temp)
            SPI1_MOSI_H();      //数据1
        else
            SPI1_MOSI_L();      //数据0
        SPI1_SCK_L();           //完成上升沿
    }
    SPI1_MOSI_H();
    SPI1_SCK_H();
    #else
    while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE) == RESET); //检查发送是否完成，完成以后再发送数据
    SPI_SendData8(SPI1, DATA);
    while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY) != RESET); //必要
    #endif

#endif
}
~~~



调用层      

~~~c
void SPI_SET_Addr_SendData(char Serial,int Addr,int *DATA)
{
    Addr &= 0xBFFF;
    SPI_CS_Set(Serial,ENABLE);      //SPI开始（片选）
    SPI_Delay (1);
#if (SPI_X == 1)
    SPI1_Send_DATA(Addr);
//    SPI_Delay (1);
    SPI1_Send_DATA(DATA);

#elif (SPI_X == 2)


#endif
    SPI_Delay (1);
    SPI_CS_Set(Serial,DISABLE);     //SPI结束
}
~~~





​	**SPI接收**

调用层      

- 发需要读取的地址，等待发送完成（非发送数据为空）。

- 清除读Flag（有些读一遍接收寄存器就自动清除），发一个脉冲，等GX_IC回第一个数据，此数据是向主机返回读的地址（先让主机确认一遍）。

  

  ![image-20220920143445002](https://raw.githubusercontent.com/SwiperWitty/img/main/img/image-20220920143445002.png)

~~~c
int SPI_SET_Addr_ReadData(char Serial,int Addr,int *Back_DATA)
{
    /*
     * 等能读
     * 读
     */
    int temp = 0;
    Addr &= 0xBFFF;
#ifdef SPI_Software
//暂无
#else
    SPI_CS_Set(Serial,ENABLE);      //SPI开始（片选）
    SPI_Delay (1);
    #if (SPI_X == 1)
    SPI1_Send_DATA(Addr);
    
	SPI_ReceiveData8(SPI1);			//以读取的方式，清除接收缓冲区
    SPI1_Send_DATA(0);
    while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE) == RESET);
    *Back_DATA = SPI_ReceiveData8(SPI1);

    #elif (SPI_X == 2)

    #endif
    SPI_Delay (1);
    SPI_CS_Set(Serial,DISABLE);     //SPI结束
#endif
    return (temp);
}
~~~

