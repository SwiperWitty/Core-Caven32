/**
  ******************************************************************************
  * File   : usb_req.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Header file for the usbd_req.c file
  ******************************************************************************
  */  

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __USB_REQUEST_H_
#define __USB_REQUEST_H_

/* Includes ------------------------------------------------------------------*/
#include  "usbd_def.h"
#include  "usbd_core.h"
#include  "usbd_conf.h"


/** @addtogroup AT32_USB_OTG_DEVICE_LIBRARY
  * @{
  */
  
/** @defgroup USBD_REQ
  * @brief header file for the usbd_ioreq.c file
  * @{
  */ 

/** @defgroup USBD_REQ_Exported_Defines
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_REQ_Exported_Types
  * @{
  */
/**
  * @}
  */ 



/** @defgroup USBD_REQ_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBD_REQ_Exported_Variables
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBD_REQ_Exported_FunctionsPrototype
  * @{
  */ 

USB_Device_Status  USB_Device_StdDevReq (USB_OTG_CORE_HANDLE  *pusbdev, USB_SETUP_REQEST  *req);
USB_Device_Status  USB_Device_StdItfReq (USB_OTG_CORE_HANDLE  *pusbdev, USB_SETUP_REQEST  *req);
USB_Device_Status  USB_Device_StdEPReq (USB_OTG_CORE_HANDLE  *pusbdev, USB_SETUP_REQEST  *req);
void USB_Device_ParseSetupRequest( USB_OTG_CORE_HANDLE  *pusbdev,
                                    USB_SETUP_REQEST *req);

void USB_Device_CtlError( USB_OTG_CORE_HANDLE  *pusbdev,
                            USB_SETUP_REQEST *req);

void USB_Device_GetString(uint8_t *desc, uint8_t *unicode, uint16_t *len);
/**
  * @}
  */ 

#endif /* __USB_REQUEST_H_ */

/**
  * @}
  */ 

/**
* @}
*/ 


