 /**
  ******************************************************************************
  * File   : usbd_audio_core.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  :  This file provides the high layer firmware functions to manage the 
  *           functionalities of the USB Audio Class:
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/

#include "usbd_audio_core.h"
#include "usbd_audio_out_if.h"
#include "wm8988.h"

/** @addtogroup AT32_USB_OTG_DEVICE_LIBRARY
  * @{
  */


/** @defgroup usbd_audio 
  * @brief usbd core module
  * @{
  */ 

/** @defgroup usbd_audio_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup usbd_audio_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup usbd_audio_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup usbd_audio_Private_FunctionPrototypes
  * @{
  */

/*********************************************
   AUDIO Device library callbacks
 *********************************************/
static uint8_t  usbd_audio_Init       (void  *pusbdev, uint8_t cfgidx);
static uint8_t  usbd_audio_DeInit     (void  *pusbdev, uint8_t cfgidx);
static uint8_t  usbd_audio_Setup      (void  *pusbdev, USB_SETUP_REQEST *req);
static uint8_t  usbd_audio_EP0_RxReady(void *pusbdev);
static uint8_t  usbd_audio_DataIn     (void *pusbdev, uint8_t bepnum);
static uint8_t  usbd_audio_DataOut    (void *pusbdev, uint8_t bepnum);
static uint8_t  usbd_audio_SOF        (void *pusbdev);
static uint8_t  usbd_audio_OUT_Incplt (void  *pusbdev);
static uint8_t  usbd_audio_IN_Incplt (void  *pusbdev);

/*********************************************
   AUDIO Requests management functions
 *********************************************/
static void AUDIO_Req_GetCurrent(void *pusbdev, USB_SETUP_REQEST *req);
static void AUDIO_Req_SetCurrent(void *pusbdev, USB_SETUP_REQEST *req);
static void AUDIO_Req_GetRes(void *pusbdev, USB_SETUP_REQEST *req);
static void AUDIO_Req_GetMax(void *pusbdev, USB_SETUP_REQEST *req);
static void AUDIO_Req_GetMin(void *pusbdev, USB_SETUP_REQEST *req);
static uint8_t  *USBD_audio_GetCfgDesc (uint8_t speed, uint16_t *length);
/**
  * @}
  */ 

/** @defgroup usbd_audio_Private_Variables
  * @{
  */ 
/* Main Buffer for Audio Data Out transfers and its relative pointers */
uint8_t  IsocOutBuff [TOTAL_OUT_BUF_SIZE * 2];
uint8_t* IsocOutWrPtr = IsocOutBuff;
uint8_t* IsocOutRdPtr = IsocOutBuff;

/* Main Buffer for Audio Control Requests transfers and its relative variables */
uint8_t  AudioCtl[64];
uint8_t  AudioCtlCmd = 0;
uint32_t AudioCtlLen = 0;
uint8_t  AudioCtlUnit = 0;

/*USB audio control struct*/
typedef struct __USB_Audio_Control_
{
    uint8_t u8Enpd;
    uint8_t u8Interface;
    uint8_t u8RquestNo;
    uint8_t u8SpkMute;
    uint8_t u8MicMute;
    uint16_t u16Volume;
    uint16_t u16MicVolume;
    uint32_t u32Freq;
    uint16_t u16SpkVolumeLimits[3]; /*[0] is mininum value, [1] is maxnum value, [2] is volume resolution */
    uint16_t u16MicVolumeLimits[3]; /*[0] is mininum value, [1] is maxnum value, [2] is volume resolution */
    
}USB_Audio_Control;

USB_Audio_Control usb_audio = {0, 0, 0, 0, 0, 0x1400, 0, 0, {0x0000, 0x1400, 0x0400}, {0x0000, 0x1400, 0x0400}};

//static uint32_t PlayFlag = 0;

static __IO uint32_t  usbd_audio_AltSet = 0;
//static uint8_t usbd_audio_CfgDesc[AUDIO_CONFIG_DESC_SIZE];
//uint8_t usbd_audio_CfgDesc[AUDIO_CONFIG_DESC_SIZE]

/* AUDIO interface class callbacks structure */
USBD_Class_cb_Type  AUDIO_cb = 
{
  usbd_audio_Init,
  usbd_audio_DeInit,
  usbd_audio_Setup,
  NULL, /* EPT0_TxSent */
  usbd_audio_EP0_RxReady,
  usbd_audio_DataIn,
  usbd_audio_DataOut,
  usbd_audio_SOF,
  usbd_audio_IN_Incplt,
  usbd_audio_OUT_Incplt,   
  USBD_audio_GetCfgDesc,
#ifdef USB_OTG_HS_CORE  
  USBD_audio_GetCfgDesc, /* use same config as per FS */
#endif    
};
static uint8_t usbd_audio_CfgDesc[AUDIO_SIZ_CONFIG_DESC] =
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
    SAMPLE_FREQ(AT32_AUDIO_FREQ_16K),
#endif
#if (ENABLE_AT32_USB_AUDIO_FREQ_48K == 1)
    SAMPLE_FREQ(AT32_AUDIO_FREQ_48K),
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
    SAMPLE_FREQ(AT32_AUDIO_FREQ_16K),
#endif
#if (ENABLE_AT32_USB_AUDIO_FREQ_48K == 1)
    SAMPLE_FREQ(AT32_AUDIO_FREQ_48K),
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

/**
  * @}
  */ 

/** @defgroup usbd_audio_Private_Functions
  * @{
  */ 

/**
* @brief  usbd_audio_Init
*         Initializes the AUDIO interface.
* @param  pusbdev: device instance
* @param  cfgidx: Configuration index
* @retval status
*/
static uint8_t  usbd_audio_Init (void  *pusbdev, 
                                 uint8_t cfgidx)
{  
  /* Open EP OUT */
  USB_DCD_EPT_Open(pusbdev,
              AUDIO_OUT_EP,
              AUDIO_MAX_PACKET,
              USB_OTG_EPT_ISOC);
  USB_DCD_EPT_Open(pusbdev,
            AUDIO_IN_EP,
            AUDIO_MAX_PACKET,
            USB_OTG_EPT_ISOC);
//  USB_DCD_EPT_Open(pusbdev,
//            AUDIO_FEED_EP,
//            AUDIO_OUT_PACKET,
//            USB_OTG_EPT_ISOC);

  /* Initialize the Audio output Hardware layer */
  if (AUDIO_OUT_fops.Init(USBD_AUDIO_FREQ, DEFAULT_VOLUME, 0) != USB_DEVICE_OK)
  {
    return USB_DEVICE_FAIL;
  }
    
  /* Prepare Out endpoint to receive audio data */
  USB_DCD_EPT_PrepareRx(pusbdev,
                   AUDIO_OUT_EP,
                   (uint8_t*)IsocOutBuff,                        
                   (uint16_t)AUDIO_OUT_fops.AudioLen());  
  USB_DCD_EPT_Tx(pusbdev, AUDIO_IN_EP, WM8988_GetMicrophoneBuffer(),  WM8988_GetMicrophoneFrameLen());
  return USB_DEVICE_OK;
}

/**
* @brief  usbd_audio_Init
*         DeInitializes the AUDIO layer.
* @param  pusbdev: device instance
* @param  cfgidx: Configuration index
* @retval status
*/
static uint8_t  usbd_audio_DeInit (void  *pusbdev, 
                                   uint8_t cfgidx)
{ 
  USB_DCD_EPT_Close (pusbdev , AUDIO_OUT_EP);
  USB_DCD_EPT_Close (pusbdev , AUDIO_IN_EP);
  
  /* DeInitialize the Audio output Hardware layer */
  if (AUDIO_OUT_fops.Reset(0) != USB_DEVICE_OK)
  {
    return USB_DEVICE_FAIL;
  }
  
  return USB_DEVICE_OK;
}

/**
  * @brief  usbd_audio_Setup
  *         Handles the Audio control request parsing.
  * @param  pusbdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t  usbd_audio_Setup (void  *pusbdev, 
                                  USB_SETUP_REQEST *req)
{
  uint16_t len=USB_AUDIO_DESC_SIZ;
  uint8_t  *pbuf=usbd_audio_CfgDesc + 18;
  
  switch (req->bmRequest & USB_REQUEST_TYPE_MASK)
  {
    /* AUDIO Class Requests -------------------------------*/
  case USB_REQUEST_TYPE_CLASS :    
    switch (req->bRequest)
    {
    case AUDIO_REQ_GET_CUR:
      AUDIO_Req_GetCurrent(pusbdev, req);
      break;
      
    case AUDIO_REQ_SET_CUR:
      AUDIO_Req_SetCurrent(pusbdev, req);   
      break;
    case AUDIO_REQ_GET_MIN:
      AUDIO_Req_GetMin(pusbdev, req);
      break;
    case AUDIO_REQ_GET_MAX:
      AUDIO_Req_GetMax(pusbdev, req);
      break;
    case AUDIO_REQ_GET_RES:
      AUDIO_Req_GetRes(pusbdev, req);
      break;
    default:
      USB_Device_CtlError (pusbdev, req);
      return USB_DEVICE_FAIL;
    }
    break;
    
    /* Standard Requests -------------------------------*/
  case USB_REQUEST_TYPE_STANDARD:
    switch (req->bRequest)
    {
    case USB_REQUEST_GET_DESCRIPTOR: 
      if( (req->wValue >> 8) == AUDIO_DESCRIPTOR_TYPE)
      {
        pbuf = usbd_audio_CfgDesc + 18;
        len = MIN(USB_AUDIO_DESC_SIZ , req->wLength);
      }
      
      USB_Device_CtrlSendData (pusbdev, 
                        pbuf,
                        len);
      break;
      
    case USB_REQUEST_GET_INTERFACE :
      USB_Device_CtrlSendData (pusbdev,
                        (uint8_t *)&usbd_audio_AltSet,
                        1);
      break;
      
    case USB_REQUEST_SET_INTERFACE :
      if ((uint8_t)(req->wValue) <= AUDIO_TOTAL_IF_NUM)
      {
        usbd_audio_AltSet = (uint8_t)(req->wValue);
        
      }
      else
      {
        /* Call the error management function (command will be nacked */
        USB_Device_CtlError (pusbdev, req);
      }
      break;
    }
  }
  return USB_DEVICE_OK;
}

/**
  * @brief  usbd_audio_EP0_RxReady
  *         Handles audio control requests data.
  * @param  pusbdev: device instance
  * @retval status
  */
static uint8_t  usbd_audio_EP0_RxReady (void  *pusbdev)
{ 
  uint8_t fVol = 0;
  /* Check if an AudioControl request has been issued */
  if (AudioCtlCmd == AUDIO_REQ_SET_CUR)
  {/* In this driver, to simplify code, only SET_CUR request is managed */
    /* Check for which addressed unit the AudioControl request has been issued */
//    if (AudioCtlUnit == AUDIO_OUT_STREAMING_CTRL)
//    {/* In this driver, to simplify code, only one unit is manage */
//      /* Call the audio interface mute function */
//      AUDIO_OUT_fops.MuteCtl(AudioCtl[0]);
//      
//      /* Reset the AudioCtlCmd variable to prevent re-entering this function */
//      AudioCtlCmd = 0;
//      AudioCtlLen = 0;
//    }
    switch (usb_audio.u8RquestNo)
    {
      case USB_AUDIO_SET_VOLUME:
        if ( usb_audio.u8Interface == AUDIO_SPK_FEATURE_UNIT_ID )
        {
          usb_audio.u16Volume = (uint16_t)(AudioCtl[0] | AudioCtl[1] << 8);
          fVol = (uint8_t)(usb_audio.u16Volume * 127 / usb_audio.u16SpkVolumeLimits[1]);
          AUDIO_OUT_fops.VolumeSpkCtl(fVol);
          /*Set Volume*/
        }
        if ( usb_audio.u8Interface == AUDIO_MIC_FEATURE_UNIT_ID )
        {
          usb_audio.u16MicVolume = (uint16_t)(AudioCtl[0] | AudioCtl[0] << 8);
          fVol = (uint8_t)(usb_audio.u16MicVolume * 127 / usb_audio.u16MicVolumeLimits[1]);
          AUDIO_OUT_fops.VolumeMicCtl(fVol);
        }
        break;
      case USB_AUDIO_SET_MUTE:
        if ( usb_audio.u8Interface == AUDIO_SPK_FEATURE_UNIT_ID )
        {
          usb_audio.u8SpkMute = AudioCtl[0];
          AUDIO_OUT_fops.MuteSpkCtl(usb_audio.u8SpkMute);
        }
        if ( usb_audio.u8Interface == AUDIO_MIC_FEATURE_UNIT_ID )
        {
          usb_audio.u8MicMute = AudioCtl[0];
          AUDIO_OUT_fops.MuteMicCtl(usb_audio.u8MicMute);
        }
        break;
      case USB_AUDIO_SET_FREQ:
        //audio set freq
         usb_audio.u32Freq = AudioCtl[0] | AudioCtl[1] << 8 |  AudioCtl[2] << 16;
         AUDIO_OUT_fops.FreqCtl(usb_audio.u32Freq);
        break;
      default:
        break;
      
    }
  } 
  
  return USB_DEVICE_OK;
}

/**
  * @brief  usbd_audio_DataIn
  *         Handles the audio IN data stage.
  * @param  pusbdev: instance
  * @param  bepnum: endpoint number
  * @retval status
  */
static uint8_t  usbd_audio_DataIn (void *pusbdev, uint8_t bepnum)
{
  USB_DCD_EPT_Tx(pusbdev, AUDIO_IN_EP, WM8988_GetMicrophoneBuffer(),  WM8988_GetMicrophoneFrameLen());
  return USB_DEVICE_OK;
}

/**
  * @brief  usbd_audio_DataOut
  *         Handles the Audio Out data stage.
  * @param  pusbdev: instance
  * @param  bepnum: endpoint number
  * @retval status
  */
__IO uint32_t DataOutCounter = 0;
static uint8_t  usbd_audio_DataOut (void *pusbdev, uint8_t bepnum)
{     
  DataOutCounter++;
  if (bepnum == AUDIO_OUT_EP)
  { 
    WM8988_WriteSpeakerBuffer(IsocOutWrPtr, (uint16_t)AUDIO_OUT_fops.AudioLen());
    /* Increment the Buffer pointer or roll it back when all buffers are full */
//    if (IsocOutWrPtr >= (IsocOutBuff + (AUDIO_OUT_PACKET * OUT_PACKET_NUM)))
//    {/* All buffers are full: roll back */
//      IsocOutWrPtr = IsocOutBuff;
//    }
//    else
//    {/* Increment the buffer pointer */
//      IsocOutWrPtr += AUDIO_OUT_PACKET;
//    }
//    
    /* Toggle the frame index */  
    ((USB_OTG_CORE_HANDLE*)pusbdev)->dev.out_ept[bepnum].even_odd_frame = 
      (((USB_OTG_CORE_HANDLE*)pusbdev)->dev.out_ept[bepnum].even_odd_frame)? 0:1;
      
    /* Prepare Out endpoint to receive next audio packet */
    USB_DCD_EPT_PrepareRx(pusbdev,
                     AUDIO_OUT_EP,
                     (uint8_t*)(IsocOutWrPtr),
                     (uint16_t)AUDIO_OUT_fops.AudioLen());
      
    /* Trigger the start of streaming only when half buffer is full */
//    if ((PlayFlag == 0) && (IsocOutWrPtr >= (IsocOutBuff + ((AUDIO_OUT_PACKET * OUT_PACKET_NUM) / 2))))
//    {
//      /* Enable start of Streaming */
//      PlayFlag = 1;
//    }
  }
  
  return USB_DEVICE_OK;
}

/**
  * @brief  usbd_audio_SOF
  *         Handles the SOF event (data buffer update and synchronization).
  * @param  pusbdev: instance
  * @param  bepnum: endpoint number
  * @retval status
  */
static uint8_t  usbd_audio_SOF (void *pusbdev)
{ 
//  static uint8_t audio_in = 0;  
//  /* Check if there are available data in stream buffer.
//    In this function, a single variable (PlayFlag) is used to avoid software delays.
//    The play operation must be executed as soon as possible after the SOF detection. */
//  if (PlayFlag)
//  {      
//    if ( audio_in == 0 )
//      USB_DCD_EPT_Tx(pusbdev, AUDIO_IN_EP, WM8988_GetMicrophoneBuffer(),  WM8988_GetMicrophoneFrameLen());
//    audio_in = 1;
//    /* Start playing received packet */
//    AUDIO_OUT_fops.AudioCmd((uint8_t*)(IsocOutRdPtr),  /* Samples buffer pointer */
//                            AUDIO_OUT_PACKET,          /* Number of samples in Bytes */
//                            AUDIO_CMD_PLAY);           /* Command to be processed */
//    
//    /* Increment the Buffer pointer or roll it back when all buffers all full */  
//    if (IsocOutRdPtr >= (IsocOutBuff + (AUDIO_OUT_PACKET * OUT_PACKET_NUM)))
//    {/* Roll back to the start of buffer */
//      IsocOutRdPtr = IsocOutBuff;
//    }
//    else
//    {/* Increment to the next sub-buffer */
//      IsocOutRdPtr += AUDIO_OUT_PACKET;
//    }
//    
//    /* If all available buffers have been consumed, stop playing */
//    if (IsocOutRdPtr == IsocOutWrPtr)
//    {    
//      /* Pause the audio stream */
//      AUDIO_OUT_fops.AudioCmd((uint8_t*)(IsocOutBuff),   /* Samples buffer pointer */
//                              AUDIO_OUT_PACKET,          /* Number of samples in Bytes */
//                              AUDIO_CMD_PAUSE);          /* Command to be processed */
//      
//      /* Stop entering play loop */
//      PlayFlag = 0;
//      
//      /* Reset buffer pointers */
//      IsocOutRdPtr = IsocOutBuff;
//      IsocOutWrPtr = IsocOutBuff;
//    }
//  }
  
  return USB_DEVICE_OK;
}

/**
  * @brief  usbd_audio_IN_Incplt
  *         Handles the iso IN incomplete event.
  * @param  pusbdev: instance
  * @retval status
  */
static uint8_t  usbd_audio_IN_Incplt (void  *pusbdev)
{
  return USB_DEVICE_OK;
}

/**
  * @brief  usbd_audio_OUT_Incplt
  *         Handles the iso out incomplete event.
  * @param  pusbdev: instance
  * @retval status
  */
static uint8_t  usbd_audio_OUT_Incplt (void  *pusbdev)
{
  return USB_DEVICE_OK;
}

/******************************************************************************
     AUDIO Class requests management
******************************************************************************/
/**
  * @brief  AUDIO_Req_GetMin
  *         Handles the GET_Min Audio control request.
  * @param  pusbdev: instance
  * @param  req: setup class request
  * @retval status
  */
static void AUDIO_Req_GetMin(void *pusbdev, USB_SETUP_REQEST *req)
{  
  /* Send the current mute state */
  uint8_t *buf;
  if (HIBYTE(req->wIndex) == AUDIO_SPK_FEATURE_UNIT_ID)
    buf = (uint8_t *)&usb_audio.u16SpkVolumeLimits[0];
  else
    buf = (uint8_t *)&usb_audio.u16MicVolumeLimits[0];
  USB_Device_CtrlSendData (pusbdev, 
                    buf,
                    req->wLength);
}

/**
  * @brief  AUDIO_Req_GetMax
  *         Handles the GET_Max Audio control request.
  * @param  pusbdev: instance
  * @param  req: setup class request
  * @retval status
  */
static void AUDIO_Req_GetMax(void *pusbdev, USB_SETUP_REQEST *req)
{  
    uint8_t *buf;
  if (HIBYTE(req->wIndex) == AUDIO_SPK_FEATURE_UNIT_ID)
    buf = (uint8_t *)&usb_audio.u16SpkVolumeLimits[1];
  else
    buf = (uint8_t *)&usb_audio.u16MicVolumeLimits[1];
  /* Send the current mute state */
  USB_Device_CtrlSendData (pusbdev, 
                   buf,
                    req->wLength);
}


/**
  * @brief  AUDIO_Req_GetRes
  *         Handles the GET_Res Audio control request.
  * @param  pusbdev: instance
  * @param  req: setup class request
  * @retval status
  */
static void AUDIO_Req_GetRes(void *pusbdev, USB_SETUP_REQEST *req)
{ 
  uint8_t *buf;
  if (HIBYTE(req->wIndex) == AUDIO_SPK_FEATURE_UNIT_ID)
    buf = (uint8_t *)&usb_audio.u16SpkVolumeLimits[2];
  else
    buf = (uint8_t *)&usb_audio.u16MicVolumeLimits[2];  
  /* Send the current mute state */
  USB_Device_CtrlSendData (pusbdev, 
                    buf,
                    req->wLength);
}
/**
  * @brief  AUDIO_Req_GetCurrent
  *         Handles the GET_CUR Audio control request.
  * @param  pusbdev: instance
  * @param  req: setup class request
  * @retval status
  */
static void AUDIO_Req_GetCurrent(void *pusbdev, USB_SETUP_REQEST *req)
{  
  /* Send the current mute state */
  USB_Device_CtrlSendData (pusbdev, 
                    AudioCtl,
                    req->wLength);
}

/**
  * @brief  AUDIO_Req_SetCurrent
  *         Handles the SET_CUR Audio control request.
  * @param  pusbdev: instance
  * @param  req: setup class request
  * @retval status
  */
static void AUDIO_Req_SetCurrent(void *pusbdev, USB_SETUP_REQEST *req)
{ 
  if (req->wLength)
  {
    /* Prepare the reception of the buffer over EP0 */
    USB_Device_CtrlPrepareRx (pusbdev, 
                       AudioCtl,
                       req->wLength);
    
    /* Set the global variables indicating current request and its length 
    to the function usbd_audio_EP0_RxReady() which will process the request */
    AudioCtlCmd = AUDIO_REQ_SET_CUR;     /* Set the request value */
    AudioCtlLen = req->wLength;          /* Set the request data length */
    AudioCtlUnit = HIBYTE(req->wIndex);  /* Set the request target unit */
    usb_audio.u8Interface = HIBYTE(req->wIndex);
    switch ( req->bmRequest & 0x3)
    {
      case AUDIO_CONTROL_FUNCTION:
        if (  HIBYTE(req->wValue) == AUDIO_MUTE_CONTROL)
        {
          usb_audio.u8RquestNo = USB_AUDIO_SET_MUTE;
        }
        if (  HIBYTE(req->wValue) == AUDIO_VOLUME_CONTROL)
        {
          usb_audio.u8RquestNo = USB_AUDIO_SET_VOLUME;
        }
        break;
      case AUDIO_ENDPOINT_FUNCTION:
        usb_audio.u8RquestNo = USB_AUDIO_SET_FREQ;
        break;
    }
    
//    usb_audio.u8RquestNo = 
  }
}

/**
  * @brief  USBD_audio_GetCfgDesc 
  *         Returns configuration descriptor.
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_audio_GetCfgDesc (uint8_t speed, uint16_t *length)
{
  *length = sizeof (usbd_audio_CfgDesc);
  return usbd_audio_CfgDesc;
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

