/**
  ******************************************************************************
  * File   : usbd_hid_core.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file provides the HID core functions.
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include "usbd_hid_core.h"
#include "usbd_desc.h"
#include "usbd_req.h"


/** @addtogroup AT32_USB_OTG_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_HID 
  * @brief usbd core module
  * @{
  */ 

/** @defgroup USBD_HID_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_HID_Private_Defines
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup USBD_HID_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 




/** @defgroup USBD_HID_Private_FunctionPrototypes
  * @{
  */


uint8_t  usb_device_hid_init (void  *pusbdev, 
                               uint8_t cfgidx);

uint8_t  usb_device_hid_reset (void  *pusbdev, 
                                 uint8_t cfgidx);

uint8_t  usb_device_hid_setup (void  *pusbdev, 
                                USB_SETUP_REQEST *req);

static uint8_t  *usb_device_hid_getcfgdesc (uint8_t speed, uint16_t *length);

uint8_t  usb_device_hid_datain (void  *pusbdev, uint8_t bepnum);
/**
  * @}
  */ 

/** @defgroup USBD_HID_Private_Variables
  * @{
  */ 

USBD_Class_cb_Type  USBD_HID_cb = 
{
  usb_device_hid_init,
  usb_device_hid_reset,
  usb_device_hid_setup,
  NULL, /*EPT0_TxSent*/  
  NULL, /*EPT0_RxReady*/
  usb_device_hid_datain, /*Data_In*/
  NULL, /*Data_Out*/
  NULL, /*SOF */
  NULL,
  NULL,      
  usb_device_hid_getcfgdesc,
#ifdef USB_OTG_HS_CORE  
  usb_device_hid_getcfgdesc, /* use same config as per FS */
#endif  
};

static uint32_t  USBD_HID_AltSet = 0;

static uint32_t  USBD_HID_Protocol = 0;

static uint32_t  USBD_HID_IdleState = 0;

static uint8_t usb_device_hid_cfgdesc[USB_HID_CONFIG_DESC_SIZ] =
{
  0x09, /* bLength: Configuration Descriptor size */
  USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration */
  USB_HID_CONFIG_DESC_SIZ,
  /* wTotalLength: Bytes returned */
  0x00,
  0x01,         /*bNumInterfaces: 1 interface*/
  0x01,         /*bConfigurationValue: Configuration value*/
  0x00,         /*iConfiguration: Index of string descriptor describing
  the configuration*/
  0xE0,         /*bmAttributes: bus powered and Support Remote Wake-up */
  0x32,         /*MaxPower 100 mA: this current is used for detecting Vbus*/
  
  /************** Descriptor of Joystick Mouse interface ****************/
  /* 09 */
  0x09,         /*bLength: Interface Descriptor size*/
  USB_INTERFACE_DESCRIPTOR_TYPE,/*bDescriptorType: Interface descriptor ept_type*/
  0x00,         /*bInterfaceNumber: Number of Interface*/
  0x00,         /*bAlternateSetting: Alternate setting*/
  0x01,         /*bNumEndpoints*/
  0x03,         /*bInterfaceClass: HID*/
  0x01,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
  0x02,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
  0,            /*iInterface: Index of string descriptor*/
  /******************** Descriptor of Joystick Mouse HID ********************/
  /* 18 */
  0x09,         /*bLength: HID Descriptor size*/
  HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
  0x11,         /*bcdHID: HID Class Spec release number*/
  0x01,
  0x00,         /*bCountryCode: Hardware target country*/
  0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
  0x22,         /*bDescriptorType*/
  HID_MOUSE_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
  0x00,
  /******************** Descriptor of Mouse endpoint ********************/
  /* 27 */
  0x07,          /*bLength: Endpoint Descriptor size*/
  USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/
  
  HID_IN_EP,     /*bEndpointAddress: Endpoint Address (IN)*/
  0x03,          /*bmAttributes: Interrupt endpoint*/
  HID_IN_PACKET, /*wMaxPacketSize: 4 Byte max */
  0x00,
  HID_FS_BINTERVAL,          /*bInterval: Polling Interval (10 ms)*/
  /* 34 */
} ;

static uint8_t USBD_HID_Desc[USB_HID_DESC_SIZ] =
{
  /* 18 */
  0x09,         /*bLength: HID Descriptor size*/
  HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
  0x11,         /*bcdHID: HID Class Spec release number*/
  0x01,
  0x00,         /*bCountryCode: Hardware target country*/
  0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
  0x22,         /*bDescriptorType*/
  HID_MOUSE_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
  0x00,
};



static uint8_t HID_MOUSE_ReportDesc[HID_MOUSE_REPORT_DESC_SIZE] =
{
  0x05,   0x01,
  0x09,   0x02,
  0xA1,   0x01,
  0x09,   0x01,
  
  0xA1,   0x00,
  0x05,   0x09,
  0x19,   0x01,
  0x29,   0x03,
  
  0x15,   0x00,
  0x25,   0x01,
  0x95,   0x03,
  0x75,   0x01,
  
  0x81,   0x02,
  0x95,   0x01,
  0x75,   0x05,
  0x81,   0x01,
  
  0x05,   0x01,
  0x09,   0x30,
  0x09,   0x31,
  0x09,   0x38,
  
  0x15,   0x81,
  0x25,   0x7F,
  0x75,   0x08,
  0x95,   0x03,
  
  0x81,   0x06,
  0xC0,   0x09,
  0x3c,   0x05,
  0xff,   0x09,
  
  0x01,   0x15,
  0x00,   0x25,
  0x01,   0x75,
  0x01,   0x95,
  
  0x02,   0xb1,
  0x22,   0x75,
  0x06,   0x95,
  0x01,   0xb1,
  
  0x01,   0xc0
}; 

/**
  * @}
  */ 

/** @defgroup USBD_HID_Private_Functions
  * @{
  */ 

/**
  * @brief  usb_device_hid_init
  *         Initialize the HID interface
  * @param  pusbdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
uint8_t  usb_device_hid_init (void  *pusbdev, 
                               uint8_t cfgidx)
{
  
  /* Open EP IN */
  USB_DCD_EPT_Open(pusbdev,
              HID_IN_EP,
              HID_IN_PACKET,
              USB_OTG_EPT_INT);
  
  return USB_DEVICE_OK;
}

/**
  * @brief  usb_device_hid_init
  *         DeInitialize the HID layer
  * @param  pusbdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
uint8_t  usb_device_hid_reset (void  *pusbdev, 
                                 uint8_t cfgidx)
{
  /* Close HID EPs */
  USB_DCD_EPT_Close (pusbdev , HID_IN_EP);  
  return USB_DEVICE_OK;
}

/**
  * @brief  usb_device_hid_setup
  *         Handle the HID specific requests
  * @param  pusbdev: instance
  * @param  req: usb requests
  * @retval status
  */
uint8_t  usb_device_hid_setup (void  *pusbdev, 
                         USB_SETUP_REQEST *req)
{
  uint16_t len = 0;
  uint8_t  *pbuf = NULL;
  
  switch (req->bmRequest & USB_REQUEST_TYPE_MASK)
  {
  case USB_REQUEST_TYPE_CLASS :  
    switch (req->bRequest)
    {
    case HID_REQ_SET_PROTOCOL:
      USBD_HID_Protocol = (uint8_t)(req->wValue);
      break;
      
    case HID_REQ_GET_PROTOCOL:
      USB_Device_CtrlSendData (pusbdev, 
                        (uint8_t *)&USBD_HID_Protocol,
                        1);    
      break;
      
    case HID_REQ_SET_IDLE:
      USBD_HID_IdleState = (uint8_t)(req->wValue >> 8);
      break;
      
    case HID_REQ_GET_IDLE:
      USB_Device_CtrlSendData (pusbdev, 
                        (uint8_t *)&USBD_HID_IdleState,
                        1);        
      break;      
      
    default:
      USB_Device_CtlError (pusbdev, req);
      return USB_DEVICE_FAIL; 
    }
    break;
    
  case USB_REQUEST_TYPE_STANDARD:
    switch (req->bRequest)
    {
    case USB_REQUEST_GET_DESCRIPTOR: 
      if( req->wValue >> 8 == HID_REPORT_DESC)
      {
        len = MIN(HID_MOUSE_REPORT_DESC_SIZE , req->wLength);
        pbuf = HID_MOUSE_ReportDesc;
      }
      else if( req->wValue >> 8 == HID_DESCRIPTOR_TYPE)
      {
        
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
        pbuf = USBD_HID_Desc;   
#else
        pbuf = usb_device_hid_cfgdesc + 0x12;
#endif 
        len = MIN(USB_HID_DESC_SIZ , req->wLength);
      }
      else
      {
        /* Do Nothing */
      }
      
      USB_Device_CtrlSendData (pusbdev, 
                        pbuf,
                        len);
      
      break;
      
    case USB_REQUEST_GET_INTERFACE :
      USB_Device_CtrlSendData (pusbdev,
                        (uint8_t *)&USBD_HID_AltSet,
                        1);
      break;
      
    case USB_REQUEST_SET_INTERFACE :
      USBD_HID_AltSet = (uint8_t)(req->wValue);
      break;
      
    default:
      USBD_HID_AltSet = (uint8_t)(req->wValue);
      break; 
    }
    break;
    
  default:
    USB_Device_CtrlSendData (pusbdev,
                      (uint8_t *)&USBD_HID_AltSet,
                      1);
    break; 
  }
  return USB_DEVICE_OK;
}

/**
  * @brief  usb_device_hid_sendreport 
  *         Send HID Report
  * @param  pusbdev: device instance
  * @param  buff: pointer to report
  * @retval status
  */
uint8_t usb_device_hid_sendreport     (USB_OTG_CORE_HANDLE  *pusbdev, 
                                 uint8_t *report,
                                 uint16_t len)
{
  if (pusbdev->dev.device_status == USB_OTG_CONFIGURED )
  {
    USB_DCD_EPT_Tx (pusbdev, HID_IN_EP, report, len);
  }
  return USB_DEVICE_OK;
}

/**
  * @brief  usb_device_hid_getcfgdesc 
  *         return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *usb_device_hid_getcfgdesc (uint8_t speed, uint16_t *length)
{
  *length = sizeof (usb_device_hid_cfgdesc);
  return usb_device_hid_cfgdesc;
}

/**
  * @brief  usb_device_hid_datain
  *         handle data IN Stage
  * @param  pusbdev: device instance
  * @param  bepnum: endpoint index
  * @retval status
  */
uint8_t  usb_device_hid_datain (void  *pusbdev, 
                              uint8_t bepnum)
{
  
  /* Ensure that the FIFO is empty before a new transfer, this condition could 
  be caused by  a new transfer before the end of the previous transfer */
  USB_DCD_EPT_Flush(pusbdev, HID_IN_EP);
  return USB_DEVICE_OK;
}

/**
  * @brief  usb_device_hid_getpollinginterval 
  *         return polling interval from endpoint descriptor
  * @param  pusbdev: device instance
  * @retval polling interval
  */
uint32_t usb_device_hid_getpollinginterval (USB_OTG_CORE_HANDLE *pusbdev)
{
  uint32_t polling_interval = 0;

  /* HIGH-speed endpoints */
  if(pusbdev->cfg.speed == USB_OTG_SPEED_HIGH)
  {
   /* Sets the data transfer polling interval for high speed transfers. 
    Values between 1..16 are allowed. Values correspond to interval 
    of 2 ^ (bInterval-1). This option (8 ms, corresponds to HID_HS_BINTERVAL */
    polling_interval = (((1 <<(HID_HS_BINTERVAL - 1)))/8);
  }
  else   /* LOW and FULL-speed endpoints */
  {
    /* Sets the data transfer polling interval for low and full 
    speed transfers */
    polling_interval =  HID_FS_BINTERVAL;
  }
  
  return ((uint32_t)(polling_interval));
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

