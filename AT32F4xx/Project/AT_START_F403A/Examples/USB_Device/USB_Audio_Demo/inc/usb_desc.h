/*
  **************************************************************************
  * File   : usb_desc.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Descriptor Header for Audio Microphone Demo
  **************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_DESC_H
#define __USB_DESC_H

#include "at32_audio_conf.h"
/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/


#define AUDIO_SIZ_MIC_SPK_EN                        (ENABLE_AT32_USB_SPEAKER + ENABLE_AT32_USB_MICROPHONE)
#define AUDIO_SIZ_FREQ_16_48                        (ENABLE_AT32_USB_AUDIO_FREQ_16K + ENABLE_AT32_USB_AUDIO_FREQ_48K)
#define AUDIO_SIZ_DEVICE_DESC                       18
//#define AUDIO_SIZ_CONFIG_DESC                       0x1A + (AUDIO_SIZ_MIC_SPK_EN) * 0x50
#define AUDIO_SIZ_CONFIG_DESC   0x1A + AUDIO_SIZ_MIC_SPK_EN + \
                                (AUDIO_SIZ_MIC_SPK_EN) * 0x1E + (0x31 + AUDIO_SIZ_FREQ_16_48 * 3) * (AUDIO_SIZ_MIC_SPK_EN) + ENABLE_AT32_AUDIO_FEED_BACK * 9
                        
#define AUDIO_SIZ_INTERFACE_DESC_SIZE               9
#define AUDIO_SIZ_INTERFACE_TOTALLEN                (0x08 + AUDIO_SIZ_MIC_SPK_EN) + \
                                                     (AUDIO_SIZ_MIC_SPK_EN) * 30


#define AUDIO_SIZ_STRING_LANGID                     0x04
#define AUDIO_SIZ_STRING_VENDOR                     36
#define AUDIO_SIZ_STRING_PRODUCT                    0x16
#define AUDIO_SIZ_STRING_SERIAL                     0x1A

#define AUDIO_STANDARD_ENDPOINT_DESC_SIZE             0x09
#define AUDIO_STREAMING_ENDPOINT_DESC_SIZE            0x07
/* USB Descriptor Types */
#define USB_DEVICE_DESCRIPTOR_TYPE                    0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE             0x02
#define USB_STRING_DESCRIPTOR_TYPE                    0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE                 0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE                  0x05

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

#define USB_ENDPOINT_TYPE_ISOCHRONOUS                 0x01
#define AUDIO_ENDPOINT_GENERAL                        0x01


#define AUDIO_MIC_INPUT_TERMINAL_ID                   0x01
#define AUDIO_MIC_OUTPUT_TERMINAL_ID                  0x03
#define AUDIO_MIC_FEATURE_UNIT_ID                     0x02

#define AUDIO_SPK_INPUT_TERMINAL_ID                   0x04
#define AUDIO_SPK_OUTPUT_TERMINAL_ID                  0x06
#define AUDIO_SPK_FEATURE_UNIT_ID                     0x05


#define B3VAL(x)  (x & 0xFF), ((x >> 8) & 0xFF), ((x >> 16) & 0xFF)

/* Exported functions ------------------------------------------------------- */
extern const uint8_t Audio_DeviceDescriptor[AUDIO_SIZ_DEVICE_DESC];
extern const uint8_t Audio_ConfigDescriptor[AUDIO_SIZ_CONFIG_DESC];
extern const uint8_t Audio_StringLangID[AUDIO_SIZ_STRING_LANGID];
extern const uint8_t Audio_StringVendor[AUDIO_SIZ_STRING_VENDOR];
extern const uint8_t Audio_StringProduct[AUDIO_SIZ_STRING_PRODUCT];
extern uint8_t Audio_StringSerial[AUDIO_SIZ_STRING_SERIAL];
#endif /* __USB_DESC_H */


