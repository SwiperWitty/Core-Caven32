/**
  ******************************************************************************
  * File   : usbd_conf.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : USB Device configuration file
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_CONF__H__
#define __USBD_CONF__H__

/* Includes ------------------------------------------------------------------*/
#include "usb_conf.h"

/** @defgroup USB_CONF_Exported_Defines
  * @{
  */ 
//#define USBD_CFG_MAX_NUM           2
//#define USBD_ITF_MAX_NUM           2

//#define USB_MAX_STR_DESC_SIZ       64 

#define USBD_SELF_POWERED               

#define USBD_DYNAMIC_DESCRIPTOR_CHANGE_ENABLED 

/** @defgroup UUSB_HID_Class_Layer_Parameter
  * @{
  */
/*Enable Speaker or Microphone, 1 is Enable, 0 is Disable*/
#define ENABLE_AT32_USB_SPEAKER            1
#define ENABLE_AT32_USB_MICROPHONE          1

/*Support 16K freq or 48K Freq, 1 is Enable, 0 is Disable*/
#define ENABLE_AT32_USB_AUDIO_FREQ_16K     0
#define ENABLE_AT32_USB_AUDIO_FREQ_48K     1

/*Support Audio Freq*/
#define AT32_AUDIO_FREQ_16K                16000
#define AT32_AUDIO_FREQ_48K                48000

/*Support Channel*/
#define AT32_AUDIO_STEREO                  2

/*Support bit Wide*/
#define AT32_AUDIO_BITW_16                 16

#define ENABLE_AT32_AUDIO_FEED_BACK        0

#define USB_EDNP_MAX_PACKET 0x120


/*Freq 16K*/
#define AT32_DEFAULT_AUDIO_FREQ            AT32_AUDIO_FREQ_48K
/*Bit Wide 16bit*/
#define AT32_DEFAULT_AUDIO_BITW            AT32_AUDIO_BITW_16
/*Channel 2 channel, stereo*/
#define AT32_DEFAULT_AUDIO_CHR             AT32_AUDIO_STEREO


#define HID_IN_EP                    0x81
#define HID_IN_PACKET                4

#define AUDIO_OUT_EP                    0x01
#define AUDIO_IN_EP                     0x82
#define AUDIO_FEED_EP                   0x83

#define DEFAULT_VOLUME                  65    /* Default volume in % (Mute=0%, Max = 100%) in Logarithmic values.
                                                 To get accurate volume variations, it is possible to use a logarithmic
                                                 conversion table to convert from percentage to logarithmic law.
                                                 In order to keep this example code simple, this conversion is not used.*/
                                                 
/* Use this section to modify the number of supported interfaces and configurations.
   Note that if you modify these parameters, you have to modify the descriptors 
   accordingly in usbd_audio_core.c file */
#define AUDIO_TOTAL_IF_NUM              0x02
#define USBD_CFG_MAX_NUM                1
#define USBD_ITF_MAX_NUM                2
#define USB_MAX_STR_DESC_SIZ            200 

#define USBD_SELF_POWERED     

#define USBD_AUDIO_FREQ                16000  /* Audio frequency in Hz */

//#define APP_FOPS                        CDC_vcp_fops
/**
  * @}
  */ 

/** @defgroup USB_CONF_Exported_Types
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USB_CONF_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USB_CONF_Exported_Variables
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USB_CONF_Exported_FunctionsPrototype
  * @{
  */ 
/**
  * @}
  */ 


#endif //__USBD_CONF__H__


