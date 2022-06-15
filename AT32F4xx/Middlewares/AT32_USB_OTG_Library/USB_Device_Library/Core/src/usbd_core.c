/**
  ******************************************************************************
  * File   : usbd_core.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file provides all the USB device core functions.
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "usbd_core.h"
#include "usbd_req.h"
#include "usbd_ioreq.h"
#include "usb_dcd_int.h"
#include "usb_bsp.h"

/** @addtogroup AT32_USB_OTG_DEVICE_LIBRARY
* @{
*/


/** @defgroup USBD_CORE 
* @brief usbd core module
* @{
*/ 

/** @defgroup USBD_CORE_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBD_CORE_Private_Defines
* @{
*/ 

/**
* @}
*/ 


/** @defgroup USBD_CORE_Private_Macros
* @{
*/ 
/**
* @}
*/ 




/** @defgroup USBD_CORE_Private_FunctionPrototypes
* @{
*/ 
static uint8_t USB_Device_SetupStage(USB_OTG_CORE_HANDLE *pusbdev);
static uint8_t USB_Device_DataOutStage(USB_OTG_CORE_HANDLE *pusbdev , uint8_t bepnum);
static uint8_t USB_Device_DataInStage(USB_OTG_CORE_HANDLE *pusbdev , uint8_t bepnum);
static uint8_t USB_Device_SOF(USB_OTG_CORE_HANDLE  *pusbdev);
static uint8_t USB_Device_Rst(USB_OTG_CORE_HANDLE  *pusbdev);
static uint8_t USB_Device_Suspend(USB_OTG_CORE_HANDLE  *pusbdev);
static uint8_t USB_Device_Resume(USB_OTG_CORE_HANDLE  *pusbdev);
#ifdef VBUS_SENSING_ENABLED
static uint8_t USB_Device_DevConnected(USB_OTG_CORE_HANDLE  *pusbdev);
static uint8_t USB_Device_DevDisconnected(USB_OTG_CORE_HANDLE  *pusbdev);
#endif
static uint8_t USB_Device_IsoINIncomplete(USB_OTG_CORE_HANDLE  *pusbdev);
static uint8_t USB_Device_IsoOUTIncomplete(USB_OTG_CORE_HANDLE  *pusbdev);
static uint8_t USB_Device_RunTestMode (USB_OTG_CORE_HANDLE  *pusbdev) ;
/**
* @}
*/ 

/** @defgroup USBD_CORE_Private_Variables
* @{
*/ 

__IO USB_OTG_DCTL_Type SET_TEST_MODE;

USB_DEVICE_DCD_INT_cb_Type USB_DEVICE_DCD_INT_cb = 
{
  USB_Device_DataOutStage,
  USB_Device_DataInStage,
  USB_Device_SetupStage,
  USB_Device_SOF,
  USB_Device_Rst,
  USB_Device_Suspend,
  USB_Device_Resume,
  USB_Device_IsoINIncomplete,
  USB_Device_IsoOUTIncomplete,
#ifdef VBUS_SENSING_ENABLED
  USB_Device_DevConnected, 
  USB_Device_DevDisconnected,    
#endif  
};

USB_DEVICE_DCD_INT_cb_Type  *USB_DEVICE_DCD_INT_fops = &USB_DEVICE_DCD_INT_cb;
/**
* @}
*/ 

/** @defgroup USBD_CORE_Private_Functions
* @{
*/ 

/**
* @brief  USB_Device_Init
*         Initializes the device stack and load the class driver
* @param  pusbdev: device instance
* @param  core_address: USB OTG core ID
* @param  class_cb: Class callback structure address
* @param  user_cb: User callback structure address
* @retval None
*/
void USB_Device_Init(USB_OTG_CORE_HANDLE *pusbdev,
               USB_OTG_CORE_ID_TypeDef coreid,
               USB_DEVICE_USER *pDevice,                  
               USBD_Class_cb_Type *class_cb, 
               USB_Device_User_cb_Type *user_cb)
{
  USB_OTG_BSP_Init(pusbdev);
  USB_Device_Reset(pusbdev);
  
  /*Register class and user callbacks */
  pusbdev->dev.class_cb = class_cb;
  pusbdev->dev.user_cb = user_cb;  
  pusbdev->dev.usr_device = pDevice;    
  
  /* set USB OTG core params */
  USB_DCD_Init(pusbdev , coreid);
  
  /* Upon Init call usr callback */
  pusbdev->dev.user_cb->Init();
}

/**
* @brief  USB_Device_Reset 
*         Re-Initialize the device library
* @param  pusbdev: device instance
* @retval status: status
*/
USB_Device_Status USB_Device_Reset(USB_OTG_CORE_HANDLE *pusbdev)
{
  /* Software Init */
  
  return USB_DEVICE_OK;
}

/**
* @brief  USB_Device_SetupStage 
*         Handle the setup stage
* @param  pusbdev: device instance
* @retval status
*/
static uint8_t USB_Device_SetupStage(USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_SETUP_REQEST req;
  
  USB_Device_ParseSetupRequest(pusbdev , &req);
  
  switch (req.bmRequest & 0x1F) 
  {
  case USB_REQUEST_RECIPIENT_DEVICE:   
    USB_Device_StdDevReq (pusbdev, &req);
    break;
    
  case USB_REQUEST_RECIPIENT_INTERFACE:     
    USB_Device_StdItfReq(pusbdev, &req);
    break;
    
  case USB_REQUEST_RECIPIENT_ENDPOINT:        
    USB_Device_StdEPReq(pusbdev, &req);   
    break;
    
  default:           
    USB_DCD_EPT_Stall(pusbdev , req.bmRequest & 0x80);
    break;
  }  
  return USB_DEVICE_OK;
}

/**
* @brief  USB_Device_DataOutStage 
*         Handle data out stage
* @param  pusbdev: device instance
* @param  bepnum: endpoint index
* @retval status
*/
static uint8_t USB_Device_DataOutStage(USB_OTG_CORE_HANDLE *pusbdev , uint8_t bepnum)
{
  USB_OTG_EPT *ept;
  
  if(bepnum == 0) 
  {
    ept = &pusbdev->dev.out_ept[0];
    if ( pusbdev->dev.device_state == USB_OTG_EPT0_DATAOUT)
    {
      if(ept->rem_data_len > ept->maxpacket)
      {
        ept->rem_data_len -=  ept->maxpacket;
              
        USB_Device_CtrlContinueRx (pusbdev, 
                            ept->xfer_buff,
                            MIN(ept->rem_data_len ,ept->maxpacket));
      }
      else
      {
        if((pusbdev->dev.class_cb->EPT0_RxReady != NULL)&&
           (pusbdev->dev.device_status == USB_OTG_CONFIGURED))
        {
          pusbdev->dev.class_cb->EPT0_RxReady(pusbdev); 
        }
        USB_Device_CtrlSendStatus(pusbdev);
      }
    }
  }
  else if((pusbdev->dev.class_cb->Data_Out != NULL)&&
          (pusbdev->dev.device_status == USB_OTG_CONFIGURED))
  {
    pusbdev->dev.class_cb->Data_Out(pusbdev, bepnum); 
  } 
  
  else
  {
    /* Do Nothing */
  }
  return USB_DEVICE_OK;
}

/**
* @brief  USB_Device_DataInStage 
*         Handle data in stage
* @param  pusbdev: device instance
* @param  bepnum: endpoint index
* @retval status
*/
static uint8_t USB_Device_DataInStage(USB_OTG_CORE_HANDLE *pusbdev , uint8_t bepnum)
{
  USB_OTG_EPT *ept;
  
  if(bepnum == 0) 
  {
    ept = &pusbdev->dev.in_ept[0];
    if ( pusbdev->dev.device_state == USB_OTG_EPT0_DATAIN)
    {
      if(ept->rem_data_len > ept->maxpacket)
      {
        ept->rem_data_len -=  ept->maxpacket;
        USB_Device_CtrlContinueSendData (pusbdev, 
                                  ept->xfer_buff, 
                                  ept->rem_data_len);
        
        /* Start the transfer */  
        USB_DCD_EPT_PrepareRx (pusbdev,
                          0,
                          NULL,
                          0);
      }
      else
      { /* last packet is MPS multiple, so send ZLP packet */
        if((ept->total_data_len % ept->maxpacket == 0) &&
           (ept->total_data_len >= ept->maxpacket) &&
             (ept->total_data_len < ept->ctl_data_len ))
        {
          
          USB_Device_CtrlContinueSendData(pusbdev , NULL, 0);
          ept->ctl_data_len = 0;
          
          /* Start the transfer */  
          USB_DCD_EPT_PrepareRx (pusbdev,
                            0,
                            NULL,
                            0);
        }
        else
        {
          if((pusbdev->dev.class_cb->EPT0_TxSent != NULL)&&
             (pusbdev->dev.device_status == USB_OTG_CONFIGURED))
          {
            pusbdev->dev.class_cb->EPT0_TxSent(pusbdev); 
          }          
          USB_Device_CtrlReceiveStatus(pusbdev);
        }
      }
    }
    if (pusbdev->dev.test_mode == 1)
    {
      USB_Device_RunTestMode(pusbdev); 
      pusbdev->dev.test_mode = 0;
    }
  }
  else if((pusbdev->dev.class_cb->Data_In != NULL)&& 
          (pusbdev->dev.device_status == USB_OTG_CONFIGURED))
  {
    pusbdev->dev.class_cb->Data_In(pusbdev, bepnum); 
  } 
  
  else
  {
    /* Do Nothing */
  }
  return USB_DEVICE_OK;
}




/**
* @brief  USB_Device_RunTestMode 
*         Launch test mode process
* @param  pusbdev: device instance
* @retval status
*/
static uint8_t  USB_Device_RunTestMode (USB_OTG_CORE_HANDLE  *pusbdev) 
{
  USB_OTG_WRITE_R32(&pusbdev->regs.DREGS->DCTRL, SET_TEST_MODE.u32val);
  return USB_DEVICE_OK;  
}

/**
* @brief  USB_Device_Reset 
*         Handle Reset event
* @param  pusbdev: device instance
* @retval status
*/

static uint8_t USB_Device_Rst(USB_OTG_CORE_HANDLE  *pusbdev)
{
  /* Open EP0 OUT */
  USB_DCD_EPT_Open(pusbdev,
              0x00,
              USB_OTG_MAX_EPT0_SIZE,
              EPT_TYPE_CTRL);
  
  /* Open EP0 IN */
  USB_DCD_EPT_Open(pusbdev,
              0x80,
              USB_OTG_MAX_EPT0_SIZE,
              EPT_TYPE_CTRL);
  
  /* Upon Reset call usr call back */
  pusbdev->dev.device_status = USB_OTG_DEFAULT;
  pusbdev->dev.user_cb->DeviceReset(pusbdev->cfg.speed);
  
  return USB_DEVICE_OK;
}

/**
* @brief  USB_Device_Resume 
*         Handle Resume event
* @param  pusbdev: device instance
* @retval status
*/

static uint8_t USB_Device_Resume(USB_OTG_CORE_HANDLE  *pusbdev)
{
  /* Upon Resume call usr call back */
  pusbdev->dev.user_cb->DeviceResumed(); 
  pusbdev->dev.device_status = pusbdev->dev.device_old_status;  
  pusbdev->dev.device_status = USB_OTG_CONFIGURED;  
  return USB_DEVICE_OK;
}


/**
* @brief  USB_Device_Suspend 
*         Handle Suspend event
* @param  pusbdev: device instance
* @retval status
*/

static uint8_t USB_Device_Suspend(USB_OTG_CORE_HANDLE  *pusbdev)
{
  pusbdev->dev.device_old_status = pusbdev->dev.device_status;
  pusbdev->dev.device_status  = USB_OTG_SUSPENDED;
  /* Upon Resume call usr call back */
  pusbdev->dev.user_cb->DeviceSuspended(); 
  return USB_DEVICE_OK;
}


/**
* @brief  USB_Device_SOF 
*         Handle SOF event
* @param  pusbdev: device instance
* @retval status
*/

static uint8_t USB_Device_SOF(USB_OTG_CORE_HANDLE  *pusbdev)
{
  if(pusbdev->dev.class_cb->SOF)
  {
    pusbdev->dev.class_cb->SOF(pusbdev); 
  }
  return USB_DEVICE_OK;
}
/**
* @brief  USB_Device_SetCfg 
*        Configure device and start the interface
* @param  pusbdev: device instance
* @param  cfgidx: configuration index
* @retval status
*/

USB_Device_Status USB_Device_SetCfg(USB_OTG_CORE_HANDLE  *pusbdev, uint8_t cfgidx)
{
  pusbdev->dev.class_cb->Init(pusbdev, cfgidx); 
  
  /* Upon set config call usr call back */
  pusbdev->dev.user_cb->DeviceConfigured();
  return USB_DEVICE_OK; 
}

/**
* @brief  USB_Device_ClrCfg 
*         Clear current configuration
* @param  pusbdev: device instance
* @param  cfgidx: configuration index
* @retval status: USB_Device_Status
*/
USB_Device_Status USB_Device_ClrCfg(USB_OTG_CORE_HANDLE  *pusbdev, uint8_t cfgidx)
{
  pusbdev->dev.class_cb->Reset(pusbdev, cfgidx);   
  return USB_DEVICE_OK;
}

/**
* @brief  USB_Device_IsoINIncomplete 
*         Handle iso in incomplete event
* @param  pusbdev: device instance
* @retval status
*/
static uint8_t USB_Device_IsoINIncomplete(USB_OTG_CORE_HANDLE  *pusbdev)
{
  pusbdev->dev.class_cb->IsoINIncomplete(pusbdev);   
  return USB_DEVICE_OK;
}

/**
* @brief  USB_Device_IsoOUTIncomplete 
*         Handle iso out incomplete event
* @param  pusbdev: device instance
* @retval status
*/
static uint8_t USB_Device_IsoOUTIncomplete(USB_OTG_CORE_HANDLE  *pusbdev)
{
  pusbdev->dev.class_cb->IsoOUTIncomplete(pusbdev);   
  return USB_DEVICE_OK;
}

#ifdef VBUS_SENSING_ENABLED
/**
* @brief  USB_Device_DevConnected 
*         Handle device connection event
* @param  pusbdev: device instance
* @retval status
*/
static uint8_t USB_Device_DevConnected(USB_OTG_CORE_HANDLE  *pusbdev)
{
  pusbdev->dev.user_cb->DeviceConnected();
  pusbdev->dev.connection_status = 1;  
  return USB_DEVICE_OK;
}

/**
* @brief  USB_Device_DevDisconnected 
*         Handle device disconnection event
* @param  pusbdev: device instance
* @retval status
*/
static uint8_t USB_Device_DevDisconnected(USB_OTG_CORE_HANDLE  *pusbdev)
{
  pusbdev->dev.user_cb->DeviceDisconnected();
  pusbdev->dev.class_cb->Reset(pusbdev, 0);
  pusbdev->dev.connection_status = 0;    
  return USB_DEVICE_OK;
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


