/**
  ******************************************************************************
  * File   : usbh_usr.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file is the header file for usb usr file 
  ******************************************************************************
  */   


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USH_USR_H__
#define __USH_USR_H__


/* Includes ------------------------------------------------------------------*/
#include "usbh_core.h"
#include "usb_conf.h"
#include <stdio.h>

/** @addtogroup USBH_USER
* @{
*/

/** @addtogroup USBH_HID_DEMO_USER_CALLBACKS
* @{
*/
  
/** @defgroup USBH_USR 
  * @brief This file is the header file for user action
  * @{
  */ 


/** @defgroup USBH_CORE_Exported_Variables
  * @{
  */ 


extern  USB_Host_User_cb_Type USR_Callbacks;


/**
  * @}
  */ 


/** @defgroup USBH_CORE_Exported_FunctionsPrototype
  * @{
  */ 

void USB_HOST_USER_ApplicationSelected(void);
void USB_HOST_USER_Init(void);
void USB_HOST_USER_Reset(void);
void USB_HOST_USER_DeviceAttached(void);
void USB_HOST_USER_ResetDevice(void);
void USB_HOST_USER_DeviceDisconnected (void);
void USB_HOST_USER_OverCurrentDetected (void);
void USB_HOST_USER_DeviceSpeedDetected(uint8_t DeviceSpeed); 
void USB_HOST_USER_Device_DescAvailable(void *);
void USB_HOST_USER_DeviceAddressAssigned(void);
void USB_HOST_USER_Configuration_DescAvailable(USB_HOST_CfgDesc_Type * cfgDesc,
                                          USB_HOST_InterfaceDesc_Type *itfDesc,
                                          USB_HOST_EpDesc_Type *epDesc);
void USB_HOST_USER_Manufacturer_String(void *);
void USB_HOST_USER_Product_String(void *);
void USB_HOST_USER_SerialNum_String(void *);
void USB_HOST_USER_EnumerationDone(void);
USB_Host_USR_Status USB_HOST_USER_UserInput(void);
int USB_HOST_USER_MSC_Application(void);
void USB_HOST_USER_Reset(void);
void USB_HOST_USER_DeviceNotSupported(void);
void USB_HOST_USER_UnrecoveredError(void);

/**
  * @}
  */ 

#endif /* __USBH_USR_H */
/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 
