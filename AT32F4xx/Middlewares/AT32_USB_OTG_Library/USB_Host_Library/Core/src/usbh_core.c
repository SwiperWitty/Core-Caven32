/**
  ******************************************************************************
  * File   : usbh_core.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file implements the functions for the core state machine process
  *          the enumeration and the control transfer process
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/

#include "usbh_ioreq.h"
#include "usb_bsp.h"
#include "usbh_hcs.h"
#include "usbh_stdreq.h"
#include "usbh_core.h"
#include "usb_hcd_int.h"
#include "usb_core.h"


/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_LIB_CORE
* @{
*/

/** @defgroup USBH_CORE 
  * @brief This file handles the basic enumeration when a device is connected 
  *          to the host.
  * @{
  */ 

/** @defgroup USBH_CORE_Private_TypesDefinitions
  * @{
  */ 
uint8_t USB_Host_Disconnected (USB_OTG_CORE_HANDLE *pusbdev); 
uint8_t USB_Host_Connected (USB_OTG_CORE_HANDLE *pusbdev); 
uint8_t USB_Host_SOF (USB_OTG_CORE_HANDLE *pusbdev); 
uint8_t USB_Host_PortEnabled (USB_OTG_CORE_HANDLE *pusbdev); 
uint8_t USB_Host_PortDisabled (USB_OTG_CORE_HANDLE *pusbdev); 

USB_HOST_HCD_INT_cb_Type USB_HOST_HCD_INT_cb = 
{
  USB_Host_SOF,
  USB_Host_Connected, 
  USB_Host_Disconnected,
  USB_Host_PortEnabled,
  USB_Host_PortDisabled
};

USB_HOST_HCD_INT_cb_Type  *USB_HOST_HCD_INT_fops = &USB_HOST_HCD_INT_cb;

/**
  * @}
  */ 


/** @defgroup USBH_CORE_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBH_CORE_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBH_CORE_Private_Variables
  * @{
  */ 
__IO uint32_t suspend_flag = 0;
/**
  * @}
  */ 


/** @defgroup USBH_CORE_Private_FunctionPrototypes
  * @{
  */
static USB_HOST_Status USB_HOST_HandleEnum(USB_OTG_CORE_HANDLE *pusbdev, USB_HOST *phost);
USB_HOST_Status USB_HOST_HandleControl (USB_OTG_CORE_HANDLE *pusbdev, USB_HOST *phost);

void USB_OTG_BSP_Resume(USB_OTG_CORE_HANDLE *pusbdev);                                                                 
void USB_OTG_BSP_Suspend(USB_OTG_CORE_HANDLE *pusbdev);

/**
  * @}
  */ 


/** @defgroup USBH_CORE_Private_Functions
  * @{
  */ 


/**
  * @brief  USB_Host_Connected
  *         USB Connect callback function from the Interrupt. 
  * @param  selected device
  * @retval Status
*/
uint8_t USB_Host_Connected (USB_OTG_CORE_HANDLE *pusbdev)
{
  pusbdev->host.ConnectSts = 1;
  return 0;
}

/**
  * @brief  USB_Host_PortEnabled
  *         USB Port Enable function. 
  * @param  selected device
  * @retval Status
*/
uint8_t USB_Host_PortEnabled (USB_OTG_CORE_HANDLE *pusbdev)
{
  pusbdev->host.PortEnabled = 1;
  return 0;
}

/**
  * @brief  USB_Host_PortDisabled
  *         USB Port Disable function.
  * @param  selected device
  * @retval Status
*/
uint8_t USB_Host_PortDisabled (USB_OTG_CORE_HANDLE *pusbdev)
{
  pusbdev->host.PortEnabled = 0;
  return 0;
}

/**
* @brief  USB_Host_Disconnected
*         USB Disconnect callback function from the Interrupt. 
* @param  selected device
* @retval Status
*/

uint8_t USB_Host_Disconnected (USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_BSP_DriveVBUS(pusbdev,0);
  /* Disable all interrupts. */
  USB_OTG_WRITE_R32(&pusbdev->regs.GREGS->GINTMASK, 0);
  
  /* Clear any pending interrupts. */
  USB_OTG_WRITE_R32(&pusbdev->regs.GREGS->GINTSTS, 0xFFFFFFFF);
  USB_OTG_DisableGlobalInt(pusbdev);
  pusbdev->host.ConnectSts = 0;
  return 0;  
}

/**
  * @brief  USB_Host_SOF
  *         USB SOF callback function from the Interrupt. 
  * @param  selected device
  * @retval Status
  */

uint8_t USB_Host_SOF (USB_OTG_CORE_HANDLE *pusbdev)
{
  /* This callback could be used to implement a scheduler process */
  return 0;  
}
/**
  * @brief  USB_Host_Init
  *         Host hardware and stack initializations 
  * @param  class_cb: Class callback structure address
  * @param  user_cb: User callback structure address
  * @retval None
  */
void USB_Host_Init(USB_OTG_CORE_HANDLE *pusbdev,
               USB_OTG_CORE_ID_TypeDef coreid,
               USB_HOST *phost,               
               USB_Host_Class_cb_Type *class_cb, 
               USB_Host_User_cb_Type *user_cb)
{
     
  /* Hardware Init */
  USB_OTG_BSP_Init(pusbdev);  
  
  /* configure GPIO pin used for switching VBUS power */
  USB_OTG_BSP_ConfigVBUS(0);  
  
  
  /* Host de-initializations */
  USB_Host_Reset(pusbdev, phost);
  
  /*Register class and user callbacks */
  phost->class_cb = class_cb;
  phost->user_cb = user_cb;  
    
  /* Start the USB OTG core */     
  USB_HCD_Init(pusbdev , coreid);
   
  /* Upon Init call usr call back */
  phost->user_cb->Init();
  
  /* Enable Interrupts */
  USB_OTG_BSP_EnableInterrupt(pusbdev);
}

/**
  * @brief  USB_Host_Reset 
  *         Re-Initialize Host
  * @param  None 
  * @retval status: USB_HOST_Status
  */
USB_HOST_Status USB_Host_Reset(USB_OTG_CORE_HANDLE *pusbdev, USB_HOST *phost)
{
  /* Software Init */
  
  phost->gState = USB_HOST_IDLE;
  phost->gStateBkp = USB_HOST_IDLE; 
  phost->EnumState = USB_ENUM_IDLE;
  phost->RequestState = CMD_SEND;  
  
  phost->Control.state = USB_CTRL_SETUP;
  phost->Control.ept0size = USB_OTG_MAX_EPT0_SIZE;  
  
  phost->device_prop.address = USB_HOST_DEVICE_ADDRESS_DEFAULT;
  phost->device_prop.speed = HPRT0_PRTSPD_FULL_SPEED;
  
  USB_Host_Free_Channel  (pusbdev, phost->Control.hch_num_in);
  USB_Host_Free_Channel  (pusbdev, phost->Control.hch_num_out);  
  return USB_HOST_OK;
}

/**
* @brief  USB_Host_Process
*         USB Host core main state machine process
* @param  None 
* @retval None
*/
void USB_Host_Process(USB_OTG_CORE_HANDLE *pusbdev , USB_HOST *phost)
{
  volatile USB_HOST_Status status = USB_HOST_FAIL;

  /* check for Host port events */
  if (((USB_HCD_IsDeviceConnected(pusbdev) == 0)|| (USB_HCD_IsPortEnabled(pusbdev) == 0))&& (phost->gState != USB_HOST_IDLE)) 
  {
    if(phost->gState != USB_HOST_DEV_DISCONNECTED) 
    {
      phost->gState = USB_HOST_DEV_DISCONNECTED;
    }
  }
    
  switch (phost->gState)
  {
  
  case USB_HOST_IDLE :
    
    if (USB_HCD_IsDeviceConnected(pusbdev))  
    {
      phost->gState = USB_HOST_WAIT_PRT_ENABLED; 
      
      /*wait denounce delay */
      USB_OTG_BSP_mDelay(100);
      
      /* Apply a port RESET */
      USB_HCD_ResetPort(pusbdev);
      
      /* User RESET callback*/
      phost->user_cb->ResetDevice();
    }
    break;
    
  case USB_HOST_WAIT_PRT_ENABLED:
    if (pusbdev->host.PortEnabled == 1)
    { 
      phost->gState = USB_HOST_DEV_ATTACHED; 
      USB_OTG_BSP_mDelay(50);
    }
    break;
      
  case USB_HOST_DEV_ATTACHED :
    
    phost->user_cb->DeviceAttached();
    phost->Control.hch_num_out = USB_Host_Alloc_Channel(pusbdev, 0x00);
    phost->Control.hch_num_in = USB_Host_Alloc_Channel(pusbdev, 0x80);  
  
    /* Reset USB Device */
    if ( USB_HCD_ResetPort(pusbdev) == 0)
    {
      phost->user_cb->ResetDevice();
     
      /* Host is Now ready to start the Enumeration */
      phost->device_prop.speed = USB_HCD_GetCurrentSpeed(pusbdev);
      
      phost->gState = USB_HOST_ENUMERATION;
      phost->user_cb->DeviceSpeedDetected(phost->device_prop.speed);
      
      /* Open Control pipes */
      USB_Host_Open_Channel (pusbdev,
                         phost->Control.hch_num_in,
                         phost->device_prop.address,
                         phost->device_prop.speed,
                         EPT_TYPE_CTRL,
                         phost->Control.ept0size); 
      
      /* Open Control pipes */
      USB_Host_Open_Channel (pusbdev,
                         phost->Control.hch_num_out,
                         phost->device_prop.address,
                         phost->device_prop.speed,
                         EPT_TYPE_CTRL,
                         phost->Control.ept0size);          
    }
    break;
    
  case USB_HOST_ENUMERATION:     
    /* Check for enumeration status */  
    if ( USB_HOST_HandleEnum(pusbdev , phost) == USB_HOST_OK)
    { 
      /* The function shall return USB_HOST_OK when full enumeration is complete */
      
      /* user callback for end of device basic enumeration */
      phost->user_cb->EnumerationDone();
#if defined (USB_OTG_FS_LOW_PWR_MGMT_SUPPORT)
      phost->gState  = USB_HOST_SUSPENDED;
#else
      phost->gState  = USB_HOST_USR_INPUT;
#endif /* LOW_PWR_MGMT_SUPPORT*/
    }
    break;
    
  case USB_HOST_USR_INPUT:    
    /*The function should return user response true to move to class state */
    if ( phost->user_cb->UserInput() == USB_Host_USR_RESP_OK)
    {
      if((phost->class_cb->Init(pusbdev, phost))\
        == USB_HOST_OK)
      {
        phost->gState  = USB_HOST_CLASS_REQUEST;     
      }     
    }   
    break;
    
  case USB_HOST_CLASS_REQUEST:  
    /* process class standard control requests state machine */ 
    status = phost->class_cb->Requests(pusbdev, phost);
    
     if(status == USB_HOST_OK)
     {
       phost->gState  = USB_HOST_CLASS;
     }  
     
     else
     {
       USB_Host_ErrorHandle(phost, status);
     }
 
    
    break;    
  case USB_HOST_CLASS:   
    /* process class state machine */
    status = phost->class_cb->Machine(pusbdev, phost);
    USB_Host_ErrorHandle(phost, status);
    break;       
    
  case USB_HOST_CTRL_XFER:
    /* process control transfer state machine */
    USB_HOST_HandleControl(pusbdev, phost);    
    break;
#if defined (USB_OTG_FS_LOW_PWR_MGMT_SUPPORT)
  case USB_HOST_SUSPENDED:
    if (USB_HOST_SetDeviceFeature(pusbdev, phost, USB_FEATURE_SELECTOR_DEVICE, 0)==USB_HOST_OK)
    {
      suspend_flag = 1;
      USB_OTG_BSP_Suspend(pusbdev);
      phost->user_cb->UserInput();
      PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
      /* After wakeup got to USB_HOST_WAKEUP state */
      phost->gState  = USB_HOST_WAKEUP;
    }
    break;
      
  case USB_HOST_WAKEUP:
    /* issue  a ClearDeviceFeature request */
    if (USB_HOST_ClearDeviceFeature(pusbdev, phost, USB_FEATURE_SELECTOR_DEVICE, 0)==USB_HOST_OK)
    {
      phost->gState  = USB_HOST_USR_INPUT;
    }
    break;
#endif /* USE_HOST_MODE */
  case USB_HOST_ERROR_STATE:
    /* Re-Initialize Host for new Enumeration */
    USB_Host_Reset(pusbdev, phost);
    phost->user_cb->Reset();
    phost->class_cb->Reset(pusbdev, &phost->device_prop);
    break;
    
  case USB_HOST_DEV_DISCONNECTED :
    
    /* Manage User disconint operations*/
    phost->user_cb->DeviceDisconnected();
    
    /* Re-Initialize Host for new Enumeration */
    USB_Host_Reset(pusbdev, phost);
    phost->user_cb->Reset();
    phost->class_cb->Reset(pusbdev, &phost->device_prop); 
    USB_Host_DeAllocate_AllChannel(pusbdev);  
    phost->gState = USB_HOST_IDLE;
   
    /* Re-Initialize Host for new Enumeration */
    USB_HCD_Init(pusbdev, 
#ifdef USE_USB_OTG_FS
            USB_OTG_FS_CORE_ID
#else
            USB_OTG_HS_CORE_ID
#endif
);
    
    break;
    
  default :
    break;
  }

}


/**
  * @brief  USB_Host_ErrorHandle 
  *         This function handles the Error on Host side.
  * @param  errType : Type of Error or Busy/OK state
  * @retval None
  */
void USB_Host_ErrorHandle(USB_HOST *phost, USB_HOST_Status errType)
{
  /* Error unrecovered or not supported device speed */
  if ( (errType == USB_HOST_ERROR_SPEED_UNKNOWN) ||
       (errType == USB_HOST_UNRECOVERED_ERROR) )
  {
    phost->user_cb->UnrecoveredError(); 
    phost->gState = USB_HOST_ERROR_STATE;   
  }  
  /* USB host restart requested from application layer */
  else if(errType == USB_HOST_APPLY_DEINIT)
  {
    phost->gState = USB_HOST_ERROR_STATE;  
    /* user callback for initialization */
    phost->user_cb->Init();
  } 
}


/**
  * @brief  USB_HOST_HandleEnum 
  *         This function includes the complete enumeration process
  * @param  pusbdev: Selected device
  * @retval USB_HOST_Status
  */
static USB_HOST_Status USB_HOST_HandleEnum(USB_OTG_CORE_HANDLE *pusbdev, USB_HOST *phost)
{
  USB_HOST_Status Status = USB_HOST_BUSY;  
  uint8_t Local_Buffer[64];
  
  switch (phost->EnumState)
  {
  case USB_ENUM_IDLE:  
    /* Get Device Desc for only 1st 8 bytes : To get EP0 MaxPacketSize */
    if ( USB_HOST_Get_DevDesc(pusbdev , phost, 8) == USB_HOST_OK)
    {
      phost->Control.ept0size = phost->device_prop.Dev_Desc.bMaxPacketSize;
      
      phost->EnumState = USB_ENUM_GET_FULL_DEV_DESC;
      
      /* modify control channels configuration for MaxPacket size */
      USB_Host_Modify_Channel (pusbdev,
                           phost->Control.hch_num_out,
                           0,
                           0,
                           0,
                           phost->Control.ept0size);
      
      USB_Host_Modify_Channel (pusbdev,
                           phost->Control.hch_num_in,
                           0,
                           0,
                           0,
                           phost->Control.ept0size);      
    }
    break;
    
  case USB_ENUM_GET_FULL_DEV_DESC:  
    /* Get FULL Device Desc  */
    if ( USB_HOST_Get_DevDesc(pusbdev, phost, USB_DEVICE_DESC_SIZE)\
      == USB_HOST_OK)
    {
      /* user callback for device descriptor available */
      phost->user_cb->DeviceDescAvailable(&phost->device_prop.Dev_Desc);      
      phost->EnumState = USB_ENUM_SET_ADDR;
    }
    break;
   
  case USB_ENUM_SET_ADDR: 
    /* set address */
    if ( USB_HOST_SetAddress(pusbdev, phost, USB_HOST_DEVICE_ADDRESS) == USB_HOST_OK)
    {
      USB_OTG_BSP_mDelay(2);
      phost->device_prop.address = USB_HOST_DEVICE_ADDRESS;
      
      /* user callback for device address assigned */
      phost->user_cb->DeviceAddressAssigned();
      phost->EnumState = USB_ENUM_GET_CFG_DESC;
      
      /* modify control channels to update device address */
      USB_Host_Modify_Channel (pusbdev,
                           phost->Control.hch_num_in,
                           phost->device_prop.address,
                           0,
                           0,
                           0);
      
      USB_Host_Modify_Channel (pusbdev,
                           phost->Control.hch_num_out,
                           phost->device_prop.address,
                           0,
                           0,
                           0);         
    }
    break;
    
  case USB_ENUM_GET_CFG_DESC:  
    /* get standard configuration descriptor */
    if ( USB_HOST_Get_CfgDesc(pusbdev, 
                          phost,
                          USB_CONFIGURATION_DESC_SIZE) == USB_HOST_OK)
    {
      /* before getting full config descriptor, check if it does not exceed 
      buffer size allocated to config descriptor USBH_MAX_DATA_BUFFER
      in the usbh_conf.h*/
      if (phost->device_prop.Cfg_Desc.wTotalLength <= USBH_MAX_DATA_BUFFER)
      {
        phost->EnumState = USB_ENUM_GET_FULL_CFG_DESC;
      }
    }
    break;
    
  case USB_ENUM_GET_FULL_CFG_DESC:  
    /* get FULL config descriptor (config, interface, endpoints) */
    if (USB_HOST_Get_CfgDesc(pusbdev, 
                         phost,
                         phost->device_prop.Cfg_Desc.wTotalLength) == USB_HOST_OK)
    {
      /* User callback for configuration descriptors available */
      phost->user_cb->ConfigurationDescAvailable(&phost->device_prop.Cfg_Desc,
                                                      phost->device_prop.Itf_Desc,
                                                      phost->device_prop.Ept_Desc[0]);
      
      phost->EnumState = USB_ENUM_GET_MFC_STRING_DESC;
    }
    break;
    
  case USB_ENUM_GET_MFC_STRING_DESC:  
    if (phost->device_prop.Dev_Desc.iManufacturer != 0)
    { /* Check that Manufacturer String is available */
      
      if ( USB_HOST_Get_StringDesc(pusbdev,
                               phost,
                               phost->device_prop.Dev_Desc.iManufacturer, 
                               Local_Buffer , 
                               0xff) == USB_HOST_OK)
      {
        /* User callback for Manufacturing string */
        phost->user_cb->ManufacturerString(Local_Buffer);
        phost->EnumState = USB_ENUM_GET_PRODUCT_STRING_DESC;
      }
    }
    else
    {
      phost->user_cb->ManufacturerString("N/A");      
      phost->EnumState = USB_ENUM_GET_PRODUCT_STRING_DESC;
    }
    break;
    
  case USB_ENUM_GET_PRODUCT_STRING_DESC:   
    if (phost->device_prop.Dev_Desc.iProduct != 0)
    { /* Check that Product string is available */
      if ( USB_HOST_Get_StringDesc(pusbdev,
                               phost,
                               phost->device_prop.Dev_Desc.iProduct, 
                               Local_Buffer, 
                               0xff) == USB_HOST_OK)
      {
        /* User callback for Product string */
        phost->user_cb->ProductString(Local_Buffer);
        phost->EnumState = USB_ENUM_GET_SERIALNUM_STRING_DESC;
      }
    }
    else
    {
      phost->user_cb->ProductString("N/A");
      phost->EnumState = USB_ENUM_GET_SERIALNUM_STRING_DESC;
    } 
    break;
    
  case USB_ENUM_GET_SERIALNUM_STRING_DESC:   
    if (phost->device_prop.Dev_Desc.iSerialNumber != 0)
    { /* Check that Serial number string is available */    
      if ( USB_HOST_Get_StringDesc(pusbdev, 
                               phost,
                               phost->device_prop.Dev_Desc.iSerialNumber, 
                               Local_Buffer, 
                               0xff) == USB_HOST_OK)
      {
        /* User callback for Serial number string */
        phost->user_cb->SerialNumString(Local_Buffer);
        phost->EnumState = USB_ENUM_SET_CONFIGURATION;
      }
    }
    else
    {
      phost->user_cb->SerialNumString("N/A");      
      phost->EnumState = USB_ENUM_SET_CONFIGURATION;
    }  
    break;
      
  case USB_ENUM_SET_CONFIGURATION:
    /* set configuration  (default config) */
    if (USB_HOST_SetCfg(pusbdev, 
                    phost,
                    phost->device_prop.Cfg_Desc.bConfigurationValue) == USB_HOST_OK)
    {
      phost->EnumState = USB_ENUM_DEV_CONFIGURED;
    }
    break;

    
  case USB_ENUM_DEV_CONFIGURED:
    /* user callback for enumeration done */
    Status = USB_HOST_OK;
    break;
    
  default:
    break;
  }  
  return Status;
}


/**
  * @brief  USB_HOST_HandleControl
  *         Handles the USB control transfer state machine
  * @param  pusbdev: Selected device
  * @retval Status
  */
USB_HOST_Status USB_HOST_HandleControl (USB_OTG_CORE_HANDLE *pusbdev, USB_HOST *phost)
{
  uint8_t direction;  
  static uint16_t timeout = 0;
  USB_HOST_Status status = USB_HOST_OK;
  URB_STATE URB_Status = URB_IDLE;
  
  phost->Control.status = CTRL_START;

  
  switch (phost->Control.state)
  {
  case USB_CTRL_SETUP:
    /* send a SETUP packet */
    USB_HOST_CtrlSendSetup     (pusbdev, 
	                   phost->Control.setup.d8 , 
	                   phost->Control.hch_num_out);  
    phost->Control.state = USB_CTRL_SETUP_WAIT;  
    break; 
    
  case USB_CTRL_SETUP_WAIT:
    
    URB_Status = USB_HCD_GetURB_State(pusbdev , phost->Control.hch_num_out); 
    /* case SETUP packet sent successfully */
    if(URB_Status == URB_DONE)
    { 
      direction = (phost->Control.setup.b.bmRequestType & USB_REQUEST_DIR_MASK);
      
      /* check if there is a data stage */
      if (phost->Control.setup.b.wLength.w != 0 )
      {        
        timeout = DATA_STAGE_TIMEOUT;
        if (direction == USB_DIR_D2H)
        {
          /* Data Direction is IN */
          phost->Control.state = USB_CTRL_DATA_IN;
        }
        else
        {
          /* Data Direction is OUT */
          phost->Control.state = USB_CTRL_DATA_OUT;
        } 
      }
      /* No DATA stage */
      else
      {
        timeout = NODATA_STAGE_TIMEOUT;
        
        /* If there is No Data Transfer Stage */
        if (direction == USB_DIR_D2H)
        {
          /* Data Direction is IN */
          phost->Control.state = USB_CTRL_STATUS_OUT;
        }
        else
        {
          /* Data Direction is OUT */
          phost->Control.state = USB_CTRL_STATUS_IN;
        } 
      }          
      /* Set the delay timer to enable timeout for data stage completion */
      phost->Control.timer = USB_HCD_GetCurrentFrame(pusbdev);
    }
    else if(URB_Status == URB_ERROR)
    {
      phost->Control.state = USB_CTRL_ERROR;     
      phost->Control.status = CTRL_XACTERR;
    }    
    break;
    
  case USB_CTRL_DATA_IN:  
    /* Issue an IN token */ 
    USB_HOST_CtrlReceiveData(pusbdev,
                        phost->Control.buff, 
                        phost->Control.length,
                        phost->Control.hch_num_in);
 
    phost->Control.state = USB_CTRL_DATA_IN_WAIT;
    break;    
    
  case USB_CTRL_DATA_IN_WAIT:
    
    URB_Status = USB_HCD_GetURB_State(pusbdev , phost->Control.hch_num_in); 
    
    /* check is DATA packet transferred successfully */
    if  (URB_Status == URB_DONE)
    { 
      phost->Control.state = USB_CTRL_STATUS_OUT;
    }
   
    /* manage error cases*/
    if  (URB_Status == URB_STALL) 
    { 
      /* In stall case, return to previous machine state*/
      phost->gState =   phost->gStateBkp;
      phost->Control.state = USB_CTRL_STALLED;  
    }   
    else if (URB_Status == URB_ERROR)
    {
      /* Device error */
      phost->Control.state = USB_CTRL_ERROR;    
    }
    else if ((USB_HCD_GetCurrentFrame(pusbdev)- phost->Control.timer) > timeout)
    {
      /* timeout for IN transfer */
      phost->Control.state = USB_CTRL_ERROR; 
    }   
    break;
    
  case USB_CTRL_DATA_OUT:
    /* Start DATA out transfer (only one DATA packet)*/
    pusbdev->host.hc[phost->Control.hch_num_out].toggle_out = 1; 
        
    USB_HOST_CtrlSendData (pusbdev,
                      phost->Control.buff, 
                      phost->Control.length , 
                      phost->Control.hch_num_out);
    
    phost->Control.state = USB_CTRL_DATA_OUT_WAIT;
    break;
    
  case USB_CTRL_DATA_OUT_WAIT:
    
    URB_Status = USB_HCD_GetURB_State(pusbdev , phost->Control.hch_num_out);     
    if  (URB_Status == URB_DONE)
    { /* If the Setup Pkt is sent successful, then change the state */
      phost->Control.state = USB_CTRL_STATUS_IN;
    }
    
    /* handle error cases */
    else if  (URB_Status == URB_STALL) 
    { 
      /* In stall case, return to previous machine state*/
      phost->gState =   phost->gStateBkp;
      phost->Control.state = USB_CTRL_STALLED;  
    } 
    else if  (URB_Status == URB_NOTREADY)
    { 
      /* Nack received from device */
      phost->Control.state = USB_CTRL_DATA_OUT;
    }    
    else if (URB_Status == URB_ERROR)
    {
      /* device error */
      phost->Control.state = USB_CTRL_ERROR;      
    } 
    break;
    
    
  case USB_CTRL_STATUS_IN:
    /* Send 0 bytes out packet */
    USB_HOST_CtrlReceiveData (pusbdev,
                         0,
                         0,
                         phost->Control.hch_num_in);
    
    phost->Control.state = USB_CTRL_STATUS_IN_WAIT;
    
    break;
    
  case USB_CTRL_STATUS_IN_WAIT:
    
    URB_Status = USB_HCD_GetURB_State(pusbdev , phost->Control.hch_num_in); 
    
    if  ( URB_Status == URB_DONE)
    { /* Control transfers completed, Exit the State Machine */
      phost->gState =   phost->gStateBkp;
      phost->Control.state = USB_CTRL_COMPLETE;
    }
    
    else if (URB_Status == URB_ERROR)
    {
      phost->Control.state = USB_CTRL_ERROR;  
    }
    
    else if((USB_HCD_GetCurrentFrame(pusbdev)\
      - phost->Control.timer) > timeout)
    {
      phost->Control.state = USB_CTRL_ERROR; 
    }
     else if(URB_Status == URB_STALL)
    {
      /* Control transfers completed, Exit the State Machine */
      phost->gState =   phost->gStateBkp;
      phost->Control.state = USB_CTRL_STALLED;
      status = USB_HOST_NOT_SUPPORTED;
    }
    break;
    
  case USB_CTRL_STATUS_OUT:
    pusbdev->host.hc[phost->Control.hch_num_out].toggle_out ^= 1; 
    USB_HOST_CtrlSendData (pusbdev,
                      0,
                      0,
                      phost->Control.hch_num_out);
    
    phost->Control.state = USB_CTRL_STATUS_OUT_WAIT;
    break;
    
  case USB_CTRL_STATUS_OUT_WAIT: 
    
    URB_Status = USB_HCD_GetURB_State(pusbdev , phost->Control.hch_num_out);  
    if  (URB_Status == URB_DONE)
    { 
      phost->gState =   phost->gStateBkp; 
      phost->Control.state = USB_CTRL_COMPLETE; 
    }
    else if  (URB_Status == URB_NOTREADY)
    { 
      phost->Control.state = USB_CTRL_STATUS_OUT;
    }      
    else if (URB_Status == URB_ERROR)
    {
      phost->Control.state = USB_CTRL_ERROR;      
    }
    break;
    
  case USB_CTRL_ERROR:
    /* 
    After a halt condition is encountered or an error is detected by the 
    host, a control endpoint is allowed to recover by accepting the next Setup 
    PID; i.e., recovery actions via some other pipe are not required for control
    endpoints. For the Default Control Pipe, a device reset will ultimately be 
    required to clear the halt or error condition if the next Setup PID is not 
    accepted.
    */
    if (++ phost->Control.errorcount <= USB_HOST_MAX_ERROR_COUNT)
    {
      /* Do the transmission again, starting from SETUP Packet */
      phost->Control.state = USB_CTRL_SETUP; 
    }
    else
    {
      phost->Control.status = CTRL_FAIL;
      phost->gState =   phost->gStateBkp;
      
      status = USB_HOST_FAIL;
    }
    break;
    
  default:
    break;
  }
  return status;
}

/**
  * @brief  USB_OTG_BSP_Resume
  *         Handles the USB Resume from Suspend Mode
  * @param  pusbdev: Selected device
  * @retval Status
  */
void USB_OTG_BSP_Resume(USB_OTG_CORE_HANDLE *pusbdev)                                                                 
{
  USB_OTG_HPRT0_Type    hprt0;
  USB_OTG_PCGCCTL_Type  power;
  hprt0.u32val  = 0;
  /*  switch-off the clocks */
  power.u32val = 0;
  power.b.stoppclk = 1;
  USB_OTG_MODIFY_R32(pusbdev->regs.PCGCCTL,power.u32val, 0 );  
  
  power.b.gatehclk = 1;
  USB_OTG_MODIFY_R32(pusbdev->regs.PCGCCTL,power.u32val, 0);
  
  hprt0.u32val  = 0;   
  hprt0.u32val  = USB_OTG_ReadHPRT0(pusbdev);
  hprt0.b.prtsusp = 0;
  hprt0.b.prtres = 1;
  USB_OTG_WRITE_R32(pusbdev->regs.HPRT0, hprt0.u32val);
  USB_OTG_BSP_mDelay (20);
  hprt0.b.prtres = 0;
  USB_OTG_WRITE_R32(pusbdev->regs.HPRT0, hprt0.u32val); 
}

/**
  * @brief  USB_OTG_BSP_Suspend
  *         Handles the Enter USB to Suspend Mode
  * @param  pusbdev: Selected device
  * @retval Status
  */
void USB_OTG_BSP_Suspend(USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_HPRT0_Type    hprt0;
  USB_OTG_PCGCCTL_Type  power;
  hprt0.u32val  = 0;
  hprt0.u32val  = USB_OTG_ReadHPRT0(pusbdev);
  hprt0.b.prtsusp = 1; 
  USB_OTG_WRITE_R32(pusbdev->regs.HPRT0, hprt0.u32val);
  
  /*  switch-off the clocks */
  power.u32val = 0;
  power.b.stoppclk = 1;
  USB_OTG_MODIFY_R32(pusbdev->regs.PCGCCTL, 0, power.u32val);  
  
  power.b.gatehclk = 1;
  USB_OTG_MODIFY_R32(pusbdev->regs.PCGCCTL, 0, power.u32val);
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

