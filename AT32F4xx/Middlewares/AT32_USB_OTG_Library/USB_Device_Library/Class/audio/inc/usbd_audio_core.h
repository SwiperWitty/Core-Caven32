/**
  ******************************************************************************
  * File   : usbd_audio_core.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : header file for the usbd_audio_core.c file.
  ******************************************************************************
  */   

/* Includes ------------------------------------------------------------------*/

#ifndef __USB_AUDIO_CORE_H_
#define __USB_AUDIO_CORE_H_

#include "usbd_ioreq.h"
#include "usbd_req.h"
#include "usbd_desc.h"
#include "usbd_conf.h"




/** @addtogroup AT32_USB_OTG_DEVICE_LIBRARY
  * @{
  */
  
/** @defgroup usbd_audio
  * @brief This file is the Header file for USBD_audio.c
  * @{
  */ 


/** @defgroup usbd_audio_Exported_Defines
  * @{
  */ 

/* AudioFreq * DataSize (2 bytes) * NumChannels (Stereo: 2) */
#define AUDIO_OUT_PACKET                              (uint32_t)(((USBD_AUDIO_FREQ * 2 * 2) /1000)) 
#define AUDIO_MAX_PACKET                               192

/* Number of sub-packets in the audio transfer buffer. You can modify this value but always make sure
  that it is an even number and higher than 3 */
#define OUT_PACKET_NUM                                   4
/* Total size of the audio transfer buffer */
//#define TOTAL_OUT_BUF_SIZE                           ((uint32_t)(AUDIO_OUT_PACKET * OUT_PACKET_NUM))
#define TOTAL_OUT_BUF_SIZE                           ((uint32_t)(256))

#define AUDIO_CONFIG_DESC_SIZE                        109
#define AUDIO_INTERFACE_DESC_SIZE                     9
#define USB_AUDIO_DESC_SIZ                            0x09
#define AUDIO_STANDARD_ENDPOINT_DESC_SIZE             0x09
#define AUDIO_STREAMING_ENDPOINT_DESC_SIZE            0x07

#define AUDIO_DESCRIPTOR_TYPE                         0x21
#define USB_DEVICE_CLASS_AUDIO                        0x01
#define AUDIO_SUBCLASS_AUDIOCONTROL                   0x01
#define AUDIO_SUBCLASS_AUDIOSTREAMING                 0x02
#define AUDIO_PROTOCOL_UNDEFINED                      0x00
#define AUDIO_STREAMING_GENERAL                       0x01
#define AUDIO_STREAMING_FORMAT_TYPE                   0x02

/* Audio Descriptor Types */
#define AUDIO_INTERFACE_DESCRIPTOR_TYPE               0x24
#define AUDIO_ENDPOINT_DESCRIPTOR_TYPE                0x25

/* Audio Control Interface Descriptor Subtypes */
#define AUDIO_CONTROL_HEADER                          0x01
#define AUDIO_CONTROL_INPUT_TERMINAL                  0x02
#define AUDIO_CONTROL_OUTPUT_TERMINAL                 0x03
#define AUDIO_CONTROL_FEATURE_UNIT                    0x06

#define AUDIO_INPUT_TERMINAL_DESC_SIZE                0x0C
#define AUDIO_OUTPUT_TERMINAL_DESC_SIZE               0x09
#define AUDIO_STREAMING_INTERFACE_DESC_SIZE           0x07

#define AUDIO_CONTROL_MUTE                            0x0001
#define AUDIO_CONTROL_VOLUME                          0x02

#define AUDIO_FORMAT_TYPE_I                           0x01
#define AUDIO_FORMAT_TYPE_III                         0x03

#define USB_ENDPOINT_TYPE_ISOCHRONOUS                 0x01
#define AUDIO_ENDPOINT_GENERAL                        0x01

#define AUDIO_REQ_GET_CUR                             0x81
#define AUDIO_REQ_SET_CUR                             0x01

#define AUDIO_REQ_GET_MIN                             0x82
#define AUDIO_REQ_GET_MAX                             0x83
#define AUDIO_REQ_GET_RES                             0x84

#define USB_AUDIO_SET_VOLUME                          0x03
#define USB_AUDIO_SET_MUTE                            0x02
#define USB_AUDIO_SET_FREQ                            0x01

#define AUDIO_OUT_STREAMING_CTRL                      0x02
#define AUDIO_CONTROL_FUNCTION                        0x01
#define AUDIO_ENDPOINT_FUNCTION                       0x02

#define AUDIO_MUTE_CONTROL                0x01
#define AUDIO_VOLUME_CONTROL              0x02


#define AUDIO_FORMAT_TYPE_I                           0x01

#define USB_ENDPOINT_TYPE_ISOCHRONOUS                 0x01
#define AUDIO_ENDPOINT_GENERAL                        0x01


#define AUDIO_MIC_INPUT_TERMINAL_ID                   0x01
#define AUDIO_MIC_OUTPUT_TERMINAL_ID                  0x03
#define AUDIO_MIC_FEATURE_UNIT_ID                     0x02

#define AUDIO_SPK_INPUT_TERMINAL_ID                   0x04
#define AUDIO_SPK_OUTPUT_TERMINAL_ID                  0x06
#define AUDIO_SPK_FEATURE_UNIT_ID                     0x05

#define AUDIO_SIZ_MIC_SPK_EN                        (ENABLE_AT32_USB_SPEAKER + ENABLE_AT32_USB_MICROPHONE)
#define AUDIO_SIZ_FREQ_16_48                        (ENABLE_AT32_USB_AUDIO_FREQ_16K + ENABLE_AT32_USB_AUDIO_FREQ_48K)
#define AUDIO_SIZ_DEVICE_DESC                       18
//#define AUDIO_SIZ_CONFIG_DESC                       0x1A + (AUDIO_SIZ_MIC_SPK_EN) * 0x50
#define AUDIO_SIZ_CONFIG_DESC   0x1A + AUDIO_SIZ_MIC_SPK_EN + \
                                (AUDIO_SIZ_MIC_SPK_EN) * 0x1E + (0x31 + AUDIO_SIZ_FREQ_16_48 * 3) * (AUDIO_SIZ_MIC_SPK_EN) + ENABLE_AT32_AUDIO_FEED_BACK * 9
                        
#define AUDIO_SIZ_INTERFACE_DESC_SIZE               9
#define AUDIO_SIZ_INTERFACE_TOTALLEN                (0x08 + AUDIO_SIZ_MIC_SPK_EN) + \
                                                     (AUDIO_SIZ_MIC_SPK_EN) * 30
/**
  * @}
  */ 


/** @defgroup USBD_CORE_Exported_TypesDefinitions
  * @{
  */
typedef struct _Audio_Fops
{
    uint8_t  (*Init)         (uint32_t  AudioFreq, uint32_t Volume, uint32_t options);
    uint8_t  (*Reset)       (uint32_t options);
    uint8_t  (*AudioCmd)     (uint8_t* pbuf, uint32_t size, uint8_t cmd);
    uint8_t  (*VolumeSpkCtl)    (uint8_t vol);
    uint8_t  (*VolumeMicCtl)    (uint8_t vol);
    uint8_t  (*MuteSpkCtl)      (uint8_t cmd);
    uint8_t  (*MuteMicCtl)      (uint8_t cmd);
    uint8_t  (*FreqCtl)      (uint32_t freq);
    uint32_t (*AudioLen)     (void);
    uint8_t  (*PeriodicTC)   (uint8_t cmd);
    uint8_t  (*GetState)     (void);
}AUDIO_FOPS_TypeDef;
/**
  * @}
  */ 



/** @defgroup USBD_CORE_Exported_Macros
  * @{
  */ 
#define AUDIO_PACKET_SZE(frq)          (uint8_t)(((frq * 2 * 2)/1000) & 0xFF), \
                                       (uint8_t)((((frq * 2 * 2)/1000) >> 8) & 0xFF)
#define SAMPLE_FREQ(frq)               (uint8_t)(frq), (uint8_t)((frq >> 8)), (uint8_t)((frq >> 16))
/**
  * @}
  */ 

/** @defgroup USBD_CORE_Exported_Variables
  * @{
  */ 

extern USBD_Class_cb_Type  AUDIO_cb;

/**
  * @}
  */ 

/** @defgroup USB_CORE_Exported_Functions
  * @{
  */
/**
  * @}
  */ 

#endif  /* __USB_AUDIO_CORE_H_ */
/**
  * @}
  */ 

/**
  * @}
  */ 
  
