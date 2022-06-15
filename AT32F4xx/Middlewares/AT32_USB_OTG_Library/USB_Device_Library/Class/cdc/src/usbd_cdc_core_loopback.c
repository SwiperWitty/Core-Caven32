/**
  ******************************************************************************
  * File   : usbd_cdc_core_loopback.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file provides the high layer firmware functions to manage the 
  *          following functionalities of the USB CDC Class:
  *           - Initialization and Configuration of high and low layer
  *           - Enumeration as CDC Device (and enumeration for each implemented memory interface)
  *           - OUT/IN data transfer
  *           - Command IN transfer (class requests management)
  *           - Error management
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_core_loopback.h"
#include "usbd_desc.h"
#include "usbd_req.h"



/** @addtogroup AT32_USB_OTG_DEVICE_LIBRARY
  * @{
  */


/** @defgroup usbd_cdc 
  * @brief usbd core module
  * @{
  */ 

/** @defgroup usbd_cdc_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup usbd_cdc_Private_Defines
  * @{
  */ 

#define USB_CDC_IDLE         0
#define USB_CDC_BUSY         1
#define USB_CDC_ZLP          2

/**
  * @}
  */ 


/** @defgroup usbd_cdc_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup usbd_cdc_Private_FunctionPrototypes
  * @{
  */

/*********************************************
   CDC Device library callbacks
 *********************************************/
uint8_t  usb_device_cdc_init        (void  *pusbdev, uint8_t cfgidx);
uint8_t  usb_device_cdc_reset      (void  *pusbdev, uint8_t cfgidx);
uint8_t  usb_device_cdc_Setup       (void  *pusbdev, USB_SETUP_REQEST *req);
uint8_t  usb_device_cdc_ept0_rxready  (void *pusbdev);
uint8_t  usb_device_cdc_dataIn      (void *pusbdev, uint8_t bepnum);
uint8_t  usb_device_cdc_dataOut     (void *pusbdev, uint8_t bepnum);
uint8_t  usb_device_cdc_sof        (void *pusbdev);

/*********************************************
   CDC specific management functions
 *********************************************/
static uint8_t  *usb_device_cdc_getcfgdesc (uint8_t speed, uint16_t *length);
#ifdef USE_USB_OTG_HS  
static uint8_t  *USBD_cdc_GetOtherCfgDesc (uint8_t speed, uint16_t *length);
#endif
/**
  * @}
  */ 

/** @defgroup usbd_cdc_Private_Variables
  * @{
  */ 
extern CDC_IF_Prop_Type  APP_FOPS;
extern uint8_t USB_Device_Desc   [USB_SIZ_DEVICE_DESC];
uint8_t usb_device_cdc_cfgdesc  [USB_CDC_CONFIG_DESC_SIZ];
uint8_t usb_device_cdc_othercfgdesc  [USB_CDC_CONFIG_DESC_SIZ];
static __IO uint32_t  usbd_cdc_AltSet = 0;
uint8_t CmdBuff[CDC_CMD_PACKET_SZE];
uint8_t cdc_rx_buffer[64];
uint8_t cdc_status = 2;

static uint32_t cdcCmd = 0xFF;
static uint32_t cdcLen = 0;

/* CDC interface class callbacks structure */
USBD_Class_cb_Type  USB_DEVICE_CDC_cb = 
{
  usb_device_cdc_init,
  usb_device_cdc_reset,
  usb_device_cdc_Setup,
  NULL,                 /* EPT0_TxSent, */
  usb_device_cdc_ept0_rxready,
  usb_device_cdc_dataIn,
  usb_device_cdc_dataOut,
  usb_device_cdc_sof,
  NULL,
  NULL,     
  usb_device_cdc_getcfgdesc,
#ifdef USE_USB_OTG_HS   
  USBD_cdc_GetOtherCfgDesc, /* use same cobfig as per FS */
#endif /* USE_USB_OTG_HS  */
};

/* USB CDC device Configuration Descriptor */
uint8_t usb_device_cdc_cfgdesc[USB_CDC_CONFIG_DESC_SIZ] =
{
  /*Configuration Descriptor*/
  0x09,   /* bLength: Configuration Descriptor size */
  USB_CONFIGURATION_DESCRIPTOR_TYPE,      /* bDescriptorType: Configuration */
  USB_CDC_CONFIG_DESC_SIZ,                /* wTotalLength:no of returned bytes */
  0x00,
  0x02,   /* bNumInterfaces: 2 interface */
  0x01,   /* bConfigurationValue: Configuration value */
  0x00,   /* iConfiguration: Index of string descriptor describing the configuration */
  0xC0,   /* bmAttributes: self powered */
  0x32,   /* MaxPower 0 mA */
  
  /*---------------------------------------------------------------------------*/
  
  /*Interface Descriptor */
  0x09,   /* bLength: Interface Descriptor size */
  USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: Interface */
  /* Interface descriptor ept_type */
  0x00,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x01,   /* bNumEndpoints: One endpoints used */
  0x02,   /* bInterfaceClass: Communication Interface Class */
  0x02,   /* bInterfaceSubClass: Abstract Control Model */
  0x01,   /* bInterfaceProtocol: Common AT commands */
  0x00,   /* iInterface: */
  
  /*Header Functional Descriptor*/
  0x05,   /* bLength: Endpoint Descriptor size */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x00,   /* bDescriptorSubtype: Header Func Desc */
  0x10,   /* bcdCDC: spec release number */
  0x01,
  
  /*Call Management Functional Descriptor*/
  0x05,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x01,   /* bDescriptorSubtype: Call Management Func Desc */
  0x00,   /* bmCapabilities: D0+D1 */
  0x01,   /* bDataInterface: 1 */
  
  /*ACM Functional Descriptor*/
  0x04,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x02,   /* bDescriptorSubtype: Abstract Control Management desc */
  0x02,   /* bmCapabilities */
  
  /*Union Functional Descriptor*/
  0x05,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x06,   /* bDescriptorSubtype: Union func desc */
  0x00,   /* bMasterInterface: Communication class interface */
  0x01,   /* bSlaveInterface0: Data Class Interface */
  
  /*Endpoint 2 Descriptor*/
  0x07,                           /* bLength: Endpoint Descriptor size */
  USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
  CDC_CMD_EP,                     /* bEndpointAddress */
  0x03,                           /* bmAttributes: Interrupt */
  LOBYTE(CDC_CMD_PACKET_SZE),     /* wMaxPacketSize: */
  HIBYTE(CDC_CMD_PACKET_SZE),
#ifdef USE_USB_OTG_HS
  0x10,                           /* bInterval: */
#else
  0xFF,                           /* bInterval: */
#endif /* USE_USB_OTG_HS */
  
  /*---------------------------------------------------------------------------*/
  
  /*Data class interface descriptor*/
  0x09,   /* bLength: Endpoint Descriptor size */
  USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: */
  0x01,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x02,   /* bNumEndpoints: Two endpoints used */
  0x0A,   /* bInterfaceClass: CDC */
  0x00,   /* bInterfaceSubClass: */
  0x00,   /* bInterfaceProtocol: */
  0x00,   /* iInterface: */
  
  /*Endpoint OUT Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType: Endpoint */
  CDC_OUT_EP,                        /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LOBYTE(CDC_DATA_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
  HIBYTE(CDC_DATA_MAX_PACKET_SIZE),
  0x00,                              /* bInterval: ignore for Bulk transfer */
  
  /*Endpoint IN Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType: Endpoint */
  CDC_IN_EP,                         /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LOBYTE(CDC_DATA_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
  HIBYTE(CDC_DATA_MAX_PACKET_SIZE),
  0x00                               /* bInterval: ignore for Bulk transfer */
} ;

uint8_t usb_device_cdc_othercfgdesc[USB_CDC_CONFIG_DESC_SIZ] =
{ 
  0x09,   /* bLength: Configuration Descriptor size */
  USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION,   
  USB_CDC_CONFIG_DESC_SIZ,
  0x00,
  0x02,   /* bNumInterfaces: 2 interfaces */
  0x01,   /* bConfigurationValue: */
  0x04,   /* iConfiguration: */
  0xC0,   /* bmAttributes: */
  0x32,   /* MaxPower 100 mA */  
  
  /*Interface Descriptor */
  0x09,   /* bLength: Interface Descriptor size */
  USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: Interface */
  /* Interface descriptor ept_type */
  0x00,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x01,   /* bNumEndpoints: One endpoints used */
  0x02,   /* bInterfaceClass: Communication Interface Class */
  0x02,   /* bInterfaceSubClass: Abstract Control Model */
  0x01,   /* bInterfaceProtocol: Common AT commands */
  0x00,   /* iInterface: */
  
  /*Header Functional Descriptor*/
  0x05,   /* bLength: Endpoint Descriptor size */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x00,   /* bDescriptorSubtype: Header Func Desc */
  0x10,   /* bcdCDC: spec release number */
  0x01,
  
  /*Call Management Functional Descriptor*/
  0x05,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x01,   /* bDescriptorSubtype: Call Management Func Desc */
  0x00,   /* bmCapabilities: D0+D1 */
  0x01,   /* bDataInterface: 1 */
  
  /*ACM Functional Descriptor*/
  0x04,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x02,   /* bDescriptorSubtype: Abstract Control Management desc */
  0x02,   /* bmCapabilities */
  
  /*Union Functional Descriptor*/
  0x05,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x06,   /* bDescriptorSubtype: Union func desc */
  0x00,   /* bMasterInterface: Communication class interface */
  0x01,   /* bSlaveInterface0: Data Class Interface */
  
  /*Endpoint 2 Descriptor*/
  0x07,                           /* bLength: Endpoint Descriptor size */
  USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
  CDC_CMD_EP,                     /* bEndpointAddress */
  0x03,                           /* bmAttributes: Interrupt */
  LOBYTE(CDC_CMD_PACKET_SZE),     /* wMaxPacketSize: */
  HIBYTE(CDC_CMD_PACKET_SZE),
  0xFF,                           /* bInterval: */
  
  /*---------------------------------------------------------------------------*/
  
  /*Data class interface descriptor*/
  0x09,   /* bLength: Endpoint Descriptor size */
  USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: */
  0x01,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x02,   /* bNumEndpoints: Two endpoints used */
  0x0A,   /* bInterfaceClass: CDC */
  0x00,   /* bInterfaceSubClass: */
  0x00,   /* bInterfaceProtocol: */
  0x00,   /* iInterface: */
  
  /*Endpoint OUT Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType: Endpoint */
  CDC_OUT_EP,                        /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  0x40,                              /* wMaxPacketSize: */
  0x00,
  0x00,                              /* bInterval: ignore for Bulk transfer */
  
  /*Endpoint IN Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_ENDPOINT_DESCRIPTOR_TYPE,     /* bDescriptorType: Endpoint */
  CDC_IN_EP,                        /* bEndpointAddress */
  0x02,                             /* bmAttributes: Bulk */
  0x40,                             /* wMaxPacketSize: */
  0x00,
  0x00                              /* bInterval */
};

/**
  * @}
  */ 

/** @defgroup usbd_cdc_Private_Functions
  * @{
  */ 

/**
  * @brief  usb_device_cdc_init
  *         Initialize the CDC interface
  * @param  pusbdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
uint8_t  usb_device_cdc_init (void  *pusbdev, 
                               uint8_t cfgidx)
{
  uint8_t *pbuf;

  /* Open EP IN */
  USB_DCD_EPT_Open(pusbdev,
              CDC_IN_EP,
              CDC_DATA_IN_PACKET_SIZE,
              USB_OTG_EPT_BULK);
  
  /* Open EP OUT */
  USB_DCD_EPT_Open(pusbdev,
              CDC_OUT_EP,
              CDC_DATA_OUT_PACKET_SIZE,
              USB_OTG_EPT_BULK);
  
  /* Open Command IN EP */
  USB_DCD_EPT_Open(pusbdev,
              CDC_CMD_EP,
              CDC_CMD_PACKET_SZE,
              USB_OTG_EPT_INT);
  
  pbuf = (uint8_t *)USB_Device_Desc;
//  pbuf[4] = DEVICE_CLASS_CDC;
//  pbuf[5] = DEVICE_SUBCLASS_CDC;
  USB_DCD_EPT_PrepareRx(pusbdev, CDC_OUT_EP, cdc_rx_buffer, 64);
  return USB_DEVICE_OK;
}

/**
  * @brief  usb_device_cdc_init
  *         DeInitialize the CDC layer
  * @param  pusbdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
uint8_t  usb_device_cdc_reset (void  *pusbdev, 
                                 uint8_t cfgidx)
{
  /* Open EP IN */
  USB_DCD_EPT_Close(pusbdev,
              CDC_IN_EP);
  
  /* Open EP OUT */
  USB_DCD_EPT_Close(pusbdev,
              CDC_OUT_EP);
  
  /* Open Command IN EP */
  USB_DCD_EPT_Close(pusbdev,
              CDC_CMD_EP);
  
  return USB_DEVICE_OK;
}

/**
  * @brief  usb_device_cdc_Setup
  *         Handle the CDC specific requests
  * @param  pusbdev: instance
  * @param  req: usb requests
  * @retval status
  */
uint8_t  usb_device_cdc_Setup (void  *pusbdev, 
                                USB_SETUP_REQEST *req)
{
  switch (req->bmRequest & USB_REQUEST_TYPE_MASK)
  {
    /* CDC Class Requests -------------------------------*/
  case USB_REQUEST_TYPE_CLASS :
      /* Check if the request is a data setup packet */
      if (req->wLength)
      {
        /* Check if the request is Device-to-Host */
        if (req->bmRequest & 0x80)
        {
          /* STALL request */  
          APP_FOPS.pcdc_Ctrl(req->bRequest, CmdBuff, req->wLength); 
          USB_Device_CtrlSendData(pusbdev, CmdBuff, req->wLength);          
//          USB_Device_CtlError (pusbdev, req);         
        }
        else /* Host-to-Device request */
        {
          /* Set the value of the current command to be processed */
          cdcCmd = req->bRequest;
          cdcLen = req->wLength;
          /* Prepare the reception of the buffer over EP0
          Next step: the received data will be managed in usbd_cdc_EP0_TxSent() 
          function. */
          USB_Device_CtrlPrepareRx (pusbdev,
                             CmdBuff,
                             req->wLength);           
        }
      }
      else /* No Data request */
      {
        /* do nothing */
      }
      
      return USB_DEVICE_OK;
      
    default:
      USB_Device_CtlError (pusbdev, req);
      return USB_DEVICE_FAIL;
    
    /* Standard Requests -------------------------------*/
  case USB_REQUEST_TYPE_STANDARD:
    switch (req->bRequest)
    {
    case USB_REQUEST_GET_DESCRIPTOR: 
      USB_Device_CtlError (pusbdev, req);
      return USB_DEVICE_FAIL;
      
    case USB_REQUEST_GET_INTERFACE :
      USB_Device_CtrlSendData (pusbdev,
                        (uint8_t *)&usbd_cdc_AltSet,
                        1);
      break;
      
    case USB_REQUEST_SET_INTERFACE :
      if ((uint8_t)(req->wValue) < USBD_ITF_MAX_NUM)
      {
        usbd_cdc_AltSet = (uint8_t)(req->wValue);
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
  * @brief  usb_device_cdc_ept0_rxready
  *         Data received on control endpoint
  * @param  pusbdev: device instance
  * @retval status
  */
uint8_t  usb_device_cdc_ept0_rxready (void  *pusbdev)
{ 
    if (cdcCmd != NO_CMD)
    {
        /* Process the data */
        APP_FOPS.pcdc_Ctrl(cdcCmd, CmdBuff, cdcLen);
    
        /* Reset the command variable to default value */
        cdcCmd = NO_CMD;
    }
  cdc_status = 2;
  USB_DCD_EPT_PrepareRx(pusbdev, CDC_OUT_EP, cdc_rx_buffer, 64);
  return USB_DEVICE_OK;
}


/**
  * @brief  usbd_audio_DataIn
  *         Data sent on non-control IN endpoint
  * @param  pusbdev: device instance
  * @param  bepnum: endpoint number
  * @retval status
  */
uint8_t  usb_device_cdc_dataIn (void *pusbdev, uint8_t bepnum)
{
  /* inform application layer that data was sent */ 
  cdc_status = APP_FOPS.pcdc_DataTx();  
  return USB_DEVICE_OK;
}

/**
  * @brief  usb_device_cdc_dataOut
  *         Data received on non-control Out endpoint
  * @param  pusbdev: device instance
  * @param  bepnum: endpoint number
  * @retval status
  */
uint8_t  usb_device_cdc_dataOut (void *pusbdev, uint8_t bepnum)
{      
  uint16_t USB_Rx_Cnt;
  
  /* Get the received data buffer and update the counter */
  USB_Rx_Cnt = ((USB_OTG_CORE_HANDLE*)pusbdev)->dev.out_ept[bepnum].xfer_count;
  
  /* pass received data count to application layer */
  APP_FOPS.pcdc_DataRx(cdc_rx_buffer, USB_Rx_Cnt);
  
  return USB_DEVICE_OK;
}

/**
  * @brief  usb_device_cdc_sof
  *         SOF function
  * @param  pusbdev: device instance
  * @retval status
  */
uint8_t usb_device_cdc_sof (void *pusbdev)
{
  USB_OTG_CORE_HANDLE *dev = (USB_OTG_CORE_HANDLE *)pusbdev;
  if( dev->dev.device_status == USB_OTG_CONFIGURED && (cdc_status == 2))
      APP_FOPS.pcdc_DataTx();  
  return 0;
}
/**
  * @brief  usb_device_cdc_getcfgdesc 
  *         Return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *usb_device_cdc_getcfgdesc (uint8_t speed, uint16_t *length)
{
  *length = sizeof (usb_device_cdc_cfgdesc);
  return usb_device_cdc_cfgdesc;
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

