/**
  ******************************************************************************
  * File   : usbd_cdc_hid.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file includes cdc and HID application
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_hid.h"
#include "usbd_cdc_core_loopback.h"
#include "usbd_hid_keyboard.h"
#include "usbd_desc.h"
#include "usbd_req.h"
#include "usbd_def.h"


/* CDC and hid interface class callbacks structure */
uint8_t  usb_device_cdc_hid_init        (void  *pusbdev, uint8_t cfgidx);
uint8_t  usb_device_cdc_hid_reset      (void  *pusbdev, uint8_t cfgidx);
uint8_t  usb_device_cdc_hid_Setup       (void  *pusbdev, USB_SETUP_REQEST *req);
uint8_t  usb_device_cdc_hid_ept0_rxready  (void *pusbdev);
uint8_t  usb_device_cdc_hid_dataIn      (void *pusbdev, uint8_t bepnum);
uint8_t  usb_device_cdc_hid_dataOut     (void *pusbdev, uint8_t bepnum);
uint8_t  usb_device_cdc_hid_sof        (void *pusbdev);
static uint8_t  *usb_device_cdc_hid_getcfgdesc (uint8_t speed, uint16_t *length);
USBD_Class_cb_Type  USB_DEVICE_CDC_HID_cb = 
{
  usb_device_cdc_hid_init,
  usb_device_cdc_hid_reset,
  usb_device_cdc_hid_Setup,
  NULL,                 /* EPT0_TxSent, */
  usb_device_cdc_hid_ept0_rxready,
  usb_device_cdc_hid_dataIn,
  usb_device_cdc_hid_dataOut,
  usb_device_cdc_hid_sof,
  NULL,
  NULL,     
  usb_device_cdc_hid_getcfgdesc,
};

#define USB_HID_CDC_CONFIG_DESC_SIZ  (USB_HID_CONFIG_DESC_SIZ -9 + USB_CDC_CONFIG_DESC_SIZ  + 8) + 8

#define HID_INTERFACE 0x2
#define CDC_COM_INTERFACE 0x0

extern USBD_Class_cb_Type  USB_DEVICE_CDC_cb;
extern USBD_Class_cb_Type  USBD_HID_cb;

static uint8_t USBD_HID_CDC_CfgDesc[USB_HID_CDC_CONFIG_DESC_SIZ] =
{  
  0x09, /* bLength: Configuration Descriptor size */
  USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration */
  USB_HID_CDC_CONFIG_DESC_SIZ,
  /* wTotalLength: Bytes returned */
  0x00,
  0x03,         /*bNumInterfaces: 3 interfaces (2 for CDC, 1 for MSC)*/
  0x01,         /*bConfigurationValue: Configuration value*/
  0x00,         /*iConfiguration: Index of string descriptor describing
  the configuration*/
  0xE0,         /*bmAttributes: bus powered and Support Remote Wake-up */
  0x32,         /*MaxPower 100 mA: this current is used for detecting Vbus*/
  
     /******** /IAD should be positioned just before the CDC interfaces ******/
   
  
  0x08, /* bLength */
  0x0B, /* bDescriptorType */
  0x00, /* bFirstInterface */
  0x02, /* bInterfaceCount */
  0x02, /* bFunctionClass */
  0x02, /* bFunctionSubClass */
  0x01, /* bFunctionProtocol */
  0x00, /* iFunction (Index of string descriptor describing this function) */
  
   /*Interface Descriptor */
  0x09,   /* bLength: Interface Descriptor size */
  USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: Interface */
  /* Interface descriptor type */
  CDC_COM_INTERFACE,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x01,   /* bNumEndpoints: One endpoints used */
  0x02,   /* bInterfaceClass: Communication Interface Class */
  0x02,   /* bInterfaceSubClass: Abstract Control Model */
  0x01,   /* bInterfaceProtocol: Common AT commands */
  0x01,   /* iInterface: */
  
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
  0x02,   /* bDataInterface: 2 */
  
  /*ACM Functional Descriptor*/
  0x04,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x02,   /* bDescriptorSubtype: Abstract Control Management desc */
  0x02,   /* bmCapabilities */
  
  /*Union Functional Descriptor*/
  0x05,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x06,   /* bDescriptorSubtype: Union func desc */
  0x01,   /* bMasterInterface: Communication class interface */
  0x02,   /* bSlaveInterface0: Data Class Interface */
  
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
  LOBYTE(CDC_DATA_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
  HIBYTE(CDC_DATA_MAX_PACKET_SIZE),
  0x00,                              /* bInterval: ignore for Bulk transfer */
  
  /*Endpoint IN Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_ENDPOINT_DESCRIPTOR_TYPE,     /* bDescriptorType: Endpoint */
  CDC_IN_EP,                        /* bEndpointAddress */
  0x02,                             /* bmAttributes: Bulk */
  LOBYTE(CDC_DATA_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
  HIBYTE(CDC_DATA_MAX_PACKET_SIZE),
  0x00,                              /* bInterval */
  
  
   /*            IAD to associate the two CDC interfaces */
  
  0x08, /* bLength */
  0x0B, /* bDescriptorType */
  0x02, /* bFirstInterface */
  0x01, /* bInterfaceCount */
  0x03, /* bFunctionClass */
  0x00, /* bFunctionSubClass */
  0x01, /* bFunctionProtocol */
  0x00, /* iFunction (Index of string descriptor describing this function) */
  
  /************** Descriptor of Joystick Mouse interface ****************/
  /* 09 */
  0x09,         /*bLength: Interface Descriptor size*/
  USB_INTERFACE_DESCRIPTOR_TYPE,/*bDescriptorType: Interface descriptor type*/
  HID_INTERFACE,         /*bInterfaceNumber: Number of Interface*/
  0x00,         /*bAlternateSetting: Alternate setting*/
  0x01,         /*bNumEndpoints*/
  0x03,         /*bInterfaceClass: HID*/
  0x00,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
  0x01,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
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
  HID_KEYBOARD_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
  0x00,
  /******************** Descriptor of Mouse endpoint ********************/
  /* 27 */
  0x07,          /*bLength: Endpoint Descriptor size*/
  USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/
  
  HID_IN_EP,     /*bEndpointAddress: Endpoint Address (IN)*/
  0x03,          /*bmAttributes: Interrupt endpoint*/
  HID_IN_PACKET, /*wMaxPacketSize: 4 Byte max */
  0x00,
  0x0A,          /*bInterval: Polling Interval (10 ms)*/
  /* 34 */
} ;
/**
  * @brief  usb_device_cdc_hid_init
  *         Initialize the MSC & CDC interfaces
  * @param  pusbdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
uint8_t  usb_device_cdc_hid_init (void  *pusbdev, uint8_t cfgidx)
{
  /* CDC initialization */
  USB_DEVICE_CDC_cb.Init(pusbdev, cfgidx);
  
  /* HID initialization */
  USBD_HID_cb.Init(pusbdev, cfgidx);
  
  return USB_DEVICE_OK;
}


/**
  * @brief  usb_device_cdc_hid_reset
  *         DeInitialize the HID/CDC interfaces
  * @param  pusbdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
uint8_t  usb_device_cdc_hid_reset      (void  *pusbdev, uint8_t cfgidx)
{
  /* CDC De-initialization */
  USB_DEVICE_CDC_cb.Reset(pusbdev, cfgidx);
  
  /* HID De-initialization */
  USBD_HID_cb.Reset(pusbdev, cfgidx);
  return USB_DEVICE_OK;
}

/**
  * @brief  usb_device_cdc_hid_Setup
  *         Handle the HID/CDC specific requests
  * @param  pusbdev: instance
  * @param  req: usb requests
  * @retval status
  */
uint8_t  usb_device_cdc_hid_Setup       (void  *pusbdev, USB_SETUP_REQEST *req)
{
  switch (req->bmRequest & USB_REQUEST_RECIPIENT_MASK)
  {
  case USB_REQUEST_RECIPIENT_INTERFACE:
    if (req->wIndex == HID_INTERFACE)
    {
      return ( USBD_HID_cb.Setup(pusbdev, req));
    }
    else
    {
      return (USB_DEVICE_CDC_cb.Setup(pusbdev, req));
    }
    
  case USB_REQUEST_RECIPIENT_ENDPOINT:
    if (req->wIndex == HID_IN_EP)
    {
      return (USBD_HID_cb.Setup (pusbdev, req));   
    }
    else
    {
      return (USB_DEVICE_CDC_cb.Setup(pusbdev, req));
    }
  }   
  return USB_DEVICE_OK;
  
}


/**
  * @brief  usb_device_cdc_hid_getcfgdesc 
  *         return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *usb_device_cdc_hid_getcfgdesc (uint8_t speed, uint16_t *length)
{
  *length = sizeof(USBD_HID_CDC_CfgDesc);
  return USBD_HID_CDC_CfgDesc;
}

uint8_t  usb_device_cdc_hid_ept0_rxready  (void *pusbdev)
{
  return USB_DEVICE_CDC_cb.EPT0_RxReady(pusbdev);
}
uint8_t  usb_device_cdc_hid_dataIn (void *pusbdev, uint8_t bepnum)
{
  if ( bepnum == (HID_IN_EP & ~0x80) )
  {
    return USBD_HID_cb.Data_In(pusbdev, bepnum);
  }else
  {
    return USB_DEVICE_CDC_cb.Data_In(pusbdev, bepnum);
  }
}

uint8_t  usb_device_cdc_hid_dataOut     (void *pusbdev, uint8_t bepnum)
{
  return USB_DEVICE_CDC_cb.Data_Out(pusbdev, bepnum);
}

uint8_t  usb_device_cdc_hid_sof        (void *pusbdev)
{
  USBD_HID_cb.SOF(pusbdev);
  USB_DEVICE_CDC_cb.SOF(pusbdev);
  return USB_DEVICE_OK;
}

