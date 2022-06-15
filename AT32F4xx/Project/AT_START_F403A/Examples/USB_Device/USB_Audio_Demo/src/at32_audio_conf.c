 /**
  ******************************************************************************
  * File   : USB_Device/USB_Audio_Demo/src/at32_audio_conf.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description for USB Audio Demo
  ******************************************************************************
  */
#include "at32f4xx.h"
#include "at32_audio_conf.h"
#include "wm8988.h"
#include "wm8988_i2s.h"

/** @addtogroup AT32F413_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_Audio_Demo
  * @{
  */ 
  
#define AT32_AUDIO_CUR_FREQ (AT32_DEFAULT_AUDIO_FREQ)
uint32_t u32AudioFreq = 0;

uint8_t feed_back[3] = {
                      FEEDBACK_FF_FS_B2((AT32_AUDIO_CUR_FREQ)/1000, AT32_AUDIO_CUR_FREQ - (AT32_AUDIO_CUR_FREQ / 1000) * 1000),
                      FEEDBACK_FF_FS_B1(AT32_AUDIO_CUR_FREQ/1000, AT32_AUDIO_CUR_FREQ - (AT32_AUDIO_CUR_FREQ / 1000) * 1000),
                      FEEDBACK_FF_FS_B0(AT32_AUDIO_CUR_FREQ/1000, AT32_AUDIO_CUR_FREQ - (AT32_AUDIO_CUR_FREQ / 1000) * 1000),
                       };
//uint8_t feed_back[3] = {
//                        00, 00, 04,
//                       };
/**
  * @brief  Initialize and configures all required resources for audio record function.
  * @param  AudioFreq: Statrtup audio frequency. 
  * @param  Volume: Startup volume to be set.
  * @param  options: specific options passed to low layer function.
  * @retval AUDIO_OK if all operations succeed, AUDIO_FAIL else.
  */
uint8_t Audio_Init(uint32_t freq, uint32_t bitw)
{
    /*Initialize AUDIO moudle*/
    u32AudioFreq = freq;
    if ( WM8988_Init(freq, bitw) != AUDIO_OK )
        return AUDIO_FAIL;
    return AUDIO_OK;
    
}
/**
  * @brief  Get Microphone buffer pointer
  * @param  None
  * @retval buffer pointer
  */
uint8_t *Audio_GetMicrophoneBuffer(void)
{
    return WM8988_GetMicrophoneBuffer();
}


/**
  * @brief  Get USB Audio Microphone frame length
  * @param  None
  * @retval None
  */
uint32_t Audio_GetMicrophoneLen(void)
{
    return WM8988_GetMicrophoneFrameLen();
}

/**
  * @brief  Get Speaker buffer pointer
  * @param  None
  * @retval buffer pointer
  */
uint8_t *Audio_GetSpeakerBuffer(void)
{
    return WM8988_GetSpeakerBuffer();
}


/**
  * @brief  Write data to speaker buffer
  * @param  
  * @retval buffer pointer
  */
void Audio_WriteDataToSpeakerBuffer(uint8_t *u8Pointer, uint16_t u16Len)
{
     WM8988_WriteSpeakerBuffer(u8Pointer, u16Len);
}


/**
  * @brief  Get Feedback
  * @param  None
  * @retval feed back pointer
  */
uint8_t *Audio_GetSpeakerFeedback(void)
{
    uint32_t u32Nbyte = AT32_I2S1_GetNbyteDataSend() << 1;
    uint32_t u32DfaultNbyte = u32AudioFreq * 4 / 1000 * 256, u32NewFreq;
    float fdefault = (float )u32AudioFreq * (float)u32Nbyte / (float)u32DfaultNbyte;
    fdefault += 0.5f;
    u32NewFreq = (uint32_t)fdefault;
    feed_back[0] = FEEDBACK_FF_FS_B2((u32NewFreq)/1000, u32NewFreq - (u32NewFreq / 1000) * 1000);
    feed_back[1] = FEEDBACK_FF_FS_B1(u32NewFreq/1000, u32NewFreq - (u32NewFreq / 1000) * 1000);
    feed_back[2] = FEEDBACK_FF_FS_B0(u32NewFreq/1000, u32NewFreq - (u32NewFreq / 1000) * 1000);
    return feed_back;
}

/**
  * @brief  Set audio new frequency
  * @param  u32freq  new frequency
  * @param  u8Interface  which moudle need set frequency
  * @retval None
  */
void Audio_SetFreq(uint32_t u32freq, uint8_t u8Interface)
{
    u32AudioFreq = u32freq;
//    if ( u8Interface == 0 )
   WM8988_Set_Freq(u32freq);       
}


/**
  * @brief  Set Microphone audio new Volume
  * @param  u8Interface  which moudle need set Volume
  * @retval None
  */
void Audio_SetVolume_Microphone(uint16_t u16Volume)
{
//    if ( u8Interface == 0 )
//        WM8988_Set_Volume(u32Volume);    
//    WM8988_Set_Volume(u16Volume&0x7F);  
    WM8988_SetADCMic_Volume( u16Volume );
}

/**
  * @brief  Set Speaker audio new Volume
  * @param  u8Interface  which moudle need set Volume
  * @retval None
  */
void Audio_SetVolume_Speaker(uint16_t u16Volume)
{
//    if ( u8Interface == 0 )
//        WM8988_Set_Volume(u32Volume);    
//    WM8988_Set_Volume(u16Volume&0x7F);   
    WM8988_SetDACSpk_Volume( u16Volume );
}

/**
  * @brief  Set Speaker audio mute
  * @param  u8Interface  which moudle need set mute
  * @retval None
  */
void Audio_SetMute_Microphone(uint8_t u8Mute)
{
//    if ( u8Interface == 0 )
//        WM8988_Set_Mute(u32Mute);
//    if (u8Interface == 0x5) /*Speaker Mute*/   
//        WM8988_Set_Mute(u8Mute);  
//     if (u8Interface == 0x2) /*Microphone Mute*/  
//         WM8988_Set_Mute(u8Mute); 
    WM8988_SetADCMic_Mute(u8Mute);   
}


/**
  * @brief  Set Speaker audio mute
  * @param  u8Interface  which moudle need set mute
  * @retval None
  */
void Audio_SetMute_Speaker(uint8_t u8Mute)
{
//    if ( u8Interface == 0 )
//        WM8988_Set_Mute(u32Mute);
//    if (u8Interface == 0x5) /*Speaker Mute*/   
//        WM8988_Set_Mute(u8Mute);  
//     if (u8Interface == 0x2) /*Microphone Mute*/  
//         WM8988_Set_Mute(u8Mute); 
    WM8988_SetDACSpk_Mute(u8Mute);  
}



/**
  * @brief  Set Speaker buffer len
  * @param  u32Len  recv len
  * @retval None
  */
void Audio_SetSpeakerFrameLen(uint32_t u32Len)
{
    WM8988_SetSpeakerFrameLen(u32Len);
}
/**
  * @}
  */

/**
  * @}
  */


/**
  * @}
  */

/**
  * @}
  */
