/**
  ******************************************************************************
  * File   : usbd_usr.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Header file for usbd_usr.c
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_USR_H__
#define __USBD_USR_H__

/* Includes ------------------------------------------------------------------*/
#include "usbd_ioreq.h"
/** @addtogroup USBD_USER
  * @{
  */

/** @addtogroup USBD_MSC_DEMO_USER_CALLBACKS
  * @{
  */

/** @defgroup USBD_USR
  * @brief This file is the Header file for usbd_usr.c
  * @{
  */ 


/** @defgroup USBD_USR_Exported_Types
  * @{
  */ 

extern  USB_Device_User_cb_Type USER_cb;
extern  USB_Device_User_cb_Type USR_FS_cb;
extern  USB_Device_User_cb_Type USR_HS_cb;



/**
  * @}
  */ 



/** @defgroup USBD_USR_Exported_Defines
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USBD_USR_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBD_USR_Exported_Variables
  * @{
  */ 

void     USB_DEVICE_USER_Init(void);
void     USB_DEVICE_USER_DeviceReset (uint8_t speed);
void     USB_DEVICE_USER_DeviceConfigured (void);
void     USB_DEVICE_USER_DeviceSuspended(void);
void     USB_DEVICE_USER_DeviceResumed(void);

void     USB_DEVICE_USER_DeviceConnected(void);
void     USB_DEVICE_USER_DeviceDisconnected(void); 

void     USB_DEVICE_USER_FS_Init(void);
void     USB_DEVICE_USER_FS_DeviceReset (uint8_t speed);
void     USB_DEVICE_USER_FS_DeviceConfigured (void);
void     USB_DEVICE_USER_FS_DeviceSuspended(void);
void     USB_DEVICE_USER_FS_DeviceResumed(void);

void     USB_DEVICE_USER_FS_DeviceConnected(void);
void     USB_DEVICE_USER_FS_DeviceDisconnected(void);  

/**
  * @}
  */ 

/** @defgroup USBD_USR_Exported_FunctionsPrototype
  * @{
  */ 
/**
  * @}
  */ 

#endif /*__USBD_USR_H__*/

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 

