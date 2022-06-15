/**
  ******************************************************************************
  * File   : usbd_audio_out_if.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file provides the Audio Out (palyback) interface API.
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include "usbd_audio_core.h"
#include "usbd_audio_out_if.h"
#include "wm8988.h"

/** @addtogroup AT32_USB_OTG_DEVICE_LIBRARY
  * @{
  */


/** @defgroup usbd_audio_out_if 
  * @brief usbd out interface module
  * @{
  */ 

/** @defgroup usbd_audio_out_if_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup usbd_audio_out_if_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup usbd_audio_out_if_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup usbd_audio_out_if_Private_FunctionPrototypes
  * @{
  */
static uint8_t  Init         (uint32_t  AudioFreq, uint32_t Volume, uint32_t options);
static uint8_t  Reset       (uint32_t options);
static uint8_t  AudioCmd     (uint8_t* pbuf, uint32_t size, uint8_t cmd);
static uint8_t  VolumeSpkCtl    (uint8_t vol);
static uint8_t  VolumeMicCtl    (uint8_t vol);
static uint8_t  MuteSpkCtl      (uint8_t cmd);
static uint8_t  MuteMicCtl      (uint8_t cmd);
static uint8_t  FreqCtl      (uint32_t freq);
static uint32_t AudioLen     (void);
static uint8_t  PeriodicTC   (uint8_t cmd);
static uint8_t  GetState     (void);

/**
  * @}
  */ 

/** @defgroup usbd_audio_out_if_Private_Variables
  * @{
  */ 
AUDIO_FOPS_TypeDef  AUDIO_OUT_fops = 
{
  Init,
  Reset,
  AudioCmd,
  VolumeSpkCtl,
  VolumeMicCtl,
  MuteSpkCtl,
  MuteMicCtl,
  FreqCtl,
  AudioLen,
  PeriodicTC,
  GetState
};



static uint8_t AudioState = AUDIO_STATE_INACTIVE;
static uint32_t AudioBufferLen = AUDIO_OUT_PACKET;
static uint32_t AudioBitw = 16;

/**
  * @}
  */ 

/** @defgroup usbd_audio_out_if_Private_Functions
  * @{
  */ 

/**
  * @brief  Initialize and configures all required resources for audio record function.
  * @param  AudioFreq: Statrtup audio frequency. 
  * @param  Volume: Startup volume to be set.
  * @param  options: specific options passed to low layer function.
  * @retval AUDIO_OK if all operations succeed, AUDIO_FAIL else.
  */
uint8_t Audio_Init(uint32_t freq, uint32_t bitw)
{
  if ( WM8988_Init(freq, bitw) != AUDIO_OK )
    return AUDIO_FAIL;
  AudioBufferLen = freq * AT32_AUDIO_STEREO * bitw / 8 / 1000;
  AudioBitw = bitw;
  return AUDIO_OK;
}

/**
  * @brief  Init
  *         Initialize and configures all required resources for audio play function.
  * @param  AudioFreq: Startup audio frequency. 
  * @param  Volume: Startup volume to be set.
  * @param  options: specific options passed to low layer function.
  * @retval AUDIO_OK if all operations succeed, AUDIO_FAIL else.
  */
static uint8_t  Init         (uint32_t AudioFreq, 
                              uint32_t Volume, 
                              uint32_t options)
{
  
    /*Codec init*/
    
  return AUDIO_OK;
}

/**
  * @brief  Reset
  *         Free all resources used by low layer and stops audio-play function.
  * @param  options: options passed to low layer function.
  * @retval AUDIO_OK if all operations succeed, AUDIO_FAIL else.
  */
static uint8_t  Reset       (uint32_t options)
{
  /* Update the Audio state machine */
  AudioState = AUDIO_STATE_INACTIVE;
  
  return AUDIO_OK;
}

/**
  * @brief  AudioCmd 
  *         Play, Stop, Pause or Resume current file.
  * @param  pbuf: address from which file should be played.
  * @param  size: size of the current buffer/file.
  * @param  cmd: command to be executed, can be AUDIO_CMD_PLAY , AUDIO_CMD_PAUSE, 
  *              AUDIO_CMD_RESUME or AUDIO_CMD_STOP.
  * @retval AUDIO_OK if all operations succeed, AUDIO_FAIL else.
  */
static uint8_t  AudioCmd(uint8_t* pbuf, 
                         uint32_t size,
                         uint8_t cmd)
{
  return AUDIO_OK;
}

/**
  * @brief  VolumeCtl
  *         Set the volume level in %
  * @param  vol: volume level to be set in % (from 0% to 100%)
  * @retval AUDIO_OK if all operations succeed, AUDIO_FAIL else.
  */
static uint8_t  VolumeSpkCtl    (uint8_t vol)
{
  WM8988_SetDACSpk_Volume((uint16_t)vol & 0xFF);
  return AUDIO_OK;
}

/**
  * @brief  VolumeCtl
  *         Set the volume level in %
  * @param  vol: volume level to be set in % (from 0% to 100%)
  * @retval AUDIO_OK if all operations succeed, AUDIO_FAIL else.
  */
static uint8_t  VolumeMicCtl    (uint8_t vol)
{
  WM8988_SetADCMic_Volume((uint16_t)vol & 0xFF);
  return AUDIO_OK;
}

/**
  * @brief  MuteCtl
  *         Mute or Unmute the audio current output
  * @param  cmd: can be 0 to unmute, or 1 to mute.
  * @retval AUDIO_OK if all operations succeed, AUDIO_FAIL else.
  */
static uint8_t  MuteSpkCtl      (uint8_t cmd)
{
  WM8988_SetDACSpk_Mute(cmd);
  return AUDIO_OK;
}

/**
  * @brief  MuteCtl
  *         Mute or Unmute the audio current output
  * @param  cmd: can be 0 to unmute, or 1 to mute.
  * @retval AUDIO_OK if all operations succeed, AUDIO_FAIL else.
  */
static uint8_t  MuteMicCtl      (uint8_t cmd)
{
  WM8988_SetADCMic_Mute(cmd);
  return AUDIO_OK;
}

/**
  * @brief  MuteCtl
  *         Mute or Unmute the audio current output
  * @param  cmd: can be 0 to unmute, or 1 to mute.
  * @retval AUDIO_OK if all operations succeed, AUDIO_FAIL else.
  */
static uint8_t  FreqCtl      (uint32_t freq)
{
  AudioBufferLen = freq * AT32_AUDIO_STEREO * AudioBitw / 8 / 1000;
  WM8988_Set_Freq(freq);
  return AUDIO_OK;
}


static uint32_t AudioLen     (void)
{
  return AudioBufferLen;
}
/**
  * @brief  
  *         
  * @param  
  * @param  
  * @retval AUDIO_OK if all operations succeed, AUDIO_FAIL else.
  */
static uint8_t  PeriodicTC   (uint8_t cmd)
{

  
  return AUDIO_OK;
}


/**
  * @brief  GetState
  *         Return the current state of the audio machine
  * @param  None
  * @retval Current State.
  */
static uint8_t  GetState   (void)
{
  return AudioState;
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

