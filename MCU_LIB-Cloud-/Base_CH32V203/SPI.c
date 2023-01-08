#include "SPI.h"

int SPI_State[3] = {0};

void SPI1_GPIO_Init(int SET)
{
#if (SPI_X == 1)
    GPIO_InitTypeDef  GPIO_InitStructure;
    if (SET)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   //GPIOA
        GPIO_InitStructure.GPIO_Pin = SPI1_SCK|SPI1_MOSI | SPI1_NSS;
        GPIO_InitStructure.GPIO_Mode = SPI_MODE_OUT;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIO_SPI1, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = SPI1_NSS;                 //NSS 不推荐复用
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIO_SPI1, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = SPI1_MISO;
        GPIO_InitStructure.GPIO_Mode = SPI_MODE_IN;
        GPIO_Init(GPIO_SPI1, &GPIO_InitStructure);

        GPIO_SPI1->BSHR = GPIO_Pin_4;     //  High
    }
    else
    {
        GPIO_InitStructure.GPIO_Pin = SPI1_NSS|SPI1_SCK|SPI1_MOSI|SPI1_MISO;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
        GPIO_Init(GPIO_SPI1, &GPIO_InitStructure);
    }
#endif
}

void SPI2_GPIO_Init(int SET)
{
#if (SPI_X == 2)
    GPIO_InitTypeDef  GPIO_InitStructure;
    if (SET)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   //GPIOB
        GPIO_InitStructure.GPIO_Pin = SPI2_SCK|SPI2_MOSI | SPI2_NSS;
        GPIO_InitStructure.GPIO_Mode = SPI_MODE_OUT;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIO_SPI2, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = SPI2_MISO;
        GPIO_InitStructure.GPIO_Mode = SPI_MODE_IN;
        GPIO_Init(GPIO_SPI2, &GPIO_InitStructure);
    }
    else
    {
        GPIO_InitStructure.GPIO_Pin = SPI2_NSS|SPI2_SCK|SPI2_MOSI|SPI2_MISO;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
        GPIO_Init(GPIO_SPI2, &GPIO_InitStructure);
    }
#endif
}

void SPIx_Init(char Channel,int SET)
{
#ifdef Exist_SPI
    SPI_State[Channel] = SET;
    #ifndef SPI_Software
        SPI_InitTypeDef SPI_InitStructure = {0};
        NVIC_InitTypeDef NVIC_InitStructure = {0};
    #endif
    switch (Channel)
    {
        case 1:
            SPI1_GPIO_Init(SET);
    #ifndef SPI_Software                                    //开始硬件SPI
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE );
        #if (SPI_MODE == HOST_MODE)
            SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
        #elif (SPI_MODE == SLAVE_MODE)
            SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
        #endif
            SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
            SPI_InitStructure.SPI_DataSize = SPI_Size;
            SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;          //空闲时SCK高电平
            SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;        //偶数边沿采样
            SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;   //SPI_NSS_Hard/SPI_NSS_Soft
            SPI_InitStructure.SPI_BaudRatePrescaler = SPI_Speed;  //分频
            SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  //高位先行
            SPI_InitStructure.SPI_CRCPolynomial = 7;
            SPI_Init(SPI1, &SPI_InitStructure );

            NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStructure);
            SPI_SSOutputCmd(SPI1,ENABLE);
            SPI_Cmd(SPI1,ENABLE);
//          SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_TXE,ENABLE);
    #endif
            break;
        case 2:
            SPI2_GPIO_Init(SET);
    #ifndef SPI_Software
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE );   //开始硬件SPI2,API1
            int speed_baud = SPI_Speed;
            if (speed_baud > 0) {
                speed_baud -= 8;                            //可以超频就超频（与SPI1保持同速）
            }
        #if (SPI_MODE == HOST_MODE)
            SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
        #elif (SPI_MODE == SLAVE_MODE)
            SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
        #endif
            SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
            SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
            SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;         //空闲时SCK高电平
            SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;        //偶数边沿采样
            SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;   //SPI_NSS_Hard/SPI_NSS_Soft
            SPI_InitStructure.SPI_BaudRatePrescaler = speed_baud;  //分频
            SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  //高位先行
            SPI_InitStructure.SPI_CRCPolynomial = 7;
            SPI_Init(SPI2, &SPI_InitStructure );

            NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//                NVIC_Init(&NVIC_InitStructure);
            SPI_SSOutputCmd(SPI2, ENABLE);
            SPI_Cmd(SPI2,ENABLE);

            break;
        default:
            break;
    #endif
    }
#endif
}

static void SPI_Delay (int time)
{
    volatile int temp;
    for (int i = 0; i < time; ++i)
    {
        temp = 10;            //SET
        while((temp--) > 0);
    }
}

void SPI_CS_Set(char Serial,char State)
{
#if (SPI_X == 1)
    switch (Serial)
    {
        case 1:
            if (State) {
                GPIO_SPI1->BCR = GPIO_Pin_4;      //  Low
            }
            else {
                GPIO_SPI1->BSHR = GPIO_Pin_4;     //  High
            }
            break;
        case 2:

            break;
        default:
            break;
    }
#endif
#if (SPI_X == 2)
    switch (Serial)
    {
        case 1:
            if (State) {
                GPIO_SPI2->BCR = GPIO_Pin_12;      //  Low
            }
            else {
                GPIO_SPI2->BSHR = GPIO_Pin_12;     //  High
            }

            break;
        default:
            break;
    }
#endif

}

void SPI1_Send_DATA(char Serial,const uint16_t DATA)
{
    /*
     * 写标志位
     * 写数据
     * 等忙位
     */
#if (SPI_X == 1)
    #if(SPI_MODE == HOST_MODE)
//    SPI_CS_Set(Serial,ENABLE);      //SPI开始（片选）   // not
    #ifdef SPI_Software
    char temp;
    for (int i = 0; i < 8; i++)
    {
        SPI1_SCK_L();           //预备上升沿
        temp = (DATA << i) & 0x80;
        if (temp)
            SPI1_MOSI_H();      //数据1
        else
            SPI1_MOSI_L();      //数据0
        SPI1_SCK_H();           //完成上升沿
    }
    SPI1_MOSI_H();
    #else
        while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE) == RESET); //检查发送是否完成，完成以后再发送数据
        SPI_I2S_SendData(SPI1, DATA);
        while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY) != RESET); //必要
    #endif
//    SPI_CS_Set(Serial,DISABLE);               //SPI结束
    #endif
#endif
}

void SPI2_Send_DATA(char Serial,const uint16_t DATA)
{
#if (SPI_X == 2)
    #if(SPI_MODE == HOST_MODE)

    #ifdef SPI_Software
//    SPI_CS_Set(Serial,ENABLE);      //SPI开始（片选）
    char temp;
    for (int i = 0; i < 8; i++)
    {
        SPI2_SCK_L();           //预备上升沿
        temp = (DATA << i) & 0x80;
        if (temp)
            SPI2_MOSI_H();      //数据1
        else
            SPI2_MOSI_L();      //数据0
        SPI2_SCK_H();           //完成上升沿
    }
    SPI2_MOSI_H();
//    SPI_CS_Set(Serial,DISABLE);               //SPI结束
    #else
        while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE) == RESET); //检查发送是否完成，完成以后再发送数据
        SPI_I2S_SendData(SPI2, DATA);
        while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_BSY) != RESET); //必要
    #endif

    #endif
#endif
}



void SPI_SET_Addr_SendData(char Serial,uint16_t Addr,uint16_t DATA)
{
    Addr &= 0xBFFF;

#if (SPI_X == 1)
    SPI_CS_Set(Serial,ENABLE);      //SPI开始（片选）
    SPI_Delay (1);
    SPI1_Send_DATA(Serial,Addr | 0xC000);       //满足某种协议
//    SPI_Delay (1);
    SPI1_Send_DATA(Serial,DATA);
    SPI_Delay (1);
    SPI_CS_Set(Serial,DISABLE);                 //SPI结束
#elif (SPI_X == 2)
    SPI2_Send_DATA(Serial,Addr);
    SPI2_Send_DATA(Serial,DATA);

#endif
}

uint16_t SPI_SET_Addr_ReadData(char Serial,uint16_t Addr)
{
    /*
     * 等能读
     * 读
     *
     */
    uint16_t temp = 0;
    Addr &= 0xBFFF;
#ifndef SPI_Software
    #if (SPI_X == 1)
    SPI_CS_Set(Serial,ENABLE);      //SPI开始（片选）
    SPI_Delay (1);
    SPI1_Send_DATA(Serial,Addr | 0x8000);

    SPI1_Send_DATA(Serial,0);
    while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE) == RESET);
    temp = SPI_I2S_ReceiveData(SPI1);

    SPI1_Send_DATA(Serial,0);
    while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE) == RESET);
    temp = SPI_I2S_ReceiveData(SPI1);
    SPI_Delay (1);
    SPI_CS_Set(Serial,DISABLE);     //SPI结束
    #elif (SPI_X == 2)
    SPI2_Send_DATA(Serial,Addr);
    SPI2_Send_DATA(Serial,0);
    while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_RXNE) == RESET);
    temp = SPI_I2S_ReceiveData(SPI2);
    #endif
#endif


    return temp;
}

