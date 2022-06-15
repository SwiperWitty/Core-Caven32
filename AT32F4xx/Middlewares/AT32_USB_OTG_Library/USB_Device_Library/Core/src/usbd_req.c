/**
  ******************************************************************************
  * File   : usbd_req.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file provides the standard USB requests following chapter 9.
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include "usbd_req.h"
#include "usbd_ioreq.h"
#include "usbd_desc.h"


/** @addtogroup AT32_USB_OTG_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_REQ 
  * @brief USB standard requests module
  * @{
  */ 

/** @defgroup USBD_REQ_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_REQ_Private_Defines
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup USBD_REQ_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_REQ_Private_Variables
  * @{
  */ 
extern __IO USB_OTG_DCTL_Type SET_TEST_MODE;
uint32_t USBD_ep_status  = 0; 

uint32_t  USBD_default_cfg = 0;

uint32_t  USBD_cfg_status = 0;  

/**
  * @}
  */ 


/** @defgroup USBD_REQ_Private_FunctionPrototypes
  * @{
  */ 
static void USB_Device_GetDescriptor(USB_OTG_CORE_HANDLE  *pusbdev, 
                               USB_SETUP_REQEST *req);

static void USB_Device_SetAddress(USB_OTG_CORE_HANDLE  *pusbdev, 
                            USB_SETUP_REQEST *req);

static void USB_Device_SetConfig(USB_OTG_CORE_HANDLE  *pusbdev, 
                           USB_SETUP_REQEST *req);

static void USB_Device_GetConfig(USB_OTG_CORE_HANDLE  *pusbdev, 
                           USB_SETUP_REQEST *req);

static void USB_Device_GetStatus(USB_OTG_CORE_HANDLE  *pusbdev, 
                           USB_SETUP_REQEST *req);

static void USB_Device_SetFeature(USB_OTG_CORE_HANDLE  *pusbdev, 
                            USB_SETUP_REQEST *req);

static void USB_Device_ClrFeature(USB_OTG_CORE_HANDLE  *pusbdev, 
                            USB_SETUP_REQEST *req);

static uint8_t USB_Device_GetLen(uint8_t *buf);
/**
  * @}
  */ 


/** @defgroup USBD_REQ_Private_Functions
  * @{
  */ 


/**
* @brief  USB_Device_StdDevReq
*         Handle standard usb device requests
* @param  pusbdev: device instance
* @param  req: usb request
* @retval status
*/
USB_Device_Status  USB_Device_StdDevReq (USB_OTG_CORE_HANDLE  *pusbdev, USB_SETUP_REQEST  *req)
{
  USB_Device_Status ret = USB_DEVICE_OK;  
  
  switch (req->bRequest) 
  {
  case USB_REQUEST_GET_DESCRIPTOR: 
    
    USB_Device_GetDescriptor (pusbdev, req) ;
    break;
    
  case USB_REQUEST_SET_ADDRESS:                      
    USB_Device_SetAddress(pusbdev, req);
    break;
    
  case USB_REQUEST_SET_CONFIGURATION:                    
    USB_Device_SetConfig (pusbdev , req);
    break;
    
  case USB_REQUEST_GET_CONFIGURATION:                 
    USB_Device_GetConfig (pusbdev , req);
    break;
    
  case USB_REQUEST_GET_STATUS:                                  
    USB_Device_GetStatus (pusbdev , req);
    break;
    
    
  case USB_REQUEST_SET_FEATURE:   
    USB_Device_SetFeature (pusbdev , req);    
    break;
    
  case USB_REQUEST_CLEAR_FEATURE:                                   
    USB_Device_ClrFeature (pusbdev , req);
    break;
    
  default:  
    USB_Device_CtlError(pusbdev , req);
    break;
  }
  
  return ret;
}

/**
* @brief  USB_Device_StdItfReq
*         Handle standard usb interface requests
* @param  pusbdev: USB OTG device instance
* @param  req: usb request
* @retval status
*/
USB_Device_Status  USB_Device_StdItfReq (USB_OTG_CORE_HANDLE  *pusbdev, USB_SETUP_REQEST  *req)
{
  USB_Device_Status ret = USB_DEVICE_OK; 
  
  switch (pusbdev->dev.device_status) 
  {
  case USB_OTG_CONFIGURED:
    
    if (LOBYTE(req->wIndex) <= USBD_ITF_MAX_NUM) 
    {
      ret = (USB_Device_Status)pusbdev->dev.class_cb->Setup (pusbdev, req); 
      
      if((req->wLength == 0)&& (ret == USB_DEVICE_OK))
      {
         USB_Device_CtrlSendStatus(pusbdev);
      }
    } 
    else 
    {                                               
       USB_Device_CtlError(pusbdev , req);
    }
    break;
    
  default:
     USB_Device_CtlError(pusbdev , req);
    break;
  }
  return ret;
}

/**
* @brief  USB_Device_StdEPReq
*         Handle standard usb endpoint requests
* @param  pusbdev: USB OTG device instance
* @param  req: usb request
* @retval status
*/
USB_Device_Status  USB_Device_StdEPReq (USB_OTG_CORE_HANDLE  *pusbdev, USB_SETUP_REQEST  *req)
{
  
  uint8_t   ep_addr;
  USB_Device_Status ret = USB_DEVICE_OK; 
  
  ep_addr  = LOBYTE(req->wIndex);   

  /* Check the class specific requests before going to standard request */
  if ((req->bmRequest & USB_REQUEST_TYPE_MASK) == USB_REQUEST_TYPE_CLASS)
  {
    pusbdev->dev.class_cb->Setup (pusbdev, req);
    return ret;
  }

  switch (req->bRequest) 
  {  
  case USB_REQUEST_SET_FEATURE :
    
    switch (pusbdev->dev.device_status) 
    {
    case USB_OTG_ADDRESSED:          
      if ((ep_addr != 0x00) && (ep_addr != 0x80)) 
      {
        USB_DCD_EPT_Stall(pusbdev , ep_addr);
      }
      break;	
      
    case USB_OTG_CONFIGURED:   
      if (req->wValue == USB_FEATURE_EP_HALT)
      {
        if ((ep_addr != 0x00) && (ep_addr != 0x80)) 
        { 
          USB_DCD_EPT_Stall(pusbdev , ep_addr);
          
        }
      }
      pusbdev->dev.class_cb->Setup (pusbdev, req);   
      USB_Device_CtrlSendStatus(pusbdev);
      
      break;
      
    default:                         
      USB_Device_CtlError(pusbdev , req);
      break;    
    }
    break;
    
  case USB_REQUEST_CLEAR_FEATURE :
    
    switch (pusbdev->dev.device_status) 
    {
    case USB_OTG_ADDRESSED:          
      if ((ep_addr != 0x00) && (ep_addr != 0x80)) 
      {
        USB_DCD_EPT_Stall(pusbdev , ep_addr);
      }
      break;	
      
    case USB_OTG_CONFIGURED:   
      if (req->wValue == USB_FEATURE_EP_HALT)
      {
        if ((ep_addr != 0x00) && (ep_addr != 0x80)) 
        {        
          USB_DCD_EPT_ClrStall(pusbdev , ep_addr);
          pusbdev->dev.class_cb->Setup (pusbdev, req);
        }
        USB_Device_CtrlSendStatus(pusbdev);
      }
      break;
      
    default:                         
       USB_Device_CtlError(pusbdev , req);
      break;    
    }
    break;
    
  case USB_REQUEST_GET_STATUS:                  
    switch (pusbdev->dev.device_status) 
    {
    case USB_OTG_ADDRESSED:          
      if ((ep_addr != 0x00) && (ep_addr != 0x80)) 
      {
        USB_DCD_EPT_Stall(pusbdev , ep_addr);
      }
      break;	
      
    case USB_OTG_CONFIGURED:         
      
      
      if ((ep_addr & 0x80)== 0x80)
      {
        if(pusbdev->dev.in_ept[ep_addr & 0x7F].is_stall)
        {
          USBD_ep_status = 0x0001;     
        }
        else
        {
          USBD_ep_status = 0x0000;  
        }
      }
      else if ((ep_addr & 0x80)== 0x00)
      {
        if(pusbdev->dev.out_ept[ep_addr].is_stall)
        {
          USBD_ep_status = 0x0001;     
        }
        
        else 
        {
          USBD_ep_status = 0x0000;     
        }      
      }
      
      else
      {
        /* Do Nothing */
      }
      
      USB_Device_CtrlSendData (pusbdev,
                        (uint8_t *)&USBD_ep_status,
                        2);
      break;
      
    default:                         
       USB_Device_CtlError(pusbdev , req);
      break;
    }
    break;
    
  default:
    break;
  }
  return ret;
}
/**
* @brief  USB_Device_GetDescriptor
*         Handle Get Descriptor requests
* @param  pusbdev: device instance
* @param  req: usb request
* @retval status
*/
static void USB_Device_GetDescriptor(USB_OTG_CORE_HANDLE  *pusbdev, 
                               USB_SETUP_REQEST *req)
{
  uint16_t len;
  uint8_t *pbuf;
  len = req->wLength ;
    
  switch (req->wValue >> 8)
  {
#if (USBD_LPM_ENABLED == 1)
  case USB_DESC_TYPE_BOS:
    pbuf = pusbdev->pDesc->GetBOSDescriptor(pusbdev->dev_speed, &len);
    break;
#endif
  case USB_DESC_TYPE_DEVICE:
    pbuf = pusbdev->dev.usr_device->GetDeviceDescriptor(pusbdev->cfg.speed, &len);
    break;
    
  case USB_DESC_TYPE_CONFIGURATION:
    pbuf   = (uint8_t *)pusbdev->dev.class_cb->GetConfigDescriptor(pusbdev->cfg.speed, &len); 
    pbuf[1] = USB_DESC_TYPE_CONFIGURATION;
    pusbdev->dev.pConfig_descriptor = pbuf;    
    break;
    
  case USB_DESC_TYPE_STRING:
    switch ((uint8_t)(req->wValue))
    {
    case USB_DEVICE_IDX_LANGID_STRING:
     pbuf = pusbdev->dev.usr_device->GetLangIDStrDescriptor(pusbdev->cfg.speed, &len);        
      break;
      
    case USB_DEVICE_IDX_MAFAC_STRING:
      pbuf = pusbdev->dev.usr_device->GetManufacturerStrDescriptor(pusbdev->cfg.speed, &len);
      break;
      
    case USB_DEVICE_IDX_PRODUCT_STRING:
      pbuf = pusbdev->dev.usr_device->GetProductStrDescriptor(pusbdev->cfg.speed, &len);
      break;
      
    case USB_DEVICE_IDX_SERIAL_STRING:
      pbuf = pusbdev->dev.usr_device->GetSerialStrDescriptor(pusbdev->cfg.speed, &len);
      break;
      
    case USB_DEVICE_IDX_CONFIG_STRING:
      pbuf = pusbdev->dev.usr_device->GetConfigurationStrDescriptor(pusbdev->cfg.speed, &len);
      break;
      
    case USB_DEVICE_IDX_INTERFACE_STRING:
      pbuf = pusbdev->dev.usr_device->GetInterfaceStrDescriptor(pusbdev->cfg.speed, &len);
      break;
      
    default:
#ifdef USB_SUPPORT_USER_STRING_DESC
      pbuf = pusbdev->dev.class_cb->GetUsrStrDescriptor(pusbdev->cfg.speed, (req->wValue) , &len);
      break;
#else      
       USB_Device_CtlError(pusbdev , req);
      return;
#endif /* USB_Device_CtlError(pusbdev , req)*/      
    }
    break;
  case USB_DESC_TYPE_DEVICE_QUALIFIER:                   
      USB_Device_CtlError(pusbdev , req);
      return;
 

  case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
      USB_Device_CtlError(pusbdev , req);
      return;  
  default: 
     USB_Device_CtlError(pusbdev , req);
    return;
  }
  
  if((len != 0)&& (req->wLength != 0))
  {
    
    len = MIN(len , req->wLength);
    
    USB_Device_CtrlSendData (pusbdev, 
                      pbuf,
                      len);
  }
  
}

/**
* @brief  USB_Device_SetAddress
*         Set device address
* @param  pusbdev: device instance
* @param  req: usb request
* @retval status
*/
static void USB_Device_SetAddress(USB_OTG_CORE_HANDLE  *pusbdev, 
                            USB_SETUP_REQEST *req)
{
  uint8_t  dev_addr; 
  
  if ((req->wIndex == 0) && (req->wLength == 0)) 
  {
    dev_addr = (uint8_t)(req->wValue) & 0x7F;     
    
    if (pusbdev->dev.device_status == USB_OTG_CONFIGURED) 
    {
      USB_Device_CtlError(pusbdev , req);
    } 
    else 
    {
      pusbdev->dev.device_address = dev_addr;
      USB_DCD_EPT_SetAddress(pusbdev, dev_addr);               
      USB_Device_CtrlSendStatus(pusbdev);                         
      
      if (dev_addr != 0) 
      {
        pusbdev->dev.device_status  = USB_OTG_ADDRESSED;
      } 
      else 
      {
        pusbdev->dev.device_status  = USB_OTG_DEFAULT; 
      }
    }
  } 
  else 
  {
     USB_Device_CtlError(pusbdev , req);                        
  } 
}

/**
* @brief  USB_Device_SetConfig
*         Handle Set device configuration request
* @param  pusbdev: device instance
* @param  req: usb request
* @retval status
*/
static void USB_Device_SetConfig(USB_OTG_CORE_HANDLE  *pusbdev, 
                           USB_SETUP_REQEST *req)
{
  
  static uint8_t  cfgidx;
  
  cfgidx = (uint8_t)(req->wValue);                 
  
  if (cfgidx > USBD_CFG_MAX_NUM ) 
  {            
     USB_Device_CtlError(pusbdev , req);                              
  } 
  else 
  {
    switch (pusbdev->dev.device_status) 
    {
    case USB_OTG_ADDRESSED:
      if (cfgidx) 
      {                                			   							   							   				
        pusbdev->dev.device_config = cfgidx;
        pusbdev->dev.device_status = USB_OTG_CONFIGURED;
        USB_Device_SetCfg(pusbdev , cfgidx);
        USB_Device_CtrlSendStatus(pusbdev);
      }
      else 
      {
         USB_Device_CtrlSendStatus(pusbdev);
      }
      break;
      
    case USB_OTG_CONFIGURED:
      if (cfgidx == 0) 
      {                           
        pusbdev->dev.device_status = USB_OTG_ADDRESSED;
        pusbdev->dev.device_config = cfgidx;          
        USB_Device_ClrCfg(pusbdev , cfgidx);
        USB_Device_CtrlSendStatus(pusbdev);
        
      } 
      else  if (cfgidx != pusbdev->dev.device_config) 
      {
        /* Clear old configuration */
        USB_Device_ClrCfg(pusbdev , pusbdev->dev.device_config);
        
        /* set new configuration */
        pusbdev->dev.device_config = cfgidx;
        USB_Device_SetCfg(pusbdev , cfgidx);
        USB_Device_CtrlSendStatus(pusbdev);
      }
      else
      {
        USB_Device_CtrlSendStatus(pusbdev);
      }
      break;
      
    default:					
       USB_Device_CtlError(pusbdev , req);                     
      break;
    }
  }
}

/**
* @brief  USB_Device_GetConfig
*         Handle Get device configuration request
* @param  pusbdev: device instance
* @param  req: usb request
* @retval status
*/
static void USB_Device_GetConfig(USB_OTG_CORE_HANDLE  *pusbdev, 
                           USB_SETUP_REQEST *req)
{
 
  if (req->wLength != 1) 
  {                   
     USB_Device_CtlError(pusbdev , req);
  }
  else 
  {
    switch (pusbdev->dev.device_status )  
    {
    case USB_OTG_ADDRESSED:                     
      
      USB_Device_CtrlSendData (pusbdev, 
                        (uint8_t *)&USBD_default_cfg,
                        1);
      break;
      
    case USB_OTG_CONFIGURED:                   
      
      USB_Device_CtrlSendData (pusbdev, 
                        &pusbdev->dev.device_config,
                        1);
      break;
      
    default:
       USB_Device_CtlError(pusbdev , req);
      break;
    }
  }
}

/**
* @brief  USB_Device_GetStatus
*         Handle Get Status request
* @param  pusbdev: device instance
* @param  req: usb request
* @retval status
*/
static void USB_Device_GetStatus(USB_OTG_CORE_HANDLE  *pusbdev, 
                           USB_SETUP_REQEST *req)
{
  
    
  switch (pusbdev->dev.device_status) 
  {
  case USB_OTG_ADDRESSED:
  case USB_OTG_CONFIGURED:
    
#ifdef USBD_SELF_POWERED
    USBD_cfg_status = USB_CONFIG_SELF_POWERED;                                    
#else
    USBD_cfg_status = 0x00;                                    
#endif
                      
    if (pusbdev->dev.DeviceRemoteWakeup) 
    {
      USBD_cfg_status |= USB_CONFIG_REMOTE_WAKEUP;                                
    }
    
    USB_Device_CtrlSendData (pusbdev, 
                      (uint8_t *)&USBD_cfg_status,
                      2);
    break;
    
  default :
    USB_Device_CtlError(pusbdev , req);                        
    break;
  }
}


/**
* @brief  USB_Device_SetFeature
*         Handle Set device feature request
* @param  pusbdev: device instance
* @param  req: usb request
* @retval status
*/
static void USB_Device_SetFeature(USB_OTG_CORE_HANDLE  *pusbdev, 
                            USB_SETUP_REQEST *req)
{

  USB_OTG_DCTL_Type     dctl;
  uint8_t test_mode = 0;
 
  if (req->wValue == USB_FEATURE_REMOTE_WAKEUP)
  {
    pusbdev->dev.DeviceRemoteWakeup = 1;  
    pusbdev->dev.class_cb->Setup (pusbdev, req);   
    USB_Device_CtrlSendStatus(pusbdev);
  }

  else if ((req->wValue == USB_FEATURE_TEST_MODE) && 
           ((req->wIndex & 0xFF) == 0))
  {
    dctl.u32val = USB_OTG_READ_R32(&pusbdev->regs.DREGS->DCTRL);
    
    test_mode = req->wIndex >> 8;
    switch (test_mode) 
    {
    case 1: /* TEST_J */
      dctl.b.tstctrl = 1;
      break;
      
    case 2: /* TEST_K */	
      dctl.b.tstctrl = 2;
      break;
      
    case 3: /* TEST_SE0_NAK */
      dctl.b.tstctrl = 3;
      break;
      
    case 4: /* TEST_PACKET */
      dctl.b.tstctrl = 4;
      break;
      
    case 5: /* TEST_FORCE_ENABLE */
      dctl.b.tstctrl = 5;
      break;
      
    default :
      dctl.b.tstctrl = 1;
      break;
    }
    SET_TEST_MODE = dctl;
    pusbdev->dev.test_mode = 1;
    USB_Device_CtrlSendStatus(pusbdev);
  }
  else
  {
    /* Do Nothing */
  }
}


/**
* @brief  USB_Device_ClrFeature
*         Handle clear device feature request
* @param  pusbdev: device instance
* @param  req: usb request
* @retval status
*/
static void USB_Device_ClrFeature(USB_OTG_CORE_HANDLE  *pusbdev, 
                            USB_SETUP_REQEST *req)
{
  switch (pusbdev->dev.device_status)
  {
  case USB_OTG_ADDRESSED:
  case USB_OTG_CONFIGURED:
    if (req->wValue == USB_FEATURE_REMOTE_WAKEUP) 
    {
      pusbdev->dev.DeviceRemoteWakeup = 0; 
      pusbdev->dev.class_cb->Setup (pusbdev, req);   
      USB_Device_CtrlSendStatus(pusbdev);
    }
    break;
    
  default :
     USB_Device_CtlError(pusbdev , req);
    break;
  }
}

/**
* @brief  USB_Device_ParseSetupRequest 
*         Copy buffer into setup structure
* @param  pusbdev: device instance
* @param  req: usb request
* @retval None
*/

void USB_Device_ParseSetupRequest( USB_OTG_CORE_HANDLE  *pusbdev,
                            USB_SETUP_REQEST *req)
{
  req->bmRequest     = *(uint8_t *)  (pusbdev->dev.setup_packet);
  req->bRequest      = *(uint8_t *)  (pusbdev->dev.setup_packet +  1);
  req->wValue        = SWAPBYTE      (pusbdev->dev.setup_packet +  2);
  req->wIndex        = SWAPBYTE      (pusbdev->dev.setup_packet +  4);
  req->wLength       = SWAPBYTE      (pusbdev->dev.setup_packet +  6);
  
  pusbdev->dev.in_ept[0].ctl_data_len = req->wLength  ;
  pusbdev->dev.device_state = USB_OTG_EPT0_SETUP;
}

/**
* @brief  USB_Device_CtlError 
*         Handle USB low level Error
* @param  pusbdev: device instance
* @param  req: usb request
* @retval None
*/

void USB_Device_CtlError( USB_OTG_CORE_HANDLE  *pusbdev,
                            USB_SETUP_REQEST *req)
{
  
  USB_DCD_EPT_Stall(pusbdev , 0x80);
  USB_DCD_EPT_Stall(pusbdev , 0);
  USB_OTG_EPT0_OutStart(pusbdev);  
}


/**
  * @brief  USB_Device_GetString
  *         Convert Ascii string into unicode one
  * @param  desc : descriptor buffer
  * @param  unicode : Formatted string buffer (unicode)
  * @param  len : descriptor length
  * @retval None
  */
void USB_Device_GetString(uint8_t *desc, uint8_t *unicode, uint16_t *len)
{
  uint8_t idx = 0;
  
  if (desc != NULL) 
  {
    *len =  USB_Device_GetLen(desc) * 2 + 2;    
    unicode[idx++] = *len;
    unicode[idx++] =  USB_DESC_TYPE_STRING;
    
    while (*desc != NULL) 
    {
      unicode[idx++] = *desc++;
      unicode[idx++] =  0x00;
    }
  } 
}

/**
  * @brief  USB_Device_GetLen
  *         return the string length
   * @param  buf : pointer to the ascii string buffer
  * @retval string length
  */
static uint8_t USB_Device_GetLen(uint8_t *buf)
{
    uint8_t  len = 0;

    while (*buf != NULL) 
    {
        len++;
        buf++;
    }

    return len;
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

