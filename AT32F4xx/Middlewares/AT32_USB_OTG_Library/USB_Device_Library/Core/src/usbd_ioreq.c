 /**
  ******************************************************************************
  * File   : usbd_ioreq.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file provides the IO requests APIs for control endpoints.
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "usbd_ioreq.h"

/** @addtogroup AT32_USB_OTG_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_IOREQ 
  * @brief control I/O requests module
  * @{
  */ 

/** @defgroup USBD_IOREQ_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_IOREQ_Private_Defines
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup USBD_IOREQ_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_IOREQ_Private_Variables
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup USBD_IOREQ_Private_FunctionPrototypes
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_IOREQ_Private_Functions
  * @{
  */ 

/**
* @brief  USB_Device_CtrlSendData
*         send data on the ctl pipe
* @param  pusbdev: device instance
* @param  buff: pointer to data buffer
* @param  len: length of data to be sent
* @retval status
*/
USB_Device_Status  USB_Device_CtrlSendData (USB_OTG_CORE_HANDLE  *pusbdev, 
                               uint8_t *pbuf,
                               uint16_t len)
{
  USB_Device_Status ret = USB_DEVICE_OK;
  
  pusbdev->dev.in_ept[0].total_data_len = len;
  pusbdev->dev.in_ept[0].rem_data_len   = len;
  pusbdev->dev.device_state = USB_OTG_EPT0_DATAIN;

  USB_DCD_EPT_Tx (pusbdev, 0, pbuf, len);
 
  return ret;
}

/**
* @brief  USB_Device_CtrlContinueSendData
*         continue sending data on the ctl pipe
* @param  pusbdev: device instance
* @param  buff: pointer to data buffer
* @param  len: length of data to be sent
* @retval status
*/
USB_Device_Status  USB_Device_CtrlContinueSendData (USB_OTG_CORE_HANDLE  *pusbdev, 
                                       uint8_t *pbuf,
                                       uint16_t len)
{
  USB_Device_Status ret = USB_DEVICE_OK;
  
  USB_DCD_EPT_Tx (pusbdev, 0, pbuf, len);
  
  
  return ret;
}

/**
* @brief  USB_Device_CtrlPrepareRx
*         receive data on the ctl pipe
* @param  pusbdev: USB OTG device instance
* @param  buff: pointer to data buffer
* @param  len: length of data to be received
* @retval status
*/
USB_Device_Status  USB_Device_CtrlPrepareRx (USB_OTG_CORE_HANDLE  *pusbdev,
                                  uint8_t *pbuf,                                  
                                  uint16_t len)
{
  USB_Device_Status ret = USB_DEVICE_OK;
  
  pusbdev->dev.out_ept[0].total_data_len = len;
  pusbdev->dev.out_ept[0].rem_data_len   = len;
  pusbdev->dev.device_state = USB_OTG_EPT0_DATAOUT;
  
  USB_DCD_EPT_PrepareRx (pusbdev,
                    0,
                    pbuf,
                    len);
  

  return ret;
}

/**
* @brief  USB_Device_CtrlContinueRx
*         continue receive data on the ctl pipe
* @param  pusbdev: USB OTG device instance
* @param  buff: pointer to data buffer
* @param  len: length of data to be received
* @retval status
*/
USB_Device_Status  USB_Device_CtrlContinueRx (USB_OTG_CORE_HANDLE  *pusbdev, 
                                          uint8_t *pbuf,                                          
                                          uint16_t len)
{
  USB_Device_Status ret = USB_DEVICE_OK;
  
  USB_DCD_EPT_PrepareRx (pusbdev,
                    0,                     
                    pbuf,                         
                    len);
  return ret;
}
/**
* @brief  USB_Device_CtrlSendStatus
*         send zero length packet on the ctl pipe
* @param  pusbdev: USB OTG device instance
* @retval status
*/
USB_Device_Status  USB_Device_CtrlSendStatus (USB_OTG_CORE_HANDLE  *pusbdev)
{
  USB_Device_Status ret = USB_DEVICE_OK;
  pusbdev->dev.device_state = USB_OTG_EPT0_STATUSIN;
  USB_DCD_EPT_Tx (pusbdev,
             0,
             NULL, 
             0); 
  
  USB_OTG_EPT0_OutStart(pusbdev);  
  
  return ret;
}

/**
* @brief  USB_Device_CtrlReceiveStatus
*         receive zero length packet on the ctl pipe
* @param  pusbdev: USB OTG device instance
* @retval status
*/
USB_Device_Status  USB_Device_CtrlReceiveStatus (USB_OTG_CORE_HANDLE  *pusbdev)
{
  USB_Device_Status ret = USB_DEVICE_OK;
  pusbdev->dev.device_state = USB_OTG_EPT0_STATUSOUT;  
  USB_DCD_EPT_PrepareRx ( pusbdev,
                    0,
                    NULL,
                    0);  

  USB_OTG_EPT0_OutStart(pusbdev);
  
  return ret;
}


/**
* @brief  USB_Device_GetRxCount
*         returns the received data length
* @param  pusbdev: USB OTG device instance
*         bepnum: endpoint index
* @retval Rx Data blength
*/
uint16_t  USB_Device_GetRxCount (USB_OTG_CORE_HANDLE  *pusbdev , uint8_t bepnum)
{
  return pusbdev->dev.out_ept[bepnum].xfer_count;
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

