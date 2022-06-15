/**
  ******************************************************************************
  * File   : usbd_hid_keyboard.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file provides the HID core functions.
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include "usbd_hid_keyboard.h"
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
uint8_t  usb_device_hid_sof (void  *pusbdev);
void ASCII_To_HID_Keyboard(USB_OTG_CORE_HANDLE *pusbdev,uint8_t ASCII_Data);
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
  usb_device_hid_sof, /*SOF */
  NULL,
  NULL,      
  usb_device_hid_getcfgdesc,
#ifdef USB_OTG_HS_CORE  
  usb_device_hid_getcfgdesc, /* use same config as per FS */
#endif  
};

#define USB_FIFO_MAX     1024
#define USB_REC_LEN	 	200	
typedef struct _usb_fifo
{
  uint8_t  fifo[USB_FIFO_MAX];
  uint16_t wrpointer;      /*write pointer*/
  uint16_t curpointer;      /*cur pointer*/
  uint16_t remaindpointer; /*remaind pointer*/
  uint16_t total;
}usb_fifo;

usb_fifo keyboard_usb_txfifo;					
uint8_t usb_send_flag = 1;
uint8_t Keyboad_Buf[8]={0,0,6,0,0,0,0,0};

#define SHIFT 0x80
const unsigned char _asciimap[128] =
{
	0x00,             // NUL
	0x00,             // SOH
	0x00,             // STX
	0x00,             // ETX
	0x00,             // EOT
	0x00,             // ENQ
	0x00,             // ACK
	0x00,             // BEL
	0x2a,             // BS	Backspace
	0x2b,             // TAB	Tab
	0x28,             // LF	Enter
	0x00,             // VT
	0x00,             // FF
	0x00,             // CR
	0x00,             // SO
	0x00,             // SI
	0x00,             // DEL
	0x00,             // DC1
	0x00,             // DC2
	0x00,             // DC3
	0x00,             // DC4
	0x00,             // NAK
	0x00,             // SYN
	0x00,             // ETB
	0x00,             // CAN
	0x00,             // EM
	0x00,             // SUB
	0x00,             // ESC
	0x00,             // FS
	0x00,             // GS
	0x00,             // RS
	0x00,             // US

	0x2c,		   //  ' '
	0x1e|SHIFT,	   // !
	0x34|SHIFT,	   // "
	0x20|SHIFT,    // #
	0x21|SHIFT,    // $
	0x22|SHIFT,    // %
	0x24|SHIFT,    // &
	0x34,          // '
	0x26|SHIFT,    // (
	0x27|SHIFT,    // )
	0x25|SHIFT,    // *
	0x2e|SHIFT,    // +
	0x36,          // ,
	0x2d,          // -
	0x37,          // .
	0x38,          // /
	0x27,          // 0
	0x1e,          // 1
	0x1f,          // 2
	0x20,          // 3
	0x21,          // 4
	0x22,          // 5
	0x23,          // 6
	0x24,          // 7
	0x25,          // 8
	0x26,          // 9
	0x33|SHIFT,      // :
	0x33,          // ;
	0x36|SHIFT,      // <
	0x2e,          // =
	0x37|SHIFT,      // >
	0x38|SHIFT,      // ?
	0x1f|SHIFT,      // @
	0x04|SHIFT,      // A
	0x05|SHIFT,      // B
	0x06|SHIFT,      // C
	0x07|SHIFT,      // D
	0x08|SHIFT,      // E
	0x09|SHIFT,      // F
	0x0a|SHIFT,      // G
	0x0b|SHIFT,      // H
	0x0c|SHIFT,      // I
	0x0d|SHIFT,      // J
	0x0e|SHIFT,      // K
	0x0f|SHIFT,      // L
	0x10|SHIFT,      // M
	0x11|SHIFT,      // N
	0x12|SHIFT,      // O
	0x13|SHIFT,      // P
	0x14|SHIFT,      // Q
	0x15|SHIFT,      // R
	0x16|SHIFT,      // S
	0x17|SHIFT,      // T
	0x18|SHIFT,      // U
	0x19|SHIFT,      // V
	0x1a|SHIFT,      // W
	0x1b|SHIFT,      // X
	0x1c|SHIFT,      // Y
	0x1d|SHIFT,      // Z
	0x2f,          // [
	0x31,          // bslash
	0x30,          // ]
	0x23|SHIFT,    // ^
	0x2d|SHIFT,    // _
	0x35,          // `
	0x04,          // a
	0x05,          // b
	0x06,          // c
	0x07,          // d
	0x08,          // e
	0x09,          // f
	0x0a,          // g
	0x0b,          // h
	0x0c,          // i
	0x0d,          // j
	0x0e,          // k
	0x0f,          // l
	0x10,          // m
	0x11,          // n
	0x12,          // o
	0x13,          // p
	0x14,          // q
	0x15,          // r
	0x16,          // s
	0x17,          // t
	0x18,          // u
	0x19,          // v
	0x1a,          // w
	0x1b,          // x
	0x1c,          // y
	0x1d,          // z
	0x2f|SHIFT,    //
	0x31|SHIFT,    // |
	0x30|SHIFT,    // }
	0x35|SHIFT,    // ~
	0				// DEL
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
  
  /************** Descriptor of Keyboard interface ****************/
  /* 09 */
  0x09,         /*bLength: Interface Descriptor size*/
  USB_INTERFACE_DESCRIPTOR_TYPE,/*bDescriptorType: Interface descriptor ept_type*/
  0x00,         /*bInterfaceNumber: Number of Interface*/
  0x00,         /*bAlternateSetting: Alternate setting*/
  0x01,         /*bNumEndpoints*/
  0x03,         /*bInterfaceClass: HID*/
  0x01,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
  0x01,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
  0,            /*iInterface: Index of string descriptor*/
  /******************** Descriptor of Keyboard HID ********************/
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
  /******************** Descriptor of Keyboar endpoint ********************/
  /* 27 */
  0x07,          /*bLength: Endpoint Descriptor size*/
  USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/
  
  HID_IN_EP,     /*bEndpointAddress: Endpoint Address (IN)*/
  0x03,          /*bmAttributes: Interrupt endpoint*/
  HID_IN_PACKET, /*wMaxPacketSize: 8 Byte max */
  0x00,
  HID_FS_BINTERVAL,          /*bInterval: Polling Interval (10 ms)*/
  /* 34 */
} ;

static uint8_t HID_KEYBOARD_ReportDesc[HID_KEYBOARD_REPORT_DESC_SIZE] =
{

  0x05, 0x01, // USAGE_PAGE (Generic Desktop)
  0x09, 0x06, // USAGE (Keyboard)
  0xa1, 0x01, // COLLECTION (Application)
  0x05, 0x07, // USAGE_PAGE (Keyboard)
  0x19, 0xe0, // USAGE_MINIMUM (Keyboard LeftControl)
  0x29, 0xe7, // USAGE_MAXIMUM (Keyboard Right GUI)
  0x15, 0x00, // LOGICAL_MINIMUM (0)
  0x25, 0x01, // LOGICAL_MAXIMUM (1)
  0x75, 0x01, // REPORT_SIZE (1)
  0x95, 0x08, // REPORT_COUNT (8)
  0x81, 0x02, // INPUT (Data,Var,Abs)
  0x95, 0x01, // REPORT_COUNT (1)
  0x75, 0x08, // REPORT_SIZE (8)
  0x81, 0x03, // INPUT (Cnst,Var,Abs)
  0x95, 0x05, // REPORT_COUNT (5)
  0x75, 0x01, // REPORT_SIZE (1)
  0x05, 0x08, // USAGE_PAGE (LEDs)
  0x19, 0x01, // USAGE_MINIMUM (Num Lock)
  0x29, 0x05, // USAGE_MAXIMUM (Kana)
  0x91, 0x02, // OUTPUT (Data,Var,Abs)
  0x95, 0x01, // REPORT_COUNT (1)
  0x75, 0x03, // REPORT_SIZE (3)
  0x91, 0x03, // OUTPUT (Cnst,Var,Abs)
  0x95, 0x06, // REPORT_COUNT (6)
  0x75, 0x08, // REPORT_SIZE (8)
  0x15, 0x00, // LOGICAL_MINIMUM (0)
  0x25, 0xFF, // LOGICAL_MAXIMUM (255)
  0x05, 0x07, // USAGE_PAGE (Keyboard)
  0x19, 0x00, // USAGE_MINIMUM (Reserved (no event indicated))
  0x29, 0x65, // USAGE_MAXIMUM (Keyboard Application)
  0x81, 0x00, // INPUT (Data,Ary,Abs)
  0xc0  // END_COLLECTION 
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
      usb_send_flag = 0;
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
        len = MIN(HID_KEYBOARD_REPORT_DESC_SIZE , req->wLength);
        pbuf = HID_KEYBOARD_ReportDesc;
      }
      else if( req->wValue >> 8 == HID_DESCRIPTOR_TYPE)
      {
        pbuf = usb_device_hid_cfgdesc + 0x12;
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
  volatile uint16_t dwSendLen = 0;
	uint32_t SendPtr = 0;
  /* Ensure that the FIFO is empty before a new transfer, this condition could 
  be caused by  a new transfer before the end of the previous transfer */
  USB_DCD_EPT_Flush(pusbdev, HID_IN_EP);
  	/*no data need send*/
	if (keyboard_usb_txfifo.wrpointer == keyboard_usb_txfifo.curpointer )
  {
    usb_send_flag = 0;
		return 0;
  }
	
	if ( keyboard_usb_txfifo.wrpointer > keyboard_usb_txfifo.curpointer )
	{
	    dwSendLen = keyboard_usb_txfifo.wrpointer - keyboard_usb_txfifo.curpointer;
	}		
	else
	{
		dwSendLen = USB_FIFO_MAX - keyboard_usb_txfifo.curpointer;
	}
	SendPtr = keyboard_usb_txfifo.curpointer;

	keyboard_usb_txfifo.curpointer += 1;
	
	if ( keyboard_usb_txfifo.curpointer >= USB_FIFO_MAX )
		keyboard_usb_txfifo.curpointer = 0;

  usb_send_flag = 1;
  ASCII_To_HID_Keyboard((USB_OTG_CORE_HANDLE *)pusbdev, keyboard_usb_txfifo.fifo[SendPtr]);
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
  * @brief  usb_device_hid_sof 
  *         sof function
  * @param  pusbdev: device instance
  * @retval status
  */
uint8_t  usb_device_hid_sof (void  *pusbdev)
{
   USB_OTG_CORE_HANDLE *usbdev = (USB_OTG_CORE_HANDLE *)pusbdev;
  if (usbdev->dev.device_status == USB_OTG_CONFIGURED && usb_send_flag == 0)
  {
    usb_device_hid_datain(pusbdev, HID_IN_EP);
  }
  return USB_DEVICE_OK;
}


/**
  * @brief  USB_Keyboard_Send_DATA 
  * @param  u8SendBuffer: buffer
  * @param  u16Sendlen: buffer len
  * @retval polling interval
  */
uint16_t USB_Keyboard_Send_String (uint8_t *u8SendBuffer, uint16_t u16Sendlen)
{
  uint16_t  i = 0;
  if ( keyboard_usb_txfifo.wrpointer >= USB_FIFO_MAX )
      keyboard_usb_txfifo.wrpointer = 0;
    
  for (i = 0 ; keyboard_usb_txfifo.wrpointer < USB_FIFO_MAX && i <  u16Sendlen; i ++ )
  {
      keyboard_usb_txfifo.fifo[keyboard_usb_txfifo.wrpointer] = u8SendBuffer[i];
      keyboard_usb_txfifo.wrpointer ++;
      if ( keyboard_usb_txfifo.wrpointer >= USB_FIFO_MAX )
          keyboard_usb_txfifo.wrpointer = 0;  
  }
  return u16Sendlen;
}


/**
  * @brief  USB_Keyboard_Send_DATA 
  * @param  u8SendBuffer: buffer
  * @param  u16Sendlen: buffer len
  * @retval polling interval
  */
uint16_t USB_Keyboard_Send_Char (uint8_t u8Senddata)
{
  if ( keyboard_usb_txfifo.wrpointer >= USB_FIFO_MAX )
      keyboard_usb_txfifo.wrpointer = 0;
   keyboard_usb_txfifo.fifo[keyboard_usb_txfifo.wrpointer++] = u8Senddata;
  return 1;
}

/**
  * @brief  ASCII_To_HID_Keyboard 
  *        ASCII to Keyboard convert
  * @param  pusbdev: device instance
  * @param  ASCII_Data: ASCII value
  * @retval None
  */
void ASCII_To_HID_Keyboard(USB_OTG_CORE_HANDLE *pusbdev,uint8_t ASCII_Data)
{
  u8 Key_Data =0;
  static u8 Templata = 0;
  
  if(ASCII_Data>=128)
  {
    ASCII_Data =0;
  }
  else
  {
    ASCII_Data = _asciimap[ASCII_Data];
    if (!ASCII_Data) {
      ASCII_Data = 0;
			//setWriteError();
		}
		if (ASCII_Data & SHIFT) {						// it's a capital letter or other character reached with shift
			Key_Data= 0x02;	// the left shift modifier
			ASCII_Data &= 0x7F;
		}
  }
  if((Templata == ASCII_Data)&(ASCII_Data!=0))
  {
		if(keyboard_usb_txfifo.curpointer == 0)
		{
			keyboard_usb_txfifo.curpointer = USB_FIFO_MAX-1;
		}else{
			keyboard_usb_txfifo.curpointer -= 1;
		}
    Keyboad_Buf[0] = 0;
    Keyboad_Buf[2] = 0;
    USB_DCD_EPT_Tx (pusbdev, HID_IN_EP, Keyboad_Buf, 8);
    Templata = 0;
  }else{
    
  Keyboad_Buf[0] = Key_Data;
  Keyboad_Buf[2] = ASCII_Data;

  USB_DCD_EPT_Tx (pusbdev, HID_IN_EP, Keyboad_Buf, 8);
  Templata = ASCII_Data;
    
  }
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

