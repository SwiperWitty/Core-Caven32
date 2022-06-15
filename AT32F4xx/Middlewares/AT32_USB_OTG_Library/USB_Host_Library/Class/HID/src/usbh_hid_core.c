/**
  ******************************************************************************
  * File   : usbh_hid_core.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file is the HID Layer Handlers for USB Host HID class.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usbh_hid_core.h"
#include "usbh_hid_mouse.h"
#include "usbh_hid_keybd.h"

/** @addtogroup USBH_LIB
* @{
*/

/** @addtogroup USBH_CLASS
* @{
*/

/** @addtogroup USB_HOST_HID_CLASS
* @{
*/

/** @defgroup USBH_HID_CORE 
* @brief    This file includes HID Layer Handlers for USB Host HID class.
* @{
*/ 

/** @defgroup USBH_HID_CORE_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_HID_CORE_Private_Defines
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_HID_CORE_Private_Macros
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_HID_CORE_Private_Variables
* @{
*/
HID_Machine_TypeDef HID_Machine;

HID_Report_TypeDef  HID_Report;

USB_Setup_Type      HID_Setup;

USB_HOST_HIDDesc_Type  HID_Desc; 

__IO uint8_t start_toggle = 0;
/**
* @}
*/ 


/** @defgroup USBH_HID_CORE_Private_FunctionPrototypes
* @{
*/ 

static USB_HOST_Status USBH_HID_InterfaceInit  (USB_OTG_CORE_HANDLE *pusbdev , 
                                            void *phost);

static void  USBH_ParseHIDDesc (USB_HOST_HIDDesc_Type *desc, uint8_t *buf);

static void USBH_HID_InterfaceDeInit  (USB_OTG_CORE_HANDLE *pusbdev , 
                                       void *phost);

static USB_HOST_Status USBH_HID_Handle(USB_OTG_CORE_HANDLE *pusbdev , 
                                   void *phost);

static USB_HOST_Status USBH_HID_ClassRequest(USB_OTG_CORE_HANDLE *pusbdev , 
                                         void *phost);

static USB_HOST_Status USBH_Get_HID_ReportDescriptor (USB_OTG_CORE_HANDLE *pusbdev, 
                                                  USB_HOST *phost,
                                                  uint16_t length);

static USB_HOST_Status USBH_Get_HID_Descriptor (USB_OTG_CORE_HANDLE *pusbdev,\
                                            USB_HOST *phost,  
                                            uint16_t length);

static USB_HOST_Status USBH_Set_Idle (USB_OTG_CORE_HANDLE *pusbdev, 
                                  USB_HOST *phost,
                                  uint8_t duration,
                                  uint8_t reportId);

static USB_HOST_Status USBH_Set_Protocol (USB_OTG_CORE_HANDLE *pusbdev, 
                                      USB_HOST *phost,
                                      uint8_t protocol);


USB_Host_Class_cb_Type  HID_cb = 
{
  USBH_HID_InterfaceInit,
  USBH_HID_InterfaceDeInit,
  USBH_HID_ClassRequest,
  USBH_HID_Handle
};
/**
* @}
*/ 


/** @defgroup USBH_HID_CORE_Private_Functions
* @{
*/ 

/**
* @brief  USBH_HID_InterfaceInit 
*         The function init the HID class.
* @param  pusbdev: Selected device
* @param  hdev: Selected device property
* @retval  USB_HOST_Status :Response for USB HID driver initialization
*/
static USB_HOST_Status USBH_HID_InterfaceInit ( USB_OTG_CORE_HANDLE *pusbdev, 
                                           void *phost)
{	
  uint8_t maxEP;
  USB_HOST *pphost = phost;
    
  uint8_t num =0;
  USB_HOST_Status status = USB_HOST_BUSY ;
  HID_Machine.state = HID_ERROR;
  
  
  if(pphost->device_prop.Itf_Desc[0].bInterfaceSubClass  == HID_BOOT_CODE)
  {
    /*Decode Bootclass Protocol: Mouse or Keyboard*/
    if(pphost->device_prop.Itf_Desc[0].bInterfaceProtocol == HID_KEYBRD_BOOT_CODE)
    {
      HID_Machine.cb = &HID_KEYBRD_cb;
    }
    else if(pphost->device_prop.Itf_Desc[0].bInterfaceProtocol  == HID_MOUSE_BOOT_CODE)		  
    {
      HID_Machine.cb = &HID_MOUSE_cb;
    }
    
    HID_Machine.state     = HID_IDLE;
    HID_Machine.ctl_state = HID_REQ_IDLE; 
    HID_Machine.ept_addr   = pphost->device_prop.Ept_Desc[0][0].bEndpointAddress;
    HID_Machine.length    = pphost->device_prop.Ept_Desc[0][0].wMaxPacketSize;
    HID_Machine.poll      = pphost->device_prop.Ept_Desc[0][0].bInterval ;
    
    if (HID_Machine.poll  < HID_MIN_POLL) 
    {
       HID_Machine.poll = HID_MIN_POLL;
    }

    
    /* Check fo available number of endpoints */
    /* Find the number of EPs in the Interface Descriptor */      
    /* Choose the lower number in order not to overrun the buffer allocated */
    maxEP = ( (pphost->device_prop.Itf_Desc[0].bNumEndpoints <= USBH_MAX_NUM_ENDPOINTS) ? 
             pphost->device_prop.Itf_Desc[0].bNumEndpoints :
                 USBH_MAX_NUM_ENDPOINTS); 
    
    
    /* Decode endpoint IN and OUT address from interface descriptor */
    for (num=0; num < maxEP; num++)
    {
      if(pphost->device_prop.Ept_Desc[0][num].bEndpointAddress & 0x80)
      {
        HID_Machine.HIDIntInEp = (pphost->device_prop.Ept_Desc[0][num].bEndpointAddress);
        HID_Machine.hch_num_in  =\
               USB_Host_Alloc_Channel(pusbdev, 
                                  pphost->device_prop.Ept_Desc[0][num].bEndpointAddress);
        
        /* Open channel for IN endpoint */
        USB_Host_Open_Channel  (pusbdev,
                            HID_Machine.hch_num_in,
                            pphost->device_prop.address,
                            pphost->device_prop.speed,
                            EPT_TYPE_INTR,
                            HID_Machine.length); 
      }
      else
      {
        HID_Machine.HIDIntOutEp = (pphost->device_prop.Ept_Desc[0][num].bEndpointAddress);
        HID_Machine.hch_num_out  =\
                USB_Host_Alloc_Channel(pusbdev, 
                                   pphost->device_prop.Ept_Desc[0][num].bEndpointAddress);
        
        /* Open channel for OUT endpoint */
        USB_Host_Open_Channel  (pusbdev,
                            HID_Machine.hch_num_out,
                            pphost->device_prop.address,
                            pphost->device_prop.speed,
                            EPT_TYPE_INTR,
                            HID_Machine.length); 
      }
      
    }   
    
     start_toggle =0;
     status = USB_HOST_OK; 
  }
  else
  {
    if (pphost->device_prop.Itf_Desc[0].bInterfaceClass == USB_HUB_CLASS)
    {
      printf("Hub is not supported.\r\n");
    }
    
    else if (pphost->device_prop.Itf_Desc[0].bInterfaceClass == USB_CDCC_CLASS)
    {
      printf("Communications and CDC Control device is not supported.\r\n");
    }
    
    else if (pphost->device_prop.Itf_Desc[0].bInterfaceClass == USB_MSC_CLASS)
    {
      printf("MSC device is not supported.\r\n");
    }
    
    else if (pphost->device_prop.Itf_Desc[0].bInterfaceClass == USB_PRINTER_CLASS)
    {
      printf("Printer device is not supported.\r\n");
    }
    
    else if (pphost->device_prop.Itf_Desc[0].bInterfaceClass == USB_SMARTCARD_CLASS)
    {
      printf("Smart Card device is not supported.\r\n");
    }
    
    
    else if (pphost->device_prop.Itf_Desc[0].bInterfaceClass == USB_VIDEO_CLASS)
    {
      printf("Video device  is not supported.\r\n");
    }
    
    
    else if (pphost->device_prop.Itf_Desc[0].bInterfaceClass == USB_AVD_CLASS)
    {
      printf("Audio/Video Devices is not supported.\r\n");
    }
    
    else
    {
      printf ("The attached device is not supported. \r\n");
    }
    
    pphost->user_cb->DeviceNotSupported();  
  }
  
  return status;
  
}



/**
* @brief  USBH_HID_InterfaceDeInit 
*         The function Reset the Host Channels used for the HID class.
* @param  pusbdev: Selected device
* @param  hdev: Selected device property
* @retval None
*/
void USBH_HID_InterfaceDeInit ( USB_OTG_CORE_HANDLE *pusbdev,
                               void *phost)
{	
   //USB_HOST *pphost = phost;
    
  if(HID_Machine.hch_num_in != 0x00)
  {   
    USB_OTG_HCH_Halt(pusbdev, HID_Machine.hch_num_in);
    USB_Host_Free_Channel  (pusbdev, HID_Machine.hch_num_in);
    HID_Machine.hch_num_in = 0;     /* Reset the Channel as Free */  
  }
  
  if(HID_Machine.hch_num_out != 0x00)
  {   
    USB_OTG_HCH_Halt(pusbdev, HID_Machine.hch_num_out);
    USB_Host_Free_Channel  (pusbdev, HID_Machine.hch_num_out);
    HID_Machine.hch_num_out = 0;     /* Reset the Channel as Free */  
  }
 
  start_toggle = 0;
}

/**
* @brief  USBH_HID_ClassRequest 
*         The function is responsible for handling HID Class requests
*         for HID class.
* @param  pusbdev: Selected device
* @param  hdev: Selected device property
* @retval  USB_HOST_Status :Response for USB Set Protocol request
*/
static USB_HOST_Status USBH_HID_ClassRequest(USB_OTG_CORE_HANDLE *pusbdev , 
                                         void *phost)
{   
    USB_HOST *pphost = phost;
    
  USB_HOST_Status status         = USB_HOST_BUSY;
  USB_HOST_Status classReqStatus = USB_HOST_BUSY;
  
  
  /* Switch HID state machine */
  switch (HID_Machine.ctl_state)
  {
  case HID_IDLE:  
  case HID_REQ_GET_HID_DESC:
    
    /* Get HID Desc */ 
    if (USBH_Get_HID_Descriptor (pusbdev, pphost, USB_HID_DESC_SIZE)== USB_HOST_OK)
    {
      
      USBH_ParseHIDDesc(&HID_Desc, pusbdev->host.Rx_Buffer);
      HID_Machine.ctl_state = HID_REQ_GET_REPORT_DESC;
    }
    
    break;     
  case HID_REQ_GET_REPORT_DESC:
    
    
    /* Get Report Desc */ 
    if (USBH_Get_HID_ReportDescriptor(pusbdev , pphost, HID_Desc.wItemLength) == USB_HOST_OK)
    {
      HID_Machine.ctl_state = HID_REQ_SET_IDLE;
    }
    
    break;
    
  case HID_REQ_SET_IDLE:
    
    classReqStatus = USBH_Set_Idle (pusbdev, pphost, 0, 0);
    
    /* set Idle */
    if (classReqStatus == USB_HOST_OK)
    {
      HID_Machine.ctl_state = HID_REQ_SET_PROTOCOL;  
    }
    else if(classReqStatus == USB_HOST_NOT_SUPPORTED) 
    {
      HID_Machine.ctl_state = HID_REQ_SET_PROTOCOL;        
    } 
    break; 
    
  case HID_REQ_SET_PROTOCOL:
    /* set protocol */
    if (USBH_Set_Protocol (pusbdev ,pphost, 0) == USB_HOST_OK)
    {
      HID_Machine.ctl_state = HID_REQ_IDLE;
      
      /* all requests performed*/
      status = USB_HOST_OK; 
    } 
    break;
    
  default:
    break;
  }
  
  return status; 
}


/**
* @brief  USBH_HID_Handle 
*         The function is for managing state machine for HID data transfers 
* @param  pusbdev: Selected device
* @param  hdev: Selected device property
* @retval USB_HOST_Status
*/
static USB_HOST_Status USBH_HID_Handle(USB_OTG_CORE_HANDLE *pusbdev , 
                                   void   *phost)
{
  USB_HOST *pphost = phost;
  USB_HOST_Status status = USB_HOST_OK;
  
  switch (HID_Machine.state)
  {
    
  case HID_IDLE:
    HID_Machine.cb->Init();
    HID_Machine.state = HID_SYNC;
    
  case HID_SYNC:

    /* Sync with start of Even Frame */
    if(USB_OTG_IsEvenFrame(pusbdev) == TRUE)
    {
      HID_Machine.state = HID_GET_DATA;  
    }
    break;
    
  case HID_GET_DATA:

    USB_HOST_InterruptReceiveData(pusbdev, 
                              HID_Machine.buff,
                              HID_Machine.length,
                              HID_Machine.hch_num_in);
    start_toggle = 1;
    
    HID_Machine.state = HID_POLL;
    HID_Machine.timer = USB_HCD_GetCurrentFrame(pusbdev);
    break;
    
  case HID_POLL:
    if(( USB_HCD_GetCurrentFrame(pusbdev) - HID_Machine.timer) >= HID_Machine.poll)
    {
      HID_Machine.state = HID_GET_DATA;
    }
    else if(USB_HCD_GetURB_State(pusbdev , HID_Machine.hch_num_in) == URB_DONE)
    {
      if(start_toggle == 1) /* handle data once */
      {
        start_toggle = 0;
        HID_Machine.cb->Decode(HID_Machine.buff);
      }
    }
    else if(USB_HCD_GetURB_State(pusbdev, HID_Machine.hch_num_in) == URB_STALL) /* IN Endpoint Stalled */
    {
      
      /* Issue Clear Feature on interrupt IN endpoint */ 
      if( (USB_HOST_ClrFeature(pusbdev, 
                           pphost,
                           HID_Machine.ept_addr,
                           HID_Machine.hch_num_in)) == USB_HOST_OK)
      {
        /* Change state to issue next IN token */
        HID_Machine.state = HID_GET_DATA;
        
      }
      
    }      
    break;
    
  default:
    break;
  }
  return status;
}


/**
* @brief  USBH_Get_HID_ReportDescriptor
*         Issue report Descriptor command to the device. Once the response 
*         received, parse the report descriptor and update the status.
* @param  pusbdev   : Selected device
* @param  Length : HID Report Descriptor Length
* @retval USB_HOST_Status : Response for USB HID Get Report Descriptor Request
*/
static USB_HOST_Status USBH_Get_HID_ReportDescriptor (USB_OTG_CORE_HANDLE *pusbdev,
                                                  USB_HOST *phost,
                                                  uint16_t length)
{
  
  USB_HOST_Status status;
  
  status = USB_HOST_GetDescriptor(pusbdev,
                              phost,
                              USB_REQUEST_RECIPIENT_INTERFACE
                                | USB_REQUEST_TYPE_STANDARD,                                  
                                USB_DESC_HID_REPORT, 
                                pusbdev->host.Rx_Buffer,
                                length);
  
  /* HID report descriptor is available in pusbdev->host.Rx_Buffer.
  In case of USB Boot Mode devices for In report handling ,
  HID report descriptor parsing is not required.
  In case, for supporting Non-Boot Protocol devices and output reports,
  user may parse the report descriptor*/
  
  
  return status;
}

/**
* @brief  USBH_Get_HID_Descriptor
*         Issue HID Descriptor command to the device. Once the response 
*         received, parse the report descriptor and update the status.
* @param  pusbdev   : Selected device
* @param  Length : HID Descriptor Length
* @retval USB_HOST_Status : Response for USB HID Get Report Descriptor Request
*/
static USB_HOST_Status USBH_Get_HID_Descriptor (USB_OTG_CORE_HANDLE *pusbdev,
                                            USB_HOST *phost,
                                            uint16_t length)
{
  
  USB_HOST_Status status;
  
  status = USB_HOST_GetDescriptor(pusbdev, 
                              phost,
                              USB_REQUEST_RECIPIENT_INTERFACE
                                | USB_REQUEST_TYPE_STANDARD,                                  
                                USB_DESC_HID,
                                pusbdev->host.Rx_Buffer,
                                length);
 
  return status;
}

/**
* @brief  USBH_Set_Idle
*         Set Idle State. 
* @param  pusbdev: Selected device
* @param  duration: Duration for HID Idle request
* @param  reportID : Targeted report ID for Set Idle request
* @retval USB_HOST_Status : Response for USB Set Idle request
*/
static USB_HOST_Status USBH_Set_Idle (USB_OTG_CORE_HANDLE *pusbdev,
                                  USB_HOST *phost,
                                  uint8_t duration,
                                  uint8_t reportId)
{
  
  phost->Control.setup.b.bmRequestType = USB_DIR_H2D | USB_REQUEST_RECIPIENT_INTERFACE |\
    USB_REQUEST_TYPE_CLASS;
  
  
  phost->Control.setup.b.bRequest = USB_HID_SET_IDLE;
  phost->Control.setup.b.wValue.w = (duration << 8 ) | reportId;
  
  phost->Control.setup.b.wIndex.w = 0;
  phost->Control.setup.b.wLength.w = 0;
  
  return USB_HOST_CtrlReq(pusbdev, phost, 0 , 0 );
}


/**
* @brief  USBH_Set_Report
*         Issues Set Report 
* @param  pusbdev: Selected device
* @param  reportType  : Report ept_type to be sent
* @param  reportID    : Targeted report ID for Set Report request
* @param  reportLen   : Length of data report to be send
* @param  reportBuff  : Report Buffer
* @retval USB_HOST_Status : Response for USB Set Idle request
*/
USB_HOST_Status USBH_Set_Report (USB_OTG_CORE_HANDLE *pusbdev, 
                                 USB_HOST *phost,
                                    uint8_t reportType,
                                    uint8_t reportId,
                                    uint8_t reportLen,
                                    uint8_t* reportBuff)
{
  
  phost->Control.setup.b.bmRequestType = USB_DIR_H2D | USB_REQUEST_RECIPIENT_INTERFACE |\
    USB_REQUEST_TYPE_CLASS;
  
  
  phost->Control.setup.b.bRequest = USB_HID_SET_REPORT;
  phost->Control.setup.b.wValue.w = (reportType << 8 ) | reportId;
  
  phost->Control.setup.b.wIndex.w = 0;
  phost->Control.setup.b.wLength.w = reportLen;
  
  return USB_HOST_CtrlReq(pusbdev, phost, reportBuff , reportLen );
}


/**
* @brief  USBH_Set_Protocol
*         Set protocol State.
* @param  pusbdev: Selected device
* @param  protocol : Set Protocol for HID : boot/report protocol
* @retval USB_HOST_Status : Response for USB Set Protocol request
*/
static USB_HOST_Status USBH_Set_Protocol(USB_OTG_CORE_HANDLE *pusbdev,
                                     USB_HOST *phost,
                                     uint8_t protocol)
{
  
  
  phost->Control.setup.b.bmRequestType = USB_DIR_H2D | USB_REQUEST_RECIPIENT_INTERFACE |\
    USB_REQUEST_TYPE_CLASS;
  
  
  phost->Control.setup.b.bRequest = USB_HID_SET_PROTOCOL;
  
  if(protocol != 0)
  {
    /* Boot Protocol */
    phost->Control.setup.b.wValue.w = 0;
  }
  else
  {
    /*Report Protocol*/
    phost->Control.setup.b.wValue.w = 1;
  }
  
  phost->Control.setup.b.wIndex.w = 0;
  phost->Control.setup.b.wLength.w = 0;
  
  return USB_HOST_CtrlReq(pusbdev, phost, 0 , 0 );
  
}

/**
* @brief  USBH_ParseHIDDesc 
*         This function Parse the HID descriptor
* @param  buf: Buffer where the source descriptor is available
* @retval None
*/
static void  USBH_ParseHIDDesc (USB_HOST_HIDDesc_Type *desc, uint8_t *buf)
{
  
  desc->bLength                  = *(uint8_t  *) (buf + 0);
  desc->bDescriptorType          = *(uint8_t  *) (buf + 1);
  desc->bcdHID                   =  LE16  (buf + 2);
  desc->bCountryCode             = *(uint8_t  *) (buf + 4);
  desc->bNumDescriptors          = *(uint8_t  *) (buf + 5);
  desc->bReportDescriptorType    = *(uint8_t  *) (buf + 6);
  desc->wItemLength              =  LE16  (buf + 7);
  
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


/**
* @}
*/


/**
* @}
*/

