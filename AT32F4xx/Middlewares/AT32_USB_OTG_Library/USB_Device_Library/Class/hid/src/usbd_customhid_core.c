/**
  ******************************************************************************
  * File   : usbd_customhid_core.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file provides the HID core functions.
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include "usbd_customhid_core.h"
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
uint8_t  usb_device_hid_dataout(void  *pusbdev,  uint8_t bepnum);

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
  usb_device_hid_dataout, /*Data_Out*/
  NULL, /*SOF */
  NULL,
  NULL,      
  usb_device_hid_getcfgdesc,
#ifdef USB_OTG_HS_CORE  
  usb_device_hid_getcfgdesc, /* use same config as per FS */
#endif  
};

uint8_t hid_rxBuf[HID_OUT_PACKET + 1];
uint8_t hid_txBuf[HID_IN_PACKET + 1];


static uint32_t  USBD_HID_AltSet = 0;

static uint32_t  USBD_HID_Protocol = 0;

static uint32_t  USBD_HID_IdleState = 0;

extern uint8_t Rxbuffer[];
extern __IO uint8_t receive_flag;
extern __IO uint8_t receive_len;


static uint8_t CustomHID_ConfigDescriptor[CUSTOMHID_SIZ_CONFIG_DESC] =
{
  0x09, /* bLength: Configuration Descriptor size */
  USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration */
  CUSTOMHID_SIZ_CONFIG_DESC,
  /* wTotalLength: Bytes returned */
  0x00,
  0x01,         /* bNumInterfaces: 2 interface */
  0x01,         /* bConfigurationValue: Configuration value */
  0x00,         /* iConfiguration: Index of string descriptor describing
                               the configuration*/
  0xC0,         /* bmAttributes: Self powered */
  0x32,         /* MaxPower 100 mA: this current is used for detecting Vbus */

  /************** Button + LED ****************/
  /************** Descriptor of Custom HID interface ****************/
  /* 09 */
  0x09,         /* bLength: Interface Descriptor size */
  USB_INTERFACE_DESCRIPTOR_TYPE,/* bDescriptorType: Interface descriptor type */
  0x00,         /* bInterfaceNumber: Number of Interface */
  0x00,         /* bAlternateSetting: Alternate setting */
  0x02,         /* bNumEndpoints */
  0x03,         /* bInterfaceClass: HID */
  0x00,         /* bInterfaceSubClass : 1=BOOT, 0=no boot */
  0x00,         /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
  0,            /* iInterface: Index of string descriptor */
  /******************** Descriptor of Custom HID HID ********************/
  /* 18 */
  0x09,         /* bLength: HID Descriptor size */
  HID_DESCRIPTOR_TYPE, /* bDescriptorType: HID */
  0x10,         /* bcdHID: HID Class Spec release number */
  0x01,
  0x00,         /* bCountryCode: Hardware target country */
  0x01,         /* bNumDescriptors: Number of HID class descriptors to follow */
  0x22,         /* bDescriptorType */
  CUSTOMHID_SIZ_REPORT_DESC&0xFF,/* wItemLength: Total length of Report descriptor */
  (CUSTOMHID_SIZ_REPORT_DESC>>8)&0xFF,
  /******************** Descriptor of Custom HID endpoints ******************/
  /* 27 */
  0x07,          /* bLength: Endpoint Descriptor size */
  USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: */
  HID_IN_EP,          /* bEndpointAddress: Endpoint Address (IN) */
  0x03,          /* bmAttributes: Interrupt endpoint */
  HID_IN_PACKET,          /* wMaxPacketSize: 64 Bytes max */
  0x00,
  HID_FS_BINTERVAL,          /* bInterval: Polling Interval (32 ms) */
  /* 34 */
    
  0x07,	/* bLength: Endpoint Descriptor size */
  USB_ENDPOINT_DESCRIPTOR_TYPE,	/* bDescriptorType: */
  HID_OUT_EP,	        /* bEndpointAddress: Endpoint Address (OUT) */
  0x03,	        /* bmAttributes: Interrupt endpoint */
  HID_OUT_PACKET,	        /* wMaxPacketSize: 64 Bytes max  */
  0x00,
  HID_FS_BINTERVAL,	        /* bInterval: Polling Interval (32 ms) */
  /* 41 */
}; /* CustomHID_ConfigDescriptor */


//static uint8_t USBD_HID_Desc[USB_HID_DESC_SIZ] =
//{
//  /* 18 */
//  0x09,         /*bLength: HID Descriptor size*/
//  HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
//  0x11,         /*bcdHID: HID Class Spec release number*/
//  0x01,
//  0x00,         /*bCountryCode: Hardware target country*/
//  0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
//  0x22,         /*bDescriptorType*/
//  CUSTOMHID_SIZ_REPORT_DESC,/*wItemLength: Total length of Report descriptor*/
//  0x00,
//};


static uint8_t CustomHID_ReportDescriptor[CUSTOMHID_SIZ_REPORT_DESC] = 
{                    
  0x06, 0xFF, 0x00,      /* USAGE_PAGE (Vendor Page: 0xFF00) */                       
  0x09, 0x01,            /* USAGE (Demo Kit)               */    
  0xa1, 0x01,            /* COLLECTION (Application)       */            
  /* 7 */
  
  /* Led 2 */
  0x85, 0x02,            /*     REPORT_ID 2		     */
  0x09, 0x02,            /*     USAGE (LED 2)	             */
  0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */          
  0x25, 0x01,            /*     LOGICAL_MAXIMUM (1)        */           
  0x75, 0x08,            /*     REPORT_SIZE (8)            */        
  0x95, 0x3F,            /*     REPORT_COUNT (1)           */       
  0xB1, 0x82,             /*    FEATURE (Data,Var,Abs,Vol) */     

  0x85, 0x02,            /*     REPORT_ID (2)              */
  0x09, 0x02,            /*     USAGE (LED 2)              */
  0x91, 0x82,            /*     OUTPUT (Data,Var,Abs,Vol)  */
  /* 27 */
  
  /* Led 3 */        
  0x85, 0x03,            /*     REPORT_ID (3)		     */
  0x09, 0x03,            /*     USAGE (LED 3)	             */
  0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */          
  0x25, 0x01,            /*     LOGICAL_MAXIMUM (1)        */           
  0x75, 0x08,            /*     REPORT_SIZE (8)            */        
  0x95, 0x3F,            /*     REPORT_COUNT (1)           */       
  0xB1, 0x82,             /*    FEATURE (Data,Var,Abs,Vol) */     

  0x85, 0x03,            /*     REPORT_ID (3)              */
  0x09, 0x03,            /*     USAGE (LED 3)              */
  0x91, 0x82,            /*     OUTPUT (Data,Var,Abs,Vol)  */
  /* 47 */
  
  /* Led 4 */
  0x85, 0x04,            /*     REPORT_ID 4)		     */
  0x09, 0x04,            /*     USAGE (LED 4)	             */
  0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */          
  0x25, 0x01,            /*     LOGICAL_MAXIMUM (1)        */           
  0x75, 0x08,            /*     REPORT_SIZE (8)            */        
  0x95, 0x3F,            /*     REPORT_COUNT (1)           */       
  0xB1, 0x82,            /*     FEATURE (Data,Var,Abs,Vol) */     

  0x85, 0x04,            /*     REPORT_ID (4)              */
  0x09, 0x04,            /*     USAGE (LED 4)              */
  0x91, 0x82,            /*     OUTPUT (Data,Var,Abs,Vol)  */
  /* 67 */
  
  /* key Push Button */  
  0x85, 0x05,            /*     REPORT_ID (5)              */
  0x09, 0x05,            /*     USAGE (Push Button)        */      
  0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */      
  0x25, 0x01,            /*     LOGICAL_MAXIMUM (1)        */      
  0x75, 0x01,            /*     REPORT_SIZE (1)            */  
  0x81, 0x82,            /*     INPUT (Data,Var,Abs,Vol)   */   
  
  0x09, 0x05,            /*     USAGE (Push Button)        */               
  0x75, 0x01,            /*     REPORT_SIZE (1)            */           
  0xb1, 0x82,            /*     FEATURE (Data,Var,Abs,Vol) */  
       
  0x75, 0x07,            /*     REPORT_SIZE (7)            */           
  0x81, 0x83,            /*     INPUT (Cnst,Var,Abs,Vol)   */                    
  0x85, 0x05,            /*     REPORT_ID (2)              */         
                  
  0x75, 0x07,            /*     REPORT_SIZE (7)            */           
  0xb1, 0x83,            /*     FEATURE (Cnst,Var,Abs,Vol) */                      
  /* 95 */
 
  /* Data OUT */  
  0x85, 0xF0,             /*     REPORT_ID (7)              */  
  0x09, 0x06,             /*     USAGE                      */ 
  0x15, 0x00,             /*     LOGICAL_MINIMUM (0)        */ 
  0x26, 0x00,0xff,        /*     LOGICAL_MAXIMUM (255)      */ 
  0x75, 0x08,             /*     REPORT_SIZE (8)            */ 
  0x95, 0x3F,             /*     REPORT_COUNT (64)          */ 
  0x91, 0x02,             /*     OUTPUT(Data,Var,Abs,Vol)   */ 
  /* 110 */
  
  /* Data IN */ 
  0x85, 0xF0,             /*     REPORT_ID (7)              */  
  0x09, 0x07,             /*     USAGE                      */ 
  0x15, 0x00,             /*     LOGICAL_MINIMUM (0)        */ 
  0x26, 0x00,0xff,        /*     LOGICAL_MAXIMUM (255)      */ 
  0x75, 0x08,             /*     REPORT_SIZE (8)            */ 
  0x95, 0x3F,             /*     REPORT_COUNT (64)          */ 
  0x81, 0x82,             /*     INPUT(Data,Var,Abs,Vol)    */ 
                
  /* 125 */
  0xc0 	                 /*     END_COLLECTION	             */
}; /* CustomHID_ReportDescriptor */


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
  
  USB_DCD_EPT_Open(pusbdev,
              HID_OUT_EP,
              HID_OUT_PACKET,
              USB_OTG_EPT_INT);
  
  
  USB_DCD_EPT_PrepareRx(pusbdev, HID_OUT_EP, hid_rxBuf, HID_OUT_PACKET);
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
  USB_DCD_EPT_Close (pusbdev , HID_OUT_EP);   
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
        len = MIN(CUSTOMHID_SIZ_REPORT_DESC , req->wLength);
        pbuf = CustomHID_ReportDescriptor;
      }
      else if( req->wValue >> 8 == HID_DESCRIPTOR_TYPE)
      {
        
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
        pbuf = USBD_HID_Desc;   
#else
        pbuf = CustomHID_ConfigDescriptor + 0x12;
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
  uint8_t i = 0, sendlen = len > HID_IN_PACKET?HID_IN_PACKET:len;
  for ( i = 0; i < sendlen; i ++ )
  {
    hid_txBuf[i] = report[i];
  }
  if (pusbdev->dev.device_status == USB_OTG_CONFIGURED )
  {
    USB_DCD_EPT_Tx (pusbdev, HID_IN_EP, hid_txBuf, sendlen);
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
  *length = sizeof (CustomHID_ConfigDescriptor);
  return CustomHID_ConfigDescriptor;
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
  * @brief  usb_device_hid_dataout
  *         handle data IN Stage
  * @param  pusbdev: device instance
  * @param  bepnum: endpoint index
  * @retval status
  */
uint8_t  usb_device_hid_dataout (void  *pusbdev, 
                              uint8_t bepnum)
{
  BitState Led_State;
  uint8_t i = 0;

  
  if ( hid_rxBuf[1] == 0 )
  {
    Led_State = Bit_RESET;
  }
  else
  {
    Led_State = Bit_SET;
  }
  
  switch (hid_rxBuf[0])
  {
    case 2: /* Led 2 */
     if (Led_State != Bit_RESET)
     {
       AT32_LEDn_ON(LED2);
     }
     else
     {
       AT32_LEDn_OFF(LED2);
     }
      break;
    case 3: /* Led 3 */
     if (Led_State != Bit_RESET)
     {
       AT32_LEDn_ON(LED3);
     }
     else
     {
       AT32_LEDn_OFF(LED3);
     }
      break;
    case 4: /* Led 4 */
     if (Led_State != Bit_RESET)
     {
       AT32_LEDn_ON(LED4);
     }
     else
     {
       AT32_LEDn_OFF(LED4);
     }
     break;
    case 0xF0:
//      usb_device_hid_sendreport(pusbdev, hid_rxBuf, ((USB_OTG_CORE_HANDLE*)pusbdev)->dev.out_ept[bepnum].xfer_count);
      for ( i = 0; i < ((USB_OTG_CORE_HANDLE*)pusbdev)->dev.out_ept[bepnum].xfer_count; i ++ )
      {
        Rxbuffer[i] = hid_rxBuf[i];
      }
      receive_len = ((USB_OTG_CORE_HANDLE*)pusbdev)->dev.out_ept[bepnum].xfer_count;
      receive_flag = 1;
     break;
    default:
      AT32_LEDn_OFF(LED2);
      AT32_LEDn_OFF(LED3);
      AT32_LEDn_OFF(LED4); 
    break;
  }
  USB_DCD_EPT_PrepareRx(pusbdev, HID_OUT_EP, hid_rxBuf, HID_OUT_PACKET);
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

