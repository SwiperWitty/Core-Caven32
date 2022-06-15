/**
  ******************************************************************************
  * File   : usbd_printer_core.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Peripheral Device Interface Layer
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usbd_printer_core.h"
#include "usbd_desc.h"
#include "usbd_req.h"
#include "at32_board.h"


/** @addtogroup AT32_USB_OTG_DEVICE_LIBRARY
  * @{
  */


/** @defgroup usbd_printer 
  * @brief usbd core module
  * @{
  */ 

/** @defgroup usbd_printer_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup usbd_printer_Private_Defines
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup usbd_printer_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup usbd_printer_Private_FunctionPrototypes
  * @{
  */

/*********************************************
   Printer Device library callbacks
 *********************************************/
uint8_t  usbd_printer_init        (void  *pusbdev, uint8_t cfgidx);
uint8_t  usbd_printer_reset       (void  *pusbdev, uint8_t cfgidx);
uint8_t  usbd_Printer_setup       (void  *pusbdev, USB_SETUP_REQEST *req);
uint8_t  usbd_printer_ep0_rxready (void  *pusbdev);
uint8_t  usbd_printer_datain      (void  *pusbdev, uint8_t epnum);
uint8_t  usbd_printer_dataout     (void  *pusbdev, uint8_t epnum);

/*********************************************
   CDC specific management functions
 *********************************************/
static uint8_t  *usb_device_printer_getcfgdesc (uint8_t speed, uint16_t *length);
#ifdef USE_USB_OTG_HS  
static uint8_t  *USBD_printer_getothercfgdesc (uint8_t speed, uint16_t *length);
#endif
/**
  * @}
  */ 

/** @defgroup usbd_cdc_Private_Variables
  * @{
  */ 
extern CDC_IF_Prop_TypeDef  APP_FOPS;
extern uint8_t USBD_DeviceDesc   [USB_SIZ_DEVICE_DESC];
uint8_t CmdBuff[CDC_CMD_PACKET_SZE];
static __IO uint32_t  usbd_cdc_AltSet = 0;
/* CDC interface class callbacks structure */
USBD_Class_cb_Type  USBD_Printer_cb = 
{
  usbd_printer_init,
  usbd_printer_reset,
  usbd_Printer_setup,
  NULL,                 /* EP0_TxSent, */
  usbd_printer_ep0_rxready,
  usbd_printer_datain,
  usbd_printer_dataout,
  NULL,
  NULL,
  NULL,     
  usb_device_printer_getcfgdesc,
#ifdef USE_USB_OTG_HS   
  USBD_printer_getothercfgdesc, /* use same cobfig as per FS */
#endif /* USE_USB_OTG_HS  */
};

/* USB CDC device Configuration Descriptor */
uint8_t usbd_printer_cfgdesc[USB_PRINTER_CONFIG_DESC_SIZ] =
{
  /*Configuration Descriptor*/
  0x09,                              /* bLength: Configuration Descriptor size */
  USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration */
  USB_PRINTER_CONFIG_DESC_SIZ,       /* wTotalLength:no of returned bytes */
  0x00,
  0x01,                              /* bNumInterfaces: 2 interface */
  0x01,                              /* bConfigurationValue: Configuration value */
  0x00,                              /* iConfiguration: Index of string descriptor describing the configuration */
  0xA0,                              /* bmAttributes: self powered */
  0x32,                              /* MaxPower 0 mA */
  
  /*---------------------------------------------------------------------------*/
  
  /*Interface Descriptor */
  0x09,                              /* bLength: Interface Descriptor size */
  USB_INTERFACE_DESCRIPTOR_TYPE,     /* bDescriptorType: Interface */
  /* Interface descriptor type */
  0x00,                              /* bInterfaceNumber: Number of Interface */
  0x00,                              /* bAlternateSetting: Alternate setting */
  0x02,                              /* bNumEndpoints: One endpoints used */
  0x07,                              /* bInterfaceClass: Printer*/
  0x01,                              /* bInterfaceSubClass */
  0x02,                              /* bInterfaceProtocol*/
  0x00,                              /* iInterface: */
  
  /*Endpoint OUT Descriptor*/
  0x07,                              /* bLength: Endpoint Descriptor size */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType: Endpoint */
  PRINTER_OUT_EP,                    /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LOBYTE(PRINTER_DATA_MAX_PACKET_SIZE),/* wMaxPacketSize: */
  HIBYTE(PRINTER_DATA_MAX_PACKET_SIZE),
  0x00,                              /* bInterval: ignore for Bulk transfer */
  
  /*Endpoint IN Descriptor*/
  0x07,                              /* bLength: Endpoint Descriptor size */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType: Endpoint */
  PRINTER_IN_EP,                         /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LOBYTE(PRINTER_DATA_MAX_PACKET_SIZE),/* wMaxPacketSize: */
  HIBYTE(PRINTER_DATA_MAX_PACKET_SIZE),
  0x00                               /* bInterval: ignore for Bulk transfer */
} ;

uint8_t DEVICE_ID[DEVICE_ID_LEN] =
{
  0x00, 0x16,
  'M', 'F', 'G',':','A','r','t','e', 'r', 'y' ,' ',
  'C','M', 'D', ':', 'E', 'S', 'C', 'P', 'O', 'S',' ',
};


static uint8_t g_port_status = 0x18;
uint8_t g_printer_data_buf[PRINTER_DATA_MAX_PACKET_SIZE];


/**
  * @}
  */ 

/** @defgroup usbd_cdc_Private_Functions
  * @{
  */ 

/**
  * @brief  usbd_printer_Init
  *         Initialize the CDC interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
uint8_t  usbd_printer_init (void  *pdev, 
                               uint8_t cfgidx)
{
  uint8_t *pbuf;

  /* Open EP IN */
  USB_DCD_EPT_Open(pdev,
              PRINTER_IN_EP,
              PRINTER_DATA_IN_PACKET_SIZE,
              USB_OTG_EPT_BULK);
  
  /* Open EP OUT */
  USB_DCD_EPT_Open(pdev,
              PRINTER_OUT_EP,
              PRINTER_DATA_OUT_PACKET_SIZE,
              USB_OTG_EPT_BULK);
  
  pbuf = (uint8_t *)USB_Device_Desc;
  pbuf[4] = DEVICE_CLASS_PRINTER;
  pbuf[5] = DEVICE_SUBCLASS_PRINTER;
  
  /*Start Recive data*/
  USB_DCD_EPT_PrepareRx(pdev, PRINTER_OUT_EP, g_printer_data_buf, PRINTER_DATA_MAX_PACKET_SIZE);
 
  return USB_DEVICE_OK;
}

/**
  * @brief  usbd_printer_Init
  *         DeInitialize the CDC layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
uint8_t  usbd_printer_reset (void  *pdev, 
                                 uint8_t cfgidx)
{
  /* Open EP IN */
  USB_DCD_EPT_Close(pdev,
              PRINTER_IN_EP);
  
  /* Open EP OUT */
  USB_DCD_EPT_Close(pdev,
              PRINTER_OUT_EP);
  
  return USB_DEVICE_OK;
}

/**
  * @brief  usbd_Printer_Setup
  *         Handle the CDC specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
uint8_t  usbd_Printer_setup (void  *pusbdev, 
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
          switch (req->bRequest )
          {
            case GET_DEVICE_ID:
              USB_Device_CtrlSendData (pusbdev,(uint8_t *)&DEVICE_ID,DEVICE_ID_LEN);
              break;
            case GET_PORT_STATUS:
              USB_Device_CtrlSendData (pusbdev,(uint8_t *)&g_port_status, 1);
              break;
            case SOFT_RESET:
              USB_DCD_EPT_PrepareRx(pusbdev, PRINTER_OUT_EP, g_printer_data_buf, PRINTER_DATA_MAX_PACKET_SIZE);
              break;
            default:
               /* STALL request */ 
              USB_Device_CtlError (pusbdev, req); 
              break;
          }
          
                  
        }
        else /* Host-to-Device request */
        {
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
    break;
  default:
    USB_Device_CtlError (pusbdev, req);
    return USB_DEVICE_FAIL;
  }
  return USB_DEVICE_OK;
}

/**
  * @brief  usbd_printer_EP0_RxReady
  *         Data received on control endpoint
  * @param  pdev: device instance
  * @retval status
  */
uint8_t  usbd_printer_ep0_rxready (void  *pusbdev)
{ 
  return USB_DEVICE_OK;
}


/**
  * @brief  usbd_printer_DataIn
  *         Data sent on non-control IN endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
uint8_t  usbd_printer_datain (void *pusbdev, uint8_t epnum)
{
  /* inform application layer that data was sent */ 
  //DCD_EP_Tx (pdev, PRINTER_IN_EP, pbuf , buf_len );
  return USB_DEVICE_OK;
}

/**
  * @brief  usbd_printer_DataOut
  *         Data received on non-control Out endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
uint8_t  usbd_printer_dataout (void *pusbdev, uint8_t epnum)
{      
  uint16_t USB_Rx_Cnt;
  
  /* Get the received data buffer and update the counter */
  USB_Rx_Cnt = ((USB_OTG_CORE_HANDLE*)pusbdev)->dev.out_ept[epnum].xfer_count;
  
  /* pass received data count to application layer */
  /* data buffer: g_printer_data_buf
  copy and deal data here:
  */
  /*start next data received*/
  USB_DCD_EPT_PrepareRx(pusbdev, PRINTER_OUT_EP, g_printer_data_buf, PRINTER_DATA_MAX_PACKET_SIZE);
  
  return USB_DEVICE_OK;
}

/**
  * @brief  USBD_printer_GetCfgDesc 
  *         Return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *usb_device_printer_getcfgdesc (uint8_t speed, uint16_t *length)
{
  *length = sizeof (usbd_printer_cfgdesc);
  return usbd_printer_cfgdesc;
}

/**
  * @brief  USBD_printer_GetCfgDesc 
  *         Return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
#ifdef USE_USB_OTG_HS 
static uint8_t  *USBD_printer_getothercfgdesc (uint8_t speed, uint16_t *length)
{
  *length = sizeof (usbd_printer_cfgdesc);
  return usbd_printer_cfgdesc;
}
#endif
/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 

