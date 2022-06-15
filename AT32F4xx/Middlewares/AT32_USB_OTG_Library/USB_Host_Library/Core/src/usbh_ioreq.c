/**
  ******************************************************************************
  * File   : usbh_ioreq.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file handles the issuing of the USB transactions
  ******************************************************************************
  */    
/* Includes ------------------------------------------------------------------*/

#include "usbh_ioreq.h"

/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_LIB_CORE
* @{
*/
  
/** @defgroup USBH_IOREQ 
  * @brief This file handles the standard protocol processing (USB v2.0)
  * @{
  */


/** @defgroup USBH_IOREQ_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 
 

/** @defgroup USBH_IOREQ_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 



/** @defgroup USBH_IOREQ_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBH_IOREQ_Private_Variables
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBH_IOREQ_Private_FunctionPrototypes
  * @{
  */ 
static USB_HOST_Status USB_HOST_SubmitSetupRequest(USB_HOST *phost,
                                           uint8_t* buff, 
                                           uint16_t length);

/**
  * @}
  */ 


/** @defgroup USBH_IOREQ_Private_Functions
  * @{
  */ 


/**
  * @brief  USB_HOST_CtrlReq
  *         USB_HOST_CtrlReq sends a control request and provide the status after 
  *            completion of the request
  * @param  pusbdev: Selected device
  * @param  req: Setup Request Structure
  * @param  buff: data buffer address to store the response
  * @param  length: length of the response
  * @retval Status
  */
USB_HOST_Status USB_HOST_CtrlReq     (USB_OTG_CORE_HANDLE *pusbdev, 
                             USB_HOST           *phost, 
                             uint8_t             *buff,
                             uint16_t            length)
{
  USB_HOST_Status status;
  status = USB_HOST_BUSY;
  
  switch (phost->RequestState)
  {
  case CMD_SEND:
    /* Start a SETUP transfer */
    USB_HOST_SubmitSetupRequest(phost, buff, length);
    phost->RequestState = CMD_WAIT;
    status = USB_HOST_BUSY;
    break;
    
  case CMD_WAIT:
     if (phost->Control.state == USB_CTRL_COMPLETE ) 
    {
      /* Commands successfully sent and Response Received  */       
      phost->RequestState = CMD_SEND;
      phost->Control.state =USB_CTRL_IDLE;  
      status = USB_HOST_OK;      
    }
    else if  (phost->Control.state == USB_CTRL_ERROR)
    {
      /* Failure Mode */
      phost->RequestState = CMD_SEND;
      status = USB_HOST_FAIL;
    }   
     else if  (phost->Control.state == USB_CTRL_STALLED )
    {
      /* Commands successfully sent and Response Received  */       
      phost->RequestState = CMD_SEND;
      status = USB_HOST_NOT_SUPPORTED;
    }
    break;
    
  default:
    break; 
  }
  return status;
}

/**
  * @brief  USB_HOST_CtrlSendSetup
  *         Sends the Setup Packet to the Device
  * @param  pusbdev: Selected device
  * @param  buff: Buffer pointer from which the Data will be send to Device
  * @param  hc_num: Host channel Number
  * @retval Status
  */
USB_HOST_Status USB_HOST_CtrlSendSetup ( USB_OTG_CORE_HANDLE *pusbdev, 
                                uint8_t *buff, 
                                uint8_t hc_num){
  pusbdev->host.hc[hc_num].ept_is_in = 0;
  pusbdev->host.hc[hc_num].data_pid = HC_PID_SETUP;   
  pusbdev->host.hc[hc_num].xfer_buff = buff;
  pusbdev->host.hc[hc_num].xfer_len = USB_HOST_SETUP_PKT_SIZE;   

  return (USB_HOST_Status)USB_HCD_SubmitRequest (pusbdev , hc_num);   
}


/**
  * @brief  USB_HOST_CtrlSendData
  *         Sends a data Packet to the Device
  * @param  pusbdev: Selected device
  * @param  buff: Buffer pointer from which the Data will be sent to Device
  * @param  length: Length of the data to be sent
  * @param  hc_num: Host channel Number
  * @retval Status
  */
USB_HOST_Status USB_HOST_CtrlSendData ( USB_OTG_CORE_HANDLE *pusbdev, 
                                uint8_t *buff, 
                                uint16_t length,
                                uint8_t hc_num)
{
  pusbdev->host.hc[hc_num].ept_is_in = 0;
  pusbdev->host.hc[hc_num].xfer_buff = buff;
  pusbdev->host.hc[hc_num].xfer_len = length;
 
  if ( length == 0 )
  { /* For Status OUT stage, Length==0, Status Out PID = 1 */
    pusbdev->host.hc[hc_num].toggle_out = 1;   
  }
 
 /* Set the Data Toggle bit as per the Flag */
  if ( pusbdev->host.hc[hc_num].toggle_out == 0)
  { /* Put the PID 0 */
      pusbdev->host.hc[hc_num].data_pid = HC_PID_DATA0;    
  }
 else
 { /* Put the PID 1 */
      pusbdev->host.hc[hc_num].data_pid = HC_PID_DATA1 ;
 }

  USB_HCD_SubmitRequest (pusbdev , hc_num);   
   
  return USB_HOST_OK;
}


/**
  * @brief  USB_HOST_CtrlReceiveData
  *         Receives the Device Response to the Setup Packet
  * @param  pusbdev: Selected device
  * @param  buff: Buffer pointer in which the response needs to be copied
  * @param  length: Length of the data to be received
  * @param  hc_num: Host channel Number
  * @retval Status. 
  */
USB_HOST_Status USB_HOST_CtrlReceiveData(USB_OTG_CORE_HANDLE *pusbdev, 
                                uint8_t* buff, 
                                uint16_t length,
                                uint8_t hc_num)
{

  pusbdev->host.hc[hc_num].ept_is_in = 1;
  pusbdev->host.hc[hc_num].data_pid = HC_PID_DATA1;
  pusbdev->host.hc[hc_num].xfer_buff = buff;
  pusbdev->host.hc[hc_num].xfer_len = length;  

  USB_HCD_SubmitRequest (pusbdev , hc_num);   
  
  return USB_HOST_OK;
  
}


/**
  * @brief  USB_HOST_BulkSendData
  *         Sends the Bulk Packet to the device
  * @param  pusbdev: Selected device
  * @param  buff: Buffer pointer from which the Data will be sent to Device
  * @param  length: Length of the data to be sent
  * @param  hc_num: Host channel Number
  * @retval Status
  */
USB_HOST_Status USB_HOST_BulkSendData ( USB_OTG_CORE_HANDLE *pusbdev, 
                                uint8_t *buff, 
                                uint16_t length,
                                uint8_t hc_num)
{ 
  pusbdev->host.hc[hc_num].ept_is_in = 0;
  pusbdev->host.hc[hc_num].xfer_buff = buff;
  pusbdev->host.hc[hc_num].xfer_len = length;  

 /* Set the Data Toggle bit as per the Flag */
  if ( pusbdev->host.hc[hc_num].toggle_out == 0)
  { /* Put the PID 0 */
      pusbdev->host.hc[hc_num].data_pid = HC_PID_DATA0;    
  }
 else
 { /* Put the PID 1 */
      pusbdev->host.hc[hc_num].data_pid = HC_PID_DATA1 ;
 }

  USB_HCD_SubmitRequest (pusbdev , hc_num);   
  return USB_HOST_OK;
}


/**
  * @brief  USB_HOST_BulkReceiveData
  *         Receives IN bulk packet from device
  * @param  pusbdev: Selected device
  * @param  buff: Buffer pointer in which the received data packet to be copied
  * @param  length: Length of the data to be received
  * @param  hc_num: Host channel Number
  * @retval Status. 
  */
USB_HOST_Status USB_HOST_BulkReceiveData( USB_OTG_CORE_HANDLE *pusbdev, 
                                uint8_t *buff, 
                                uint16_t length,
                                uint8_t hc_num)
{
  pusbdev->host.hc[hc_num].ept_is_in = 1;   
  pusbdev->host.hc[hc_num].xfer_buff = buff;
  pusbdev->host.hc[hc_num].xfer_len = length;
  

  if( pusbdev->host.hc[hc_num].toggle_in == 0)
  {
    pusbdev->host.hc[hc_num].data_pid = HC_PID_DATA0;
  }
  else
  {
    pusbdev->host.hc[hc_num].data_pid = HC_PID_DATA1;
  }

  USB_HCD_SubmitRequest (pusbdev , hc_num);  
  return USB_HOST_OK;
}


/**
  * @brief  USB_HOST_InterruptReceiveData
  *         Receives the Device Response to the Interrupt IN token
  * @param  pusbdev: Selected device
  * @param  buff: Buffer pointer in which the response needs to be copied
  * @param  length: Length of the data to be received
  * @param  hc_num: Host channel Number
  * @retval Status. 
  */
USB_HOST_Status USB_HOST_InterruptReceiveData( USB_OTG_CORE_HANDLE *pusbdev, 
                                uint8_t *buff, 
                                uint8_t length,
                                uint8_t hc_num)
{

  pusbdev->host.hc[hc_num].ept_is_in = 1;  
  pusbdev->host.hc[hc_num].xfer_buff = buff;
  pusbdev->host.hc[hc_num].xfer_len = length;
  

  
  if(pusbdev->host.hc[hc_num].toggle_in == 0)
  {
    pusbdev->host.hc[hc_num].data_pid = HC_PID_DATA0;
  }
  else
  {
    pusbdev->host.hc[hc_num].data_pid = HC_PID_DATA1;
  }

  /* toggle DATA PID */
  pusbdev->host.hc[hc_num].toggle_in ^= 1;  
  
  USB_HCD_SubmitRequest (pusbdev , hc_num);  
  
  return USB_HOST_OK;
}

/**
  * @brief  USB_HOST_InterruptSendData
  *         Sends the data on Interrupt OUT Endpoint
  * @param  pusbdev: Selected device
  * @param  buff: Buffer pointer from where the data needs to be copied
  * @param  length: Length of the data to be sent
  * @param  hc_num: Host channel Number
  * @retval Status. 
  */
USB_HOST_Status USB_HOST_InterruptSendData( USB_OTG_CORE_HANDLE *pusbdev, 
                                uint8_t *buff, 
                                uint8_t length,
                                uint8_t hc_num)
{

  pusbdev->host.hc[hc_num].ept_is_in = 0;  
  pusbdev->host.hc[hc_num].xfer_buff = buff;
  pusbdev->host.hc[hc_num].xfer_len = length;
  
  if(pusbdev->host.hc[hc_num].toggle_in == 0)
  {
    pusbdev->host.hc[hc_num].data_pid = HC_PID_DATA0;
  }
  else
  {
    pusbdev->host.hc[hc_num].data_pid = HC_PID_DATA1;
  }

  pusbdev->host.hc[hc_num].toggle_in ^= 1;  
  
  USB_HCD_SubmitRequest (pusbdev , hc_num);  
  
  return USB_HOST_OK;
}


/**
  * @brief  USB_HOST_SubmitSetupRequest
  *         Start a setup transfer by changing the state-machine and 
  *         initializing  the required variables needed for the Control Transfer
  * @param  pusbdev: Selected device
  * @param  setup: Setup Request Structure
  * @param  buff: Buffer used for setup request
  * @param  length: Length of the data
  * @retval Status. 
*/
static USB_HOST_Status USB_HOST_SubmitSetupRequest(USB_HOST *phost,
                                           uint8_t* buff, 
                                           uint16_t length)
{
  
  /* Save Global State */
  phost->gStateBkp =   phost->gState; 
  
  /* Prepare the Transactions */
  phost->gState = USB_HOST_CTRL_XFER;
  phost->Control.buff = buff; 
  phost->Control.length = length;
  phost->Control.state = USB_CTRL_SETUP;  

  return USB_HOST_OK;  
}


/**
  * @brief  USB_HOST_IsocReceiveData
  *         Receives the Device Response to the Isochronous IN token
  * @param  pusbdev: Selected device
  * @param  buff: Buffer pointer in which the response needs to be copied
  * @param  length: Length of the data to be received
  * @param  hc_num: Host channel Number
  * @retval Status. 
  */
USB_HOST_Status USB_HOST_IsocReceiveData( USB_OTG_CORE_HANDLE *pusbdev, 
                                uint8_t *buff, 
                                uint32_t length,
                                uint8_t hc_num)
{    
  
  pusbdev->host.hc[hc_num].ept_is_in = 1;  
  pusbdev->host.hc[hc_num].xfer_buff = buff;
  pusbdev->host.hc[hc_num].xfer_len = length;
  pusbdev->host.hc[hc_num].data_pid = HC_PID_DATA0;
  

  USB_HCD_SubmitRequest (pusbdev , hc_num);  
  
  return USB_HOST_OK;
}

/**
  * @brief  USB_HOST_IsocSendData
  *         Sends the data on Isochronous OUT Endpoint
  * @param  pusbdev: Selected device
  * @param  buff: Buffer pointer from where the data needs to be copied
  * @param  length: Length of the data to be sent
  * @param  hc_num: Host channel Number
  * @retval Status. 
  */
USB_HOST_Status USB_HOST_IsocSendData( USB_OTG_CORE_HANDLE *pusbdev, 
                                uint8_t *buff, 
                                uint32_t length,
                                uint8_t hc_num)
{
  
  pusbdev->host.hc[hc_num].ept_is_in = 0;  
  pusbdev->host.hc[hc_num].xfer_buff = buff;
  pusbdev->host.hc[hc_num].xfer_len = length;
  pusbdev->host.hc[hc_num].data_pid = HC_PID_DATA0;
  
  USB_HCD_SubmitRequest (pusbdev , hc_num);  
  
  return USB_HOST_OK;
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




