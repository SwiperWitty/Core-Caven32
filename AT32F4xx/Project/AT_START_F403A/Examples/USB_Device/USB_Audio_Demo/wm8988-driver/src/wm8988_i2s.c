/*
  **************************************************************************
  * File   : wm8988_i2c.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : wm 8988  driver
  **************************************************************************
 */
 
#include "at32f4xx.h"
#include "wm8988_i2s.h"
#include "at32_audio_conf.h"

/*I2S1 Pin*/ 
#define I2S1_WS_PIN                GPIO_Pins_4
#define I2S1_CK_PIN                GPIO_Pins_5
#define I2S1_SD_PIN                GPIO_Pins_7
#define I2S1_GPIO                  GPIOA
#define I2S1_GPIO_RCC_CLK          RCC_APB2PERIPH_GPIOA
#define I2S1_DR_ADDRESS            0x4001300C

/*I2S2 Pin*/
#define I2S2_WS_PIN                GPIO_Pins_12
#define I2S2_CK_PIN                GPIO_Pins_13
#define I2S2_SD_PIN                GPIO_Pins_15
#define I2S2_GPIO                  GPIOB
#define I2S2_GPIO_RCC_CLK          RCC_APB2PERIPH_GPIOB
#define I2S2_DR_ADDRESS            0x4000380C


#define I2S_FREQ_16                16000
#define I2S_FREQ_48                48000

#define I2S_DATA_FIFOBUFFER_MAX    192 * 20 + 192
#define I2S_DATA_VALIDBUFFER_MAX  (I2S_DATA_FIFOBUFFER_MAX - 192) / 2
#define I2S_WRITE_LEN_ONTIMES      1
typedef struct _I2S_Fifo_Buffer
{
    uint16_t u16I2S1_SpeakerFiFoBuffer[I2S_DATA_FIFOBUFFER_MAX]; /*Speaker Fifo buffer mose*/
    uint16_t u16I2S1_WritePointer;
    uint16_t u16I2S1_ReadPointer;
    uint32_t u32NbyteWr;
}I2S_Fifo_Buffer;

I2S_Fifo_Buffer I2S_FiFoSpeaker = {{0}, 0, 0, 0};
I2S_TransBuffer I2S1_SpeakerBuffer = {{0}, {0}, 0, 0};                /* Speaker use buffer*/
I2S_TransBuffer I2S2_MicrophoneBuffer = {{0}, {0}, 0, 0};;                /* Microphone use buffer*/


typedef struct __I2S_Config__
{
    uint32_t u32SpeakerBufLen;    /* Speaker Send data from I2S to WM8988 1 frame length*/  
    uint32_t u32SpeakerBufLenB;    /* Speaker Send data from I2S to WM8988 1 frame length*/
    uint32_t u32MicrophoneBufLen;    /* Microphone Get data from WM8988 to I2S 1 frame length*/
    uint32_t u32I2SBitw;
    uint32_t u32I2SFreq;
    
}I2S_Config;

I2S_Config i2s_conf = {64, 16, 16000};  /*Tx Buffer len default is 64
                                         * bitw default is 16 bit
                                         * Freq default is 16000
                                        */

//static uint8_t *u8I2SCurBuffer = 0;
I2S_InitType I2S_InitStructure;
DMA_InitType DMA_InitStructure;
uint8_t u8ZeroBuffer[I2S_MAX_TXLEN] = {0};
 /**
  * @brief  I2S initialize
  *         GPIO, I2S, DMA, Interrut initialize
  * @param  None
  * @retval None
  */
 void AT32_I2S_Init(uint32_t freq, uint32_t bitw)
 {
    NVIC_InitType  NVIC_InitStructure;
    GPIO_InitType GPIO_InitStructure;  
    /*init gpio struct*/ 
    GPIO_StructInit(&GPIO_InitStructure);
    i2s_conf.u32I2SFreq = freq;
    i2s_conf.u32I2SBitw = bitw;
    i2s_conf.u32SpeakerBufLen = (i2s_conf.u32I2SFreq / 1000) * (i2s_conf.u32I2SBitw / 8) * 2;     /*Stereo*/
    i2s_conf.u32MicrophoneBufLen = (i2s_conf.u32I2SFreq / 1000) * (i2s_conf.u32I2SBitw / 8) * 2;     /*Stereo*/ 
    I2S1_SpeakerBuffer.u8I2SCurBuffer = 0;
    I2S2_MicrophoneBuffer.u8I2SCurBuffer = 0; 
    I2S_FiFoSpeaker.u16I2S1_ReadPointer  = 0;
    I2S_FiFoSpeaker.u16I2S1_WritePointer = 0; 

    /*********************************I2S1 initialize Speaker******************************/
    /*I2S1 is Slave Tx, Send data to WM8988*/
     
    /*Enable I2S1 GPIO Clock*/
    RCC_APB2PeriphClockCmd(I2S1_GPIO_RCC_CLK, ENABLE); 
     
    /*PA4-->I2S1_WS, PA5-->I2S1_CK,PA7-->I2S1_SD*/
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pins = I2S1_WS_PIN | I2S1_CK_PIN;
    GPIO_Init(I2S1_GPIO, &GPIO_InitStructure);
     
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pins = I2S1_SD_PIN;
    GPIO_Init(I2S1_GPIO, &GPIO_InitStructure);
     
    /*Enable I2S1 Clock*/
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_SPI1, ENABLE);
    /*Reset I2S1*/
    SPI_I2S_Reset(SPI1); 
    /*initialize I2S Struct*/
    I2S_DefaultInit(&I2S_InitStructure);
    if ( i2s_conf.u32I2SFreq == I2S_FREQ_48 ) /*freq is 48K*/
        I2S_InitStructure.I2S_AudioFreq = I2S_AUDIOFREQ_48K;
    else
        I2S_InitStructure.I2S_AudioFreq = I2S_AUDIOFREQ_16K; 
    I2S_InitStructure.I2s_AudioProtocol = I2S_AUDIOPROTOCOL_PHILLIPS;
    I2S_InitStructure.I2S_CPOL = I2S_CPOL_LOW;
    if ( i2s_conf.u32I2SBitw == 24 )
        I2S_InitStructure.I2S_FrameFormat = I2S_FRAMEFORMAT_DL24BIT_CHL32BIT;  /*24 bit wide*/
    else  
        I2S_InitStructure.I2S_FrameFormat = I2S_FRAMEFORMAT_DL16BIT_CHL16BIT;
    
    I2S_InitStructure.I2S_MCLKOE = I2S_MCLKOE_DISABLE;
    I2S_InitStructure.I2S_Mode = I2S_MODE_SLAVETX;
    I2S_Init(SPI1, &I2S_InitStructure);
    /* Enable I2S1 DMA request source */
    SPI_I2S_DMAEnable(SPI1,SPI_I2S_DMA_TX,ENABLE);
    I2S_Enable(SPI1, ENABLE);
    
    /*I2S1 DMA Configure, I2S1 TX DMA map DMA1_Channel3*/
    /*reset DMA1_Channel3*/
    RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA1, ENABLE);
    DMA_Reset(DMA1_Channel3);
    DMA_InitStructure.DMA_BufferSize = i2s_conf.u32SpeakerBufLen / 2;
    DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALDST;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)I2S1_SpeakerBuffer.u8I2S_BufferA;
    DMA_InitStructure.DMA_MemoryDataWidth = DMA_MEMORYDATAWIDTH_HALFWORD;
    DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE;
    DMA_InitStructure.DMA_Mode = DMA_MODE_NORMAL;
    DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;
    DMA_InitStructure.DMA_PeripheralBaseAddr = I2S1_DR_ADDRESS;
    DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_HALFWORD;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;
    DMA_InitStructure.DMA_Priority = DMA_PRIORITY_HIGH;
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);
    DMA_INTConfig(DMA1_Channel3, DMA_INT_TC, ENABLE);
    DMA_ChannelEnable(DMA1_Channel3, ENABLE);
    
    /*Enable DMA1 Channel3 TC Interrupt*/
    NVIC_PriorityGroupConfig(2);
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);  
    /*********************************End I2S1 initialize******************************/
    
    
    
    /*********************************I2S2 initialize Microphone******************************/
        /*Enable I2S1 GPIO Clock*/
    RCC_APB2PeriphClockCmd(I2S2_GPIO_RCC_CLK, ENABLE); 
     
    /*PA4-->I2S1_WS, PA5-->I2S1_CK,PA7-->I2S1_SD*/
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pins = I2S2_WS_PIN | I2S2_CK_PIN;
    GPIO_Init(I2S2_GPIO, &GPIO_InitStructure);
     
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pins = I2S2_SD_PIN;
    GPIO_Init(I2S2_GPIO, &GPIO_InitStructure);
     
    /*Enable I2S1 Clock*/
    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_SPI2, ENABLE);
    /*Reset I2S1*/
    SPI_I2S_Reset(SPI2);
    
    
    
      /*initialize I2S Struct*/
    I2S_DefaultInit(&I2S_InitStructure);
    if ( i2s_conf.u32I2SFreq == I2S_FREQ_48 ) /*freq is 48K*/
        I2S_InitStructure.I2S_AudioFreq = I2S_AUDIOFREQ_48K;
    else
        I2S_InitStructure.I2S_AudioFreq = I2S_AUDIOFREQ_16K; 
    I2S_InitStructure.I2s_AudioProtocol = I2S_AUDIOPROTOCOL_PHILLIPS;
    I2S_InitStructure.I2S_CPOL = I2S_CPOL_LOW;
    if ( i2s_conf.u32I2SBitw == 24 )
        I2S_InitStructure.I2S_FrameFormat = I2S_FRAMEFORMAT_DL24BIT_CHL32BIT;  /*24 bit wide*/
    else
        I2S_InitStructure.I2S_FrameFormat = I2S_FRAMEFORMAT_DL16BIT_CHL16BIT;
    
    I2S_InitStructure.I2S_MCLKOE = I2S_MCLKOE_DISABLE;
    I2S_InitStructure.I2S_Mode = I2S_MODE_SLAVERX;
    I2S_Init(SPI2, &I2S_InitStructure);
    /* Enable I2S2 DMA request source */
    SPI_I2S_DMAEnable(SPI2,SPI_I2S_DMA_RX,ENABLE);
    I2S_Enable(SPI2, ENABLE);
    
    
    RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA1, ENABLE);
    DMA_Reset(DMA1_Channel4);
    DMA_InitStructure.DMA_BufferSize = i2s_conf.u32MicrophoneBufLen / 2;
    DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALSRC;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)I2S2_MicrophoneBuffer.u8I2S_BufferA;
    DMA_InitStructure.DMA_MemoryDataWidth = DMA_MEMORYDATAWIDTH_HALFWORD;
    DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE;
    DMA_InitStructure.DMA_Mode = DMA_MODE_NORMAL;
    DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;
    DMA_InitStructure.DMA_PeripheralBaseAddr = I2S2_DR_ADDRESS;
    DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_HALFWORD;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;
    DMA_InitStructure.DMA_Priority = DMA_PRIORITY_HIGH;
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);
    DMA_INTConfig(DMA1_Channel4, DMA_INT_TC, ENABLE);
    DMA_ChannelEnable(DMA1_Channel4, ENABLE);
    
    NVIC_PriorityGroupConfig(2);
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);  
    /*********************************End I2S2 initialize******************************/
    
    
 }
/**
  * @brief  I2S set freq 
  * @param  freq set new frequency
  * @retval None
  */
void AT32_SetI2S_FREQ(uint32_t freq)
{  
    /*Disable I2S and DMA*/
    I2S_Enable(SPI1, DISABLE);
    DMA_ChannelEnable(DMA1_Channel3, DISABLE);
    
    I2S_Enable(SPI2, DISABLE);
    DMA_ChannelEnable(DMA1_Channel4, DISABLE);
   
    i2s_conf.u32I2SFreq = freq;
    i2s_conf.u32SpeakerBufLen = (i2s_conf.u32I2SFreq / 1000) * (i2s_conf.u32I2SBitw / 8) * 2;     /*Stereo*/
    i2s_conf.u32MicrophoneBufLen = (i2s_conf.u32I2SFreq / 1000) * (i2s_conf.u32I2SBitw / 8) * 2;     /*Stereo*/
    /*Reset SPI*/
    SPI_I2S_Reset(SPI1);
    SPI_I2S_Reset(SPI2);
    if ( freq == I2S_FREQ_48 )
    {
        I2S_InitStructure.I2S_AudioFreq = I2S_AUDIOFREQ_48K;
    }
    else
    {
        I2S_InitStructure.I2S_AudioFreq = I2S_AUDIOFREQ_16K;
    }
    I2S_InitStructure.I2S_Mode = I2S_MODE_SLAVETX;
    I2S_Init(SPI1, &I2S_InitStructure);
    /* Enable I2S2 DMA request source */
    SPI_I2S_DMAEnable(SPI1,SPI_I2S_DMA_TX,ENABLE);
    I2S_Enable(SPI1, ENABLE);
    
    I2S_InitStructure.I2S_Mode = I2S_MODE_SLAVERX;
    I2S_Init(SPI2, &I2S_InitStructure);
    /* Enable I2S2 DMA request source */
    SPI_I2S_DMAEnable(SPI2,SPI_I2S_DMA_RX,ENABLE);
    I2S_Enable(SPI2, ENABLE);
    
    
    /*Reset DMA*/
    DMA_Reset(DMA1_Channel3);
    DMA_InitStructure.DMA_BufferSize = i2s_conf.u32SpeakerBufLen / 2;
    DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALDST;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)I2S1_SpeakerBuffer.u8I2S_BufferA;
    DMA_InitStructure.DMA_MemoryDataWidth = DMA_MEMORYDATAWIDTH_HALFWORD;
    DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE;
    DMA_InitStructure.DMA_Mode = DMA_MODE_NORMAL;
    DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;
    DMA_InitStructure.DMA_PeripheralBaseAddr = I2S1_DR_ADDRESS;
    DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_HALFWORD;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;
    DMA_InitStructure.DMA_Priority = DMA_PRIORITY_HIGH;
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);
    DMA_INTConfig(DMA1_Channel3, DMA_INT_TC, ENABLE);
    DMA_ChannelEnable(DMA1_Channel3, ENABLE);
    
    DMA_Reset(DMA1_Channel4);
    DMA_InitStructure.DMA_BufferSize = i2s_conf.u32MicrophoneBufLen / 2;
    DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALSRC;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)I2S2_MicrophoneBuffer.u8I2S_BufferA;
    DMA_InitStructure.DMA_MemoryDataWidth = DMA_MEMORYDATAWIDTH_HALFWORD;
    DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE;
    DMA_InitStructure.DMA_Mode = DMA_MODE_NORMAL;
    DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;
    DMA_InitStructure.DMA_PeripheralBaseAddr = I2S2_DR_ADDRESS;
    DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_HALFWORD;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;
    DMA_InitStructure.DMA_Priority = DMA_PRIORITY_HIGH;
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);
    DMA_INTConfig(DMA1_Channel4, DMA_INT_TC, ENABLE);
    DMA_ChannelEnable(DMA1_Channel4, ENABLE);
    
}

/**
  * @brief  get current empty buffer pointer
  * @param  None
  * @retval buffer pointer
  */
uint8_t *AT32_I2S1_GetSpeakerBuffer()
{
//    return I2S1_SpeakerBuffer.u8I2SCurBuffer;
    return (uint8_t *)I2S_FiFoSpeaker.u16I2S1_SpeakerFiFoBuffer;
}


/**
  * @brief  get current buffer
  * @param  None
  * @retval buffer pointer
  */
uint8_t *AT32_I2S1_GetMicrophoneBuffer()
{
    return I2S2_MicrophoneBuffer.u8I2SCurBuffer;
}


/**
  * @brief  Get Microphone buffer length
  * @param  None
  * @retval Buffer Len
  */
uint32_t AT32_I2S1_GetMicrophoneFrameLen()
{
    return i2s_conf.u32MicrophoneBufLen;
}

/**
  * @brief  set Speaker buffer length
  * @param  buffer_len
  * @retval Buffer Len
  */
uint32_t AT32_I2S1_SetSpeakerFrameLen(uint32_t u32Len)
{
  return 0;    
}

/**
  * @brief  Write data to Speaker buffer
  * @param  u8Pointer  buffer poinger
  * @param  u16Len     Buffer len
  * @retval status
  */
uint8_t AT32_I2S1_WriteDataToSpkFifoBuffer(uint8_t *u8Pointer, uint16_t u16Len)
{
    uint16_t *u16bufer = (uint16_t *)u8Pointer;
    uint16_t *u16Fifo = I2S_FiFoSpeaker.u16I2S1_SpeakerFiFoBuffer + I2S_FiFoSpeaker.u16I2S1_WritePointer;
    uint16_t u16BufferLen = u16Len >> 1, i;
  
    for ( i = 0; i < u16BufferLen; i ++ )
    {
        if ( I2S_FiFoSpeaker.u16I2S1_WritePointer >= I2S_DATA_VALIDBUFFER_MAX )
        {
            I2S_FiFoSpeaker.u16I2S1_WritePointer = 0;
            u16Fifo = I2S_FiFoSpeaker.u16I2S1_SpeakerFiFoBuffer;
        }
        *u16Fifo ++ = *u16bufer ++;
        I2S_FiFoSpeaker.u16I2S1_WritePointer ++;

    }
    return 0;
}


/**
  * @brief  Write data to Speaker buffer
  * @param  u8Pointer  buffer poinger
  * @param  u16Len     Buffer len
  * @retval status
  */
uint32_t AT32_I2S1_GetNbyteDataSend()
{
    uint32_t u32Nbyte = I2S_FiFoSpeaker.u32NbyteWr;
    I2S_FiFoSpeaker.u32NbyteWr = 0;
    return u32Nbyte;
}
  /**
  * @brief  I2S1 TX DMA1_Channel3 IRQ Hander 
  * @param  None
  * @retval None
  */
void DMA1_Channel3_IRQHandler()
{
    uint16_t u16Byte = I2S_WRITE_LEN_ONTIMES;
    if (DMA_GetITStatus(DMA1_INT_TC3) == SET)
    {
        DMA_ChannelEnable(DMA1_Channel3,DISABLE);
        DMA_ClearITPendingBit(DMA1_INT_TC3);
        if ( (I2S_FiFoSpeaker.u16I2S1_WritePointer > I2S_FiFoSpeaker.u16I2S1_ReadPointer))
        {
          
            if ((I2S_FiFoSpeaker.u16I2S1_ReadPointer + I2S_WRITE_LEN_ONTIMES > I2S_FiFoSpeaker.u16I2S1_WritePointer))  
                u16Byte = I2S_FiFoSpeaker.u16I2S1_WritePointer - I2S_FiFoSpeaker.u16I2S1_ReadPointer;
        }else if ((I2S_FiFoSpeaker.u16I2S1_ReadPointer > I2S_FiFoSpeaker.u16I2S1_WritePointer))
        {
            if (I2S_FiFoSpeaker.u16I2S1_ReadPointer + I2S_WRITE_LEN_ONTIMES > I2S_DATA_VALIDBUFFER_MAX )
                u16Byte = I2S_DATA_VALIDBUFFER_MAX - I2S_FiFoSpeaker.u16I2S1_ReadPointer;
        }

        if ( I2S_FiFoSpeaker.u16I2S1_WritePointer == I2S_FiFoSpeaker.u16I2S1_ReadPointer )  /*No data*/
        {
            DMA1_Channel3->CMBA = (uint32_t)u8ZeroBuffer;
        }
        else
        {
            DMA1_Channel3->CMBA = (uint32_t)((uint16_t *)I2S_FiFoSpeaker.u16I2S1_SpeakerFiFoBuffer + I2S_FiFoSpeaker.u16I2S1_ReadPointer);
            
            I2S_FiFoSpeaker.u16I2S1_ReadPointer += u16Byte;
            if ( I2S_FiFoSpeaker.u16I2S1_ReadPointer >= I2S_DATA_VALIDBUFFER_MAX)
                I2S_FiFoSpeaker.u16I2S1_ReadPointer = 0;
            
       }
       I2S_FiFoSpeaker.u32NbyteWr += u16Byte;
       DMA1_Channel3->TCNT = u16Byte;
       DMA_ChannelEnable(DMA1_Channel3,ENABLE);
        
    }
}


  /**
  * @brief  I2S2 RX DMA1_Channel4 IRQ Hander 
  * @param  None
  * @retval None
  */
void DMA1_Channel4_IRQHandler()
{
    if (DMA_GetITStatus(DMA1_INT_TC4) == SET)
    {
        DMA_ClearITPendingBit(DMA1_INT_TC4);
        DMA_ChannelEnable(DMA1_Channel4,DISABLE);
        if ( I2S2_MicrophoneBuffer.u8I2SBufAB == 0 )
        { /*BufferA transmit complete*/
            I2S2_MicrophoneBuffer.u8I2SBufAB = 1;        /*Next DMA use Buffer B*/
            DMA1_Channel4->CMBA = (uint32_t)I2S2_MicrophoneBuffer.u8I2S_BufferB;
            I2S2_MicrophoneBuffer.u8I2SCurBuffer = I2S2_MicrophoneBuffer.u8I2S_BufferA;
            
        }else
        {/*BufferB transmit complete*/
            I2S2_MicrophoneBuffer.u8I2SBufAB = 0;       /*Next DMA use Buffer A*/
            DMA1_Channel4->CMBA = (uint32_t)I2S2_MicrophoneBuffer.u8I2S_BufferA;
            I2S2_MicrophoneBuffer.u8I2SCurBuffer = I2S2_MicrophoneBuffer.u8I2S_BufferB;
            
        }
        DMA1_Channel4->TCNT = i2s_conf.u32MicrophoneBufLen / 2;
        DMA_ChannelEnable(DMA1_Channel4,ENABLE);
    }
}

