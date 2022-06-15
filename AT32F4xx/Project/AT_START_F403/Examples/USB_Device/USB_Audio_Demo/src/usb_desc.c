/**
  ******************************************************************************
  * File   : USB_Device/USB_Audio_Demo/src/usb_desc.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description for USB Audio Demo
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"
#include "at32_audio_conf.h"

/** @addtogroup AT32F413_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_Audio_Demo
  * @{
  */ 
/* USB Standard Device Descriptor */
const uint8_t Audio_DeviceDescriptor[] =
  {
    AUDIO_SIZ_DEVICE_DESC,          /* bLength */
    USB_DEVICE_DESCRIPTOR_TYPE,           /* bDescriptorType */
    0x00,          /* 2.00 */             /* bcdUSB */
    0x02,
    0x00,                                 /* bDeviceClass */
    0x00,                                 /* bDeviceSubClass */
    0x00,                                 /* bDeviceProtocol */
    0x40,                                 /* bMaxPacketSize 40 */
    0x3C,                                 /* idVendor */
    0x2E,
    0x30,                                 /* idProduct  = 0x5730*/
    0x57,
    0x00,          /* 2.00 */             /* bcdDevice */
    0x02,
    1,                                    /* iManufacturer */
    2,                                    /* iProduct */
    3,                                    /* iSerialNumber */
    0x01                                  /* bNumConfigurations */
  };
  
  
/* USB Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
const uint8_t Audio_ConfigDescriptor[] =
  {
    /* Configuration 1 */
    0x09,                                 /* bLength */
    USB_CONFIGURATION_DESCRIPTOR_TYPE,    /* bDescriptorType */
    AUDIO_SIZ_CONFIG_DESC & 0xFF,         /* wTotalLength  110 bytes*/
    (AUDIO_SIZ_CONFIG_DESC) >> 8,
    0x01 + AUDIO_SIZ_MIC_SPK_EN,          /* bNumInterfaces */
    0x01,                                 /* bConfigurationValue */
    0x00,                                 /* iConfiguration */
    0xC0,                                 /* bmAttributes Self Powred*/
    0x32,                                 /* bMaxPower = 100 mA*/
    /* 09 byte*/
    
    /*Standard AC Interface Descriptor*/
    AUDIO_SIZ_INTERFACE_DESC_SIZE,      /* bLength */
    USB_INTERFACE_DESCRIPTOR_TYPE,        /* bDescriptorType */
    0x00,                                 /* bInterfaceNumber */
    0x00,                                 /* bAlternateSetting */
    0x00,                                 /* bNumEndpoints */
    USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
    AUDIO_SUBCLASS_AUDIOCONTROL,          /* bInterfaceSubClass */
    AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */
    0x00,                                 /* iInterface */
    /* 18 byte*/

    /* USB Audio Class-specific AC Interface Descriptor */
    0x08 + AUDIO_SIZ_MIC_SPK_EN,        /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
    AUDIO_CONTROL_HEADER,                 /* bDescriptorSubtype */
    0x00,          /* 1.00 */             /* bcdADC */
    0x01,
    AUDIO_SIZ_INTERFACE_TOTALLEN,         /* wTotalLength = 39*/
    0x00,

    AUDIO_SIZ_MIC_SPK_EN ,    /* bInCollection */
#if (ENABLE_AT32_USB_SPEAKER && ENABLE_AT32_USB_MICROPHONE)
    0x02,                                 /* baInterfaceNr 2*/
    0x01,                                 /* baInterfaceNr 1*/
#else
    0x01,                                 /* baInterfaceNr */
#endif
     /* 27 / 28 */

    /************************************ Config USB Microphone *****************************************/
    /*length = (AUDIO_SIZ_MIC_SPK_EN * 0x1E*/
#if ( ENABLE_AT32_USB_MICROPHONE == 1 )   /*30Byte*/
    /* USB Microphone Input Terminal Descriptor */
    AUDIO_INPUT_TERMINAL_DESC_SIZE,       /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
    AUDIO_CONTROL_INPUT_TERMINAL,         /* bDescriptorSubtype */
    AUDIO_MIC_INPUT_TERMINAL_ID,          /* bTerminalID */
    0x01,                                 /* wTerminalType Generic Microphone 0x0201 */
    0x02,
    0x00,                                 /* bAssocTerminal */
    0x02,                                 /* bNrChannels */
    0x03,                                 /* wChannelConfig 0x0000  Mono */
    0x00,
    0x00,                                 /* iChannelNames */
    0x00,                                 /* iTerminal */
    /* 12 byte*/

    /* USB Microphone Audio Feature Unit Descriptor */
    0x09,                                 /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
    AUDIO_CONTROL_FEATURE_UNIT,           /* bDescriptorSubtype */
    AUDIO_MIC_FEATURE_UNIT_ID,            /* bUnitID */
    AUDIO_MIC_INPUT_TERMINAL_ID,          /* bSourceID */
    0x01,                                 /* bControlSize */
    AUDIO_CONTROL_MUTE,                   /* bmaControls(0) */
    AUDIO_CONTROL_VOLUME,                 /* bmaControls(1) */
    0x00,                                 /* iTerminal */
    /* 21 byte*/

    /*USB Microphone Output Terminal Descriptor */
    0x09,      /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
    AUDIO_CONTROL_OUTPUT_TERMINAL,        /* bDescriptorSubtype */
    AUDIO_MIC_OUTPUT_TERMINAL_ID,         /* bTerminalID */
    0x01,                                 /* wTerminalType  0x0101*/
    0x01,
    0x00,                                 /* bAssocTerminal */
    AUDIO_MIC_FEATURE_UNIT_ID,            /* bSourceID */
    0x00,                                 /* iTerminal */
    /* 30 byte*/
 #endif   
    /************************************ End Config USB Microphone *****************************************/
    
    /************************************ Config USB Speaker *****************************************/
#if ( ENABLE_AT32_USB_SPEAKER == 1 )
    /* USB Speaker Input Terminal Descriptor */
    AUDIO_INPUT_TERMINAL_DESC_SIZE,       /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
    AUDIO_CONTROL_INPUT_TERMINAL,         /* bDescriptorSubtype */
    AUDIO_SPK_INPUT_TERMINAL_ID,          /* bTerminalID */
    0x01,                                 /* wTerminalType:USB Streaming 0x0101*/
    0x01,
    0x00,                                 /* bAssocTerminal */
    0x02,                                 /* bNrChannels */
    0x03,                                 /* wChannelConfig 0x0003  Right and left */
    0x00,
    0x00,                                 /* iChannelNames */
    0x00,                                 /* iTerminal */
    /* 12 byte*/

    /* USB Speaker Audio Feature Unit Descriptor */
    0x09,                                 /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
    AUDIO_CONTROL_FEATURE_UNIT,           /* bDescriptorSubtype */
    AUDIO_SPK_FEATURE_UNIT_ID,            /* bUnitID */
    AUDIO_SPK_INPUT_TERMINAL_ID,          /* bSourceID */
    0x01,                                 /* bControlSize */
    AUDIO_CONTROL_MUTE,                   /* bmaControls(0) */
    AUDIO_CONTROL_VOLUME,                 /* bmaControls(1) */
    0x00,                                 /* iTerminal */
    /* 21 byte*/

    /*USB Speaker Output Terminal Descriptor */
    0x09,      /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
    AUDIO_CONTROL_OUTPUT_TERMINAL,        /* bDescriptorSubtype */
    AUDIO_SPK_OUTPUT_TERMINAL_ID,         /* bTerminalID */
    0x01,                                 /* USB Streaming 0x0301*/
    0x03,
    0x00,                                 /* bAssocTerminal */
    AUDIO_SPK_FEATURE_UNIT_ID,            /* bSourceID */
    0x00,                                 /* iTerminal */
    /* 30 byte*/
 #endif   
   /************************************ End Config USB Speaker *****************************************/  
    
   /*****************************Microphone Audio Streaming Interface Descriptor*********************************/   
#if ( ENABLE_AT32_USB_MICROPHONE == 1 )   
     /*
     
     Length = 9 + 9 + 7 +  0x08 + (ENABLE_AT32_USB_AUDIO_FREQ_16K + ENABLE_AT32_USB_AUDIO_FREQ_48K) * 3 + 9 + 7
     (0x31 + AUDIO_SIZ_FREQ_16_48 * 3) * (AUDIO_SIZ_MIC_SPK_EN)
     */
    /*Standard AS Interface Descriptor*/
    AUDIO_SIZ_INTERFACE_DESC_SIZE,           /* bLength*/                           
    USB_INTERFACE_DESCRIPTOR_TYPE,           /* bDescriptorType*/         
    0x01,                                    /* bInterfaceNumber*/                  
    0x00,                                    /* bAlternateSetting*/               
    0x00,                                    /* bNumEndpoint*/                   
    USB_DEVICE_CLASS_AUDIO,                  /* bInterfaceClass*/            
    AUDIO_SUBCLASS_AUDIOSTREAMING,           /* bInterfaceSubClass*/
    0x00,                                    /* bInterfaceProtocol*/                
    0x00,                                    /* iInterface*/            
    /*9 byte*/
    
    /*Standard AS Interface Descriptor*/
    AUDIO_SIZ_INTERFACE_DESC_SIZE,           /* bLength*/                           
    USB_INTERFACE_DESCRIPTOR_TYPE,           /* bDescriptorType*/         
    0x01,                                    /* bInterfaceNumber*/                  
    0x01,                                    /* bAlternateSetting*/     
    0x01,
    USB_DEVICE_CLASS_AUDIO,                  /* bInterfaceClass*/            
    AUDIO_SUBCLASS_AUDIOSTREAMING,           /* bInterfaceSubClass*/
    0x00,                                    /* bInterfaceProtocol*/                
    0x00,
    /*18 byte*/
        /*Class-Specific AS Interface Descriptor*/
    AUDIO_STREAMING_INTERFACE_DESC_SIZE,      /* bLength*/                                      
    AUDIO_INTERFACE_DESCRIPTOR_TYPE,          /* bDescriptorType*/  
    AUDIO_STREAMING_GENERAL,                  /* bDescriptorSubType*/               
    0x03,                                     /* bTerminalLink:3*/           
    0x01,                                     /* bDelay*/                                   
    0x01,
    0x00,                                     /* wFormatTag:PCM-->0x0001*/      
    /*25 byte*/
    
    /* USB Microphone Audio Type I Format Interface Descriptor */
    0x08 + (AUDIO_SIZ_FREQ_16_48) * 3,                                 /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
    AUDIO_STREAMING_FORMAT_TYPE,          /* bDescriptorSubtype */
    AUDIO_FORMAT_TYPE_I,                  /* bFormatType */
    AT32_DEFAULT_AUDIO_CHR,               /* bNrChannels */
    AT32_DEFAULT_AUDIO_BITW/8,            /* bSubFrameSize */
    AT32_DEFAULT_AUDIO_BITW,              /* bBitResolution */
    AUDIO_SIZ_FREQ_16_48,                                 /* bSamFreqType */
#if (ENABLE_AT32_USB_AUDIO_FREQ_16K == 1)
    B3VAL(AT32_AUDIO_FREQ_16K),
#endif
#if (ENABLE_AT32_USB_AUDIO_FREQ_48K == 1)
    B3VAL(AT32_AUDIO_FREQ_48K),
#endif
    
        /*Endpoint Standard AS Isochronous Audio Data Endpoint Descriptor*/
    AUDIO_STANDARD_ENDPOINT_DESC_SIZE,          /* bLength*/                                                             
    USB_ENDPOINT_DESCRIPTOR_TYPE,               /* bDescriptorType*/                                 
    AUDIO_IN_EP,                                /* bEndpointAddress*/                                
    0x05,                                       /* bAttributes*/
    USB_EDNP_MAX_PACKET & 0xFF,                 /* wMaxPacketSize 288 bytes*/
    USB_EDNP_MAX_PACKET >> 8,                                      
    0x01,                                       /* bInterval*/
    0x00,                                       /* bRefresh*/
    0x00,                                       /* bSynchAddress*/
    /*9 byte*/
                  

    /*EndPoint Audio Steaming Descriptor*/    
    AUDIO_STREAMING_ENDPOINT_DESC_SIZE,          /* bLength*/                                   
    AUDIO_ENDPOINT_DESCRIPTOR_TYPE,              /* bDescriptorType*/  
    AUDIO_ENDPOINT_GENERAL,                      /* bDescriptorSubType*/ 
    0x01,                                        /* bAttributes*/            
    0x00,                                        /* LockDelayUnits*/                            
    0x00,
    0x00,  
#endif   
   /*****************************Microphone Audio Streaming Interface Descriptor*********************************/      


   /*****************************Speaker Audio Streaming Interface Descriptor*********************************/   
#if ( ENABLE_AT32_USB_SPEAKER == 1 )   

     /*
     
     Length = 9 + 9 + 7 +  0x08 + (ENABLE_AT32_USB_AUDIO_FREQ_16K + ENABLE_AT32_USB_AUDIO_FREQ_48K) * 3 + 9 + 7
     0x31 + (ENABLE_AT32_USB_AUDIO_FREQ_16K + ENABLE_AT32_USB_AUDIO_FREQ_48K) * 3
     */
        /*Standard AS Interface Descriptor*/
    AUDIO_SIZ_INTERFACE_DESC_SIZE,           /* bLength*/                           
    USB_INTERFACE_DESCRIPTOR_TYPE,           /* bDescriptorType*/ 
#if ( ENABLE_AT32_USB_MICROPHONE == 1 )      
    0x02,                                    /* bInterfaceNumber*/ 
#else
    0x01,
#endif
    0x00,                                    /* bAlternateSetting*/               
    0x00,                                    /* bNumEndpoint*/                   
    USB_DEVICE_CLASS_AUDIO,                  /* bInterfaceClass*/            
    AUDIO_SUBCLASS_AUDIOSTREAMING,           /* bInterfaceSubClass*/
    0x00,                                    /* bInterfaceProtocol*/                
    0x00,                                    /* iInterface*/            
    /*9 byte*/
    
    /*Standard AS Interface Descriptor*/
    AUDIO_SIZ_INTERFACE_DESC_SIZE,           /* bLength*/                           
    USB_INTERFACE_DESCRIPTOR_TYPE,           /* bDescriptorType*/         
#if ( ENABLE_AT32_USB_MICROPHONE == 1 )      
    0x02,                                    /* bInterfaceNumber*/ 
#else
    0x01,
#endif                 
    0x01,                                    /* bAlternateSetting*/   
    0x01+ENABLE_AT32_AUDIO_FEED_BACK,
    USB_DEVICE_CLASS_AUDIO,                  /* bInterfaceClass*/            
    AUDIO_SUBCLASS_AUDIOSTREAMING,           /* bInterfaceSubClass*/
    0x00,                                    /* bInterfaceProtocol*/                
    0x00,
    /*9 byte*/
        /*Class-Specific AS Interface Descriptor*/
    AUDIO_STREAMING_INTERFACE_DESC_SIZE,      /* bLength*/                                      
    AUDIO_INTERFACE_DESCRIPTOR_TYPE,          /* bDescriptorType*/  
    AUDIO_STREAMING_GENERAL,                  /* bDescriptorSubType*/               
    0x04,                                     /* bTerminalLink:4*/           
    0x01,                                     /* bDelay*/                                   
    0x01,
    0x00,                                     /* wFormatTag:PCM-->0x0001*/      
    /*7 byte*/
    
    /* USB Microphone Audio Type I Format Interface Descriptor */
    0x08 + (AUDIO_SIZ_FREQ_16_48) * 3,                                 /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
    AUDIO_STREAMING_FORMAT_TYPE,          /* bDescriptorSubtype */
    AUDIO_FORMAT_TYPE_I,                  /* bFormatType */
    AT32_DEFAULT_AUDIO_CHR,               /* bNrChannels */
    AT32_DEFAULT_AUDIO_BITW/8,            /* bSubFrameSize */
    AT32_DEFAULT_AUDIO_BITW,              /* bBitResolution */
    AUDIO_SIZ_FREQ_16_48,                                 /* bSamFreqType */
#if (ENABLE_AT32_USB_AUDIO_FREQ_16K == 1)
    B3VAL(AT32_AUDIO_FREQ_16K),
#endif
#if (ENABLE_AT32_USB_AUDIO_FREQ_48K == 1)
    B3VAL(AT32_AUDIO_FREQ_48K),
#endif
    
        /*Endpoint Standard AS Isochronous Audio Data Endpoint Descriptor*/
    AUDIO_STANDARD_ENDPOINT_DESC_SIZE,          /* bLength*/                                                             
    USB_ENDPOINT_DESCRIPTOR_TYPE,               /* bDescriptorType*/                                 
    AUDIO_OUT_EP,                                /* bEndpointAddress*/                                
    0x05,                                       /* bAttributes*/
    USB_EDNP_MAX_PACKET & 0xFF,                 /* wMaxPacketSize 288 bytes*/
    USB_EDNP_MAX_PACKET >> 8,                                      
    0x01,                                       /* bInterval*/
    0x00,                                       /* bRefresh*/
#if (ENABLE_AT32_AUDIO_FEED_BACK == 1 )
    AUDIO_FEED_EP,                                       /* bSynchAddress*/
#else
    0x00,
#endif
    /*9 byte*/
                  

    /*EndPoint Audio Steaming Descriptor*/    
    AUDIO_STREAMING_ENDPOINT_DESC_SIZE,          /* bLength*/                                   
    AUDIO_ENDPOINT_DESCRIPTOR_TYPE,              /* bDescriptorType*/  
    AUDIO_ENDPOINT_GENERAL,                      /* bDescriptorSubType*/ 
    0x01,                                        /* bAttributes*/            
    0x00,                                        /* LockDelayUnits*/                            
    0x00,
    0x00,  
    
#if (ENABLE_AT32_AUDIO_FEED_BACK == 1 )
    AUDIO_STANDARD_ENDPOINT_DESC_SIZE,
    USB_ENDPOINT_DESCRIPTOR_TYPE,
    AUDIO_FEED_EP,
    0x11,
    0x03,
    0x00,
    0x01,
    0x08,
    0x00

#endif
#endif   
   /*****************************End Speaker Audio Streaming Interface Descriptor*********************************/  

  };

/* USB String Descriptor (optional) */
const uint8_t Audio_StringLangID[AUDIO_SIZ_STRING_LANGID] =
  {
    0x04,
    0x03,
    0x09,
    0x04
  }
  ; /* LangID = 0x0409: U.S. English */

const uint8_t Audio_StringVendor[AUDIO_SIZ_STRING_VENDOR] =
  {
    AUDIO_SIZ_STRING_VENDOR, /* Size of manufacturer string */
    USB_STRING_DESCRIPTOR_TYPE,  /* bDescriptorType*/
    'A', 0, 'r', 0, 't', 0, 'e', 0, 'r', 0, 'y', 0, ' ', 0, 
    'T', 0, 'e', 0, 'c', 0, 'h', 0, 'n', 0, 'o', 0, 'l', 0, 'o', 0, 'g', 0, 'y', 0,
  };
/*Audio*/
const uint8_t Audio_StringProduct[AUDIO_SIZ_STRING_PRODUCT] =
  {
    AUDIO_SIZ_STRING_PRODUCT,  /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
    'A', 0, 'T', 0, '3', 0, '2', 0, ' ', 0,
    'A', 0, 'u', 0, 'd', 0, 'i', 0, 'o', 0
  };

uint8_t Audio_StringSerial[AUDIO_SIZ_STRING_SERIAL] =
  {
    AUDIO_SIZ_STRING_SERIAL,  /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
    'A', 0, 'T', 0, '3', 0, '2', 0
  };
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
  
/**
  * @}
  */

/**
  * @}
  */  
  

  

