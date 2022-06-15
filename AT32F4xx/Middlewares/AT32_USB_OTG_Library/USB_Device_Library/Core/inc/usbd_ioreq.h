/**
  ******************************************************************************
  * File   : usbd_ioreq.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : header file for the usbd_ioreq.c file
  ******************************************************************************
  */   

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __USBD_IOREQ_H_
#define __USBD_IOREQ_H_

/* Includes ------------------------------------------------------------------*/
#include  "usbd_def.h"
#include  "usbd_core.h"

/** @addtogroup AT32_USB_OTG_DEVICE_LIBRARY
  * @{
  */
  
/** @defgroup USBD_IOREQ
  * @brief header file for the usbd_ioreq.c file
  * @{
  */ 

/** @defgroup USBD_IOREQ_Exported_Defines
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_IOREQ_Exported_Types
  * @{
  */


/**
  * @}
  */ 



/** @defgroup USBD_IOREQ_Exported_Macros
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USBD_IOREQ_Exported_Variables
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USBD_IOREQ_Exported_FunctionsPrototype
  * @{
  */ 

USB_Device_Status  USB_Device_CtrlSendData (USB_OTG_CORE_HANDLE  *pusbdev, 
                               uint8_t *buf,
                               uint16_t len);

USB_Device_Status  USB_Device_CtrlContinueSendData (USB_OTG_CORE_HANDLE  *pusbdev, 
                               uint8_t *pbuf,
                               uint16_t len);

USB_Device_Status USB_Device_CtrlPrepareRx (USB_OTG_CORE_HANDLE  *pusbdev, 
                               uint8_t *pbuf,                                 
                               uint16_t len);

USB_Device_Status  USB_Device_CtrlContinueRx (USB_OTG_CORE_HANDLE  *pusbdev, 
                              uint8_t *pbuf,                                          
                              uint16_t len);

USB_Device_Status  USB_Device_CtrlSendStatus (USB_OTG_CORE_HANDLE  *pusbdev);

USB_Device_Status  USB_Device_CtrlReceiveStatus (USB_OTG_CORE_HANDLE  *pusbdev);

uint16_t  USB_Device_GetRxCount (USB_OTG_CORE_HANDLE  *pusbdev , 
                           uint8_t bepnum);

/**
  * @}
  */ 

#endif /* __USBD_IOREQ_H_ */

/**
  * @}
  */ 

/**
* @}
*/ 
