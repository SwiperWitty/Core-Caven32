/*
  **************************************************************************
  * File   : at32_audio_conf.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : audio config header
  **************************************************************************
 */
#ifndef __AT32_AUDIO_CONF__H
#define __AT32_AUDIO_CONF__H
#include "at32f4xx.h"

/*Enable Speaker or Microphone, 1 is Enable, 0 is Disable*/
#define ENABLE_AT32_USB_SPEAKER            1
#define ENABLE_AT32_USB_MICROPHONE          1

/*Support 16K freq or 48K Freq, 1 is Enable, 0 is Disable*/
#define ENABLE_AT32_USB_AUDIO_FREQ_16K     1
#define ENABLE_AT32_USB_AUDIO_FREQ_48K     0

/*Support Audio Freq*/
#define AT32_AUDIO_FREQ_16K                16000
#define AT32_AUDIO_FREQ_48K                48000

/*Support Channel*/
#define AT32_AUDIO_STEREO                  2

/*Support bit Wide*/
#define AT32_AUDIO_BITW_16                 16


/*Freq 16K*/
#define AT32_DEFAULT_AUDIO_FREQ            AT32_AUDIO_FREQ_16K
/*Bit Wide 16bit*/
#define AT32_DEFAULT_AUDIO_BITW            AT32_AUDIO_BITW_16
/*Channel 2 channel, stereo*/
#define AT32_DEFAULT_AUDIO_CHR             AT32_AUDIO_STEREO


/*Enable FEED back*/
#define ENABLE_AT32_AUDIO_FEED_BACK        1


enum AUDIO_State
{
    AUDIO_OK,
    AUDIO_FAIL,  
};

/* Macros to convert the samples number into 10.10 format (extended into 10.14 format) */
#define FEEDBACK_FF_FS_B0(a,b)                        (uint8_t)(((a) & 0x3FC) >> 2)
#define FEEDBACK_FF_FS_B1(a,b)                        (uint8_t)(((((a) & 0x03) << 6) & 0xC0) | (((b) >> 4) & 0x3F))
#ifndef FEEDBACK_EXTENDED_PRECISION_10_14
 #define FEEDBACK_FF_FS_B2(a,b)                        (uint8_t)(((b) << 4) & 0xF0)
#else
 #define FEEDBACK_FF_FS_B2(a,b)                        (uint8_t)(b & 0xFF)
#endif /* FEEDBACK_EXTENDED_PRECISION_10_14 */

#define AUDIO_DATA_BUFFER_MAX    1024

extern uint8_t feed_back[];

uint8_t Audio_Init(uint32_t freq, uint32_t bitw);
uint8_t *Audio_GetMicrophoneBuffer(void);
uint8_t *Audio_GetSpeakerBuffer(void);
void Audio_SetFreq(uint32_t u32freq, uint8_t u8Interface);
//void Audio_SetMute(uint8_t u8Mute, uint8_t u8Interface);
//void Audio_SetVolume(uint16_t u16Volume, uint8_t u8Interface);
void Audio_SetMute_Microphone(uint8_t u8Mute);
void Audio_SetMute_Speaker(uint8_t u8Mute);
void Audio_SetVolume_Microphone(uint16_t u16Volume);
void Audio_SetVolume_Speaker(uint16_t u16Volume);
uint32_t Audio_GetMicrophoneLen(void);
void Audio_SetSpeakerFrameLen(uint32_t u32Len);
void Audio_WriteDataToSpeakerBuffer(uint8_t *u8Pointer, uint16_t u16Len);
uint8_t *Audio_GetSpeakerFeedback(void);
#endif
