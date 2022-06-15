/*
  **************************************************************************
  * File   : wm8988.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : WM8988  driver
  **************************************************************************
 */
#include "at32f4xx.h"
#include "wm8988.h"
#include "wm8988_i2c.h"
#include "wm8988_i2s.h"
//#include "at32_audio_conf.h"

#define WM8988_REG_FREQ48K             ((WM8988_R8_SAMPLE_RATE << 9) | 0x0081)
#define WM8988_REG_FREQ16K             ((WM8988_R8_SAMPLE_RATE << 9) | 0x0095)
#if (AT32_I2S_IS_SLAVE == 1)
#define WM8988_REG_BITW24                  ((WM8988_R7_AUDIO_INTERFACE << 9) | 0x004A)
#define WM8988_REG_BITW16                  ((WM8988_R7_AUDIO_INTERFACE << 9) | 0x0042)
#else
#define WM8988_REG_BITW24                  ((WM8988_R7_AUDIO_INTERFACE << 9) | 0x000A)
#define WM8988_REG_BITW16                  ((WM8988_R7_AUDIO_INTERFACE << 9) | 0x0002)
#endif
//#define WM8988_REG_BITW24                  ((WM8988_R7_AUDIO_INTERFACE << 9) | 0x000A)
//#define WM8988_REG_BITW16                  ((WM8988_R7_AUDIO_INTERFACE << 9) | 0x0002)


#define AUDIO_FREQ_16K                 16000
#define AUDIO_FREQ_48K                 48000

#define AUDIO_BITW_16                  16
#define AUDIO_BITW_24                  24



#define WM8988_R8_DEFAULT_FREQ         WM8988_REG_FREQ16K
#define WM8988_R7_DEFAULT_BITW         WM8988_REG_BITW16





/*7 bit address + 9bit data*/
uint16_t u16RegAddrData[] = 
{
//    (WM8988_R15_RESET << 9) |                     0x0000,                 /*Reset*/
    (WM8988_R0_LEFT_INPUT_VOLUME << 9) |          0x012F,                /*Left Input Channel Volume*/
    (WM8988_R1_RIGHT_INPUT_VOLUME << 9) |         0x012F,                /*Right Input Channel Volume*/
    (WM8988_R2_LOUT1_VOLUME << 9) |               0x0179,                /*Left Output Channel Volume*/
    (WM8988_R3_ROUT1_VOLUME << 9) |               0x0179,                /*Right Output Channel Volume*/      
    (WM8988_R5_ADC_DAC_CONTROL << 9) |            0x0006,                /*De-emphasis Control and Digital soft mute*/
    WM8988_R7_DEFAULT_BITW,
    WM8988_R8_DEFAULT_FREQ,
    (WM8988_R10_LEFT_DAC_VOLUME << 9) |           0x01FF,                /*Left Digital DAC Volume Control*/
    (WM8988_R11_RIGHT_DAC_VOLUME << 9) |          0x01FF,                /*Right Digital DAC Volume Control*/
    (WM8988_R12_BASS_CONTROL << 9) |              0x000F,                /*Bass Control*/
    (WM8988_R13_TREBLE_CONTROL << 9) |            0x000F,                /*Treble Control*/
    (WM8988_R16_3D_CONTROL << 9) |                0x0000,                /*3D stereo Enhancment*/
//    (WM8988_R17_ALC1 << 9) |                      0x01FB,                /*3D stereo Enhancment*/
    (WM8988_R21_LEFT_ADC_VOLUME << 9) |           0x01C3,                /*Left ADC Digital Volume*/
    (WM8988_R22_RIGHT_ADC_VOLUME << 9) |          0x01C3,                /*Right ADC Digital Volume*/    
    (WM8988_R23_ADDITIONAL_1_CONTROL << 9) |      0x00C2,                /*Additional Control 1*/
    (WM8988_R24_ADDITIONAL_2_CONTROL << 9) |      0x0000,                /*Additional Control 2*/
    (WM8988_R27_ADDITIONAL_3_CONTROL << 9) |      0x0000,                /*Additional Control 3*/
    (WM8988_R31_ADC_INPUT_MODE << 9) |            0x0000,                /*ADC input mode*/
    (WM8988_R32_ADC_L_SIGNAL_PATH << 9) |         0x0000,                /*ADC Signal Path Control left*/  
    (WM8988_R33_ADC_R_SIGNAL_PATH << 9) |         0x0000,                /*ADC Signal Path Control right*/
    (WM8988_R34_LEFT_OUT_MIX_1 << 9) |            0x0152,                /*Left DAC mixer Control*/
    (WM8988_R35_LEFT_OUT_MIX_2 << 9) |            0x0050,                /*Left DAC mixer Control*/
    (WM8988_R36_RIGHT_OUT_MIX_1 << 9) |           0x0052,                /*Right DAC mixer Control*/
    (WM8988_R37_RIGHT_OUT_MIX_2 << 9) |           0x0150,                /*Right DAC mixer Control*/
    (WM8988_R40_LOUT2_VOLUME << 9) |              0x01FF,                /*Output left channel volume*/
    (WM8988_R41_ROUT2_VOLUME << 9) |              0x01FF,                /*Output left channel volume*/
    (WM8988_R43_LOW_POWER_PALYBACK << 9) |        0x0008,                /*Output left channel volume*/
    (WM8988_R25_PWR_1_MGMT << 9) |                0x017C,                /*Power Management1*/
    (WM8988_R26_PWR_2_MGMT << 9) |                0x01F8,                /*Power Management2*/     
};
static uint16_t *u16DefaultFreqR8 = &u16RegAddrData[6];                  /*Frequency value*/
static uint16_t *u16DefaultBitwR7 = &u16RegAddrData[5];                  /*Bit Wide value*/
uint16_t u16ADCVolume = 0, u16oldADCVolume = 0;
uint16_t u16DACVolume = 0, u16oldDACVolume = 0;
uint16_t u16ADCMute = 0, u16oldADCMute = 0;
uint16_t u16DACMute = 0, u16oldDACMute = 0;
uint32_t u32Freq = 0;
static void WM8988_Clk_TMR1_Init(void);

///**
//  * @brief  Provide WM8988 MCLK Use PA8 
//  *         WM8988 run USB mode, Provide 12Mhz Clock
//  * @param  None
//  * @retval None
//  */
//static void WM8988_MCLKConfig(void)
//{
//    GPIO_InitType  GPIO_InitStructure;

//    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA|RCC_APB2PERIPH_AFIO, ENABLE);

//    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_8;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);
//    RCC_CLKOUTConfig(RCC_CLKOUT_PLL_Div4);
//}

/**
  * @brief  Provide WM8988 MCLK Use PA8 
  *         WM8988 run USB mode, Provide 12Mhz Clock
  * @param  None
  * @retval None
  */
static void WM8988_Clk_TMR1_Init()
{
  
    GPIO_InitType GPIO_InitStructure;
    TMR_TimerBaseInitType  TMR_TMReBaseStructure;
    TMR_OCInitType  TMR_OCInitStructure;
    uint16_t Preiod = 1;

    RCC_APB2PeriphClockCmd(  RCC_APB2PERIPH_GPIOA  | RCC_APB2PERIPH_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR1, ENABLE);

    /* GPIOA Configuration:TMR1 Channel1 */
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_2MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    /* TMR base configuration */
    TMR_TimeBaseStructInit(&TMR_TMReBaseStructure);
    TMR_TMReBaseStructure.TMR_Period = Preiod;
    TMR_TMReBaseStructure.TMR_DIV = (uint16_t) (SystemCoreClock / 24000000) - 1;;
    TMR_TMReBaseStructure.TMR_ClockDivision = 0;
    TMR_TMReBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;

    TMR_TimeBaseInit(TMR1, &TMR_TMReBaseStructure);

    /* PWM1 Mode configuration: Channel1 */
    TMR_OCStructInit(&TMR_OCInitStructure);
    TMR_OCInitStructure.TMR_OCMode = TMR_OCMode_PWM1;
    TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
    TMR_OCInitStructure.TMR_Pulse = (Preiod + 1) / 2;
    TMR_OCInitStructure.TMR_OCPolarity = TMR_OCPolarity_High;
    TMR_OC1Init(TMR1, &TMR_OCInitStructure);
    TMR_OC1PreloadConfig(TMR1, TMR_OCPreload_Enable);

    /* TMR1 enable counter */
    TMR_Cmd(TMR1, ENABLE);
    /* Main Output Enable */
    TMR_CtrlPWMOutputs(TMR1, ENABLE);

}


/**
  * @brief  WM8988 initialize interface 
  *         init WM8988 register
  * @param  freq WM8988 startup frequency
  * @param  bitw WM8988 startup bit wide
  * @retval None
  */
uint8_t WM8988_Init(uint32_t freq, uint32_t bitw)
{
    /**/
    uint8_t u8item = 0, len = sizeof(u16RegAddrData) / sizeof(uint16_t);
  
    /*I2C GPIO init*/
    AT32_I2C_Init();
    /*Set Freq and bitw*/
    if ( freq == AUDIO_FREQ_48K )
        *u16DefaultFreqR8 = WM8988_REG_FREQ48K;
    else
        *u16DefaultFreqR8 = WM8988_REG_FREQ16K;
    if (bitw == AUDIO_BITW_24)
        *u16DefaultBitwR7 = WM8988_REG_BITW24;
    else
        *u16DefaultBitwR7 = WM8988_REG_BITW16;
    u32Freq = freq;
    /*Set WM8988 register*/
    for ( u8item = 0; u8item < len; u8item ++ )
    {
        if ( AT32_I2C_WriteReg(WM8988_I2C_ADDR_CSB_LOW, u16RegAddrData[u8item] >> 8, u16RegAddrData[u8item] & 0xFF) != I2C_SUCCESS )
        {
            /*I2C init failed*/
            return 1;
        }
    }
    /*Configure WM8988 MCLK*/
    WM8988_Clk_TMR1_Init();
    /*WM8988 I2S initialize*/
    AT32_I2S_Init(freq, bitw);
    
    
    return 0;
}

/**
  * @brief  WM8988 set freq 
  * @param  freq 
  * @retval None
  */
uint8_t WM8988_Set_Freq(uint32_t freq)
{
//    if ( u32Freq == freq )
//      return 0;
//    u32Freq = freq;
//    TMR_Cmd(TMR1, DISABLE);
//    if ( freq == AUDIO_FREQ_48K )
//    {
//        AT32_I2C_WriteReg(WM8988_I2C_ADDR_CSB_LOW, WM8988_REG_FREQ48K >> 8, WM8988_REG_FREQ48K & 0xFF);
//    }
//    else
//    {
//        AT32_I2C_WriteReg(WM8988_I2C_ADDR_CSB_LOW, WM8988_REG_FREQ16K >> 8, WM8988_REG_FREQ16K & 0xFF);
//    }
//    
//    /*Set I2S freq*/
//    AT32_SetI2S_FREQ(freq);
//    TMR_Cmd(TMR1, ENABLE);
    return 0;
}


/**
  * @brief  WM8988 set Microphone Volume 
  * @param  Volume 0~64
  * @retval None
  */
uint8_t WM8988_SetADCMic_Volume(uint16_t volume)
{
    /*Set ADC Mic volume*/
    u16ADCVolume = volume;
    return 0;
}

/**
  * @brief  WM8988 set Speaker Volume 
  * @param  Volume 0~64
  * @retval None
  */
uint8_t WM8988_SetDACSpk_Volume(uint16_t volume)
{
    /*Set DAC Spk volume*/
    u16DACVolume = volume;
    return 0;
}

/**
  * @brief  WM8988 set Speaker Mute 
  * @param  Volume 0~64
  * @retval None
  */
uint8_t WM8988_SetDACSpk_Mute(uint8_t mute)
{
    u16DACMute = mute;   
    return 0;
}


/**
  * @brief  WM8988 set Microphone Mute 
  * @param  Volume 0~64
  * @retval None
  */
uint8_t WM8988_SetADCMic_Mute(uint8_t mute)
{
    u16ADCMute = mute;
    return 0;
}
/**
  * @brief  Get WM8988 Speaker buffer pointer
  * @param  None
  * @retval buffer pointer
  */
uint8_t *WM8988_GetSpeakerBuffer()
{
    return AT32_I2S1_GetSpeakerBuffer();
}

/**
  * @brief  Write WM8988 Speaker buffer pointer
  * @param  u8Pointer   data buffer
  * @param  u16Len      data len
  * @retval None
  */
void WM8988_WriteSpeakerBuffer(uint8_t *u8Pointer, uint16_t u16Len)
{
    AT32_I2S1_WriteDataToSpkFifoBuffer(u8Pointer, u16Len);
}
 

/**
  * @brief  Get WM8988 Microphone buffer pointer
  * @param  None
  * @retval buffer pointer
  */
uint8_t *WM8988_GetMicrophoneBuffer()
{
    return AT32_I2S1_GetMicrophoneBuffer();
}

/**
  * @brief  Get WM8988 Microphone buffer length
  * @param  None
  * @retval buffer pointer
  */
uint32_t WM8988_GetMicrophoneFrameLen()
{
    return AT32_I2S1_GetMicrophoneFrameLen();
}

/**
  * @brief  Set WM8988 Speaker buffer length
  * @param  buffer len
  * @retval buffer len
  */
uint32_t WM8988_SetSpeakerFrameLen(uint32_t u32Len)
{
    return AT32_I2S1_SetSpeakerFrameLen(u32Len);
}

/**
  * @brief  WM8988 Poll
  * @param  None
  * @retval None
  */
void WM8988_Poll(void)
{
    uint16_t u16LVolume;
    uint16_t u16RVolume;
    uint16_t u16Mute;
    if ( u16DACVolume != u16oldDACVolume )
    {
        u16LVolume = WM8988_R2_LOUT1_VOLUME << 9 | 0x100;
        u16RVolume = WM8988_R3_ROUT1_VOLUME << 9 | 0x100;
        u16oldDACVolume = u16DACVolume;
        u16LVolume |= ( u16oldDACVolume & 0x7F);
        u16RVolume |= ( u16oldDACVolume & 0x7F);
        AT32_I2C_WriteReg(WM8988_I2C_ADDR_CSB_LOW, u16LVolume >> 8 , u16LVolume & 0xFF);
        AT32_I2C_WriteReg(WM8988_I2C_ADDR_CSB_LOW, u16RVolume >> 8 , u16RVolume & 0xFF);
    }
    if ( u16ADCVolume != u16oldADCVolume )
    {
        u16oldADCVolume = u16ADCVolume;
        u16LVolume = WM8988_R21_LEFT_ADC_VOLUME << 9 | 0x100;
        u16RVolume = WM8988_R22_RIGHT_ADC_VOLUME << 9 | 0x100;
        u16LVolume |= ( u16oldADCVolume & 0xFF);
        u16RVolume |= ( u16oldADCVolume & 0xFF);
        AT32_I2C_WriteReg(WM8988_I2C_ADDR_CSB_LOW, (u16LVolume >> 8) & 0xFF, u16LVolume & 0xFF);
        AT32_I2C_WriteReg(WM8988_I2C_ADDR_CSB_LOW, (u16RVolume >> 8) & 0xFF , u16RVolume & 0xFF);
    }
    
    if (u16ADCMute != u16oldADCMute)
    {
        u16oldADCMute = u16ADCMute;
        u16LVolume = WM8988_R21_LEFT_ADC_VOLUME << 9 | 0x100;
        u16RVolume = WM8988_R22_RIGHT_ADC_VOLUME << 9 | 0x100;
        if ( u16oldADCMute == 1 )
        {
//            u16Mute = (WM8988_R0_LEFT_INPUT_VOLUME << 9) | 0x01C0 ;
//            AT32_I2C_WriteReg(WM8988_I2C_ADDR_CSB_LOW, u16Mute >> 8 , u16Mute & 0xFF);
//            u16Mute = (WM8988_R1_RIGHT_INPUT_VOLUME << 9) | 0x01C0 ;
//            AT32_I2C_WriteReg(WM8988_I2C_ADDR_CSB_LOW, u16Mute >> 8 , u16Mute & 0xFF);
            u16LVolume |= ( u16oldDACVolume & 0x7F);
            u16RVolume |= ( u16oldDACVolume & 0x7F);
        }else
        {
//            u16Mute = (WM8988_R0_LEFT_INPUT_VOLUME << 9) | 0x012F;
//            AT32_I2C_WriteReg(WM8988_I2C_ADDR_CSB_LOW, u16Mute >> 8 , u16Mute & 0xFF);
//            u16Mute = (WM8988_R1_RIGHT_INPUT_VOLUME << 9) | 0x012F;
//            AT32_I2C_WriteReg(WM8988_I2C_ADDR_CSB_LOW, u16Mute >> 8 , u16Mute & 0xFF);
        }
        AT32_I2C_WriteReg(WM8988_I2C_ADDR_CSB_LOW, (u16LVolume >> 8) & 0xFF, u16LVolume & 0xFF);
        AT32_I2C_WriteReg(WM8988_I2C_ADDR_CSB_LOW, (u16RVolume >> 8) & 0xFF , u16RVolume & 0xFF);
    }
    
    if ( u16DACMute != u16oldDACMute)
    {
        u16oldDACMute = u16DACMute;
        u16Mute = (WM8988_R5_ADC_DAC_CONTROL << 9) | 0x0006 | (u16oldDACMute << 3);
        AT32_I2C_WriteReg(WM8988_I2C_ADDR_CSB_LOW, u16Mute >> 8 , u16Mute & 0xFF);
    }
}
