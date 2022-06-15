/**
  ******************************************************************************
  * File   : usbd_customhid_core.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : header file for the usbd_hid_core.c file.
  ******************************************************************************
  */  
/* Includes ------------------------------------------------------------------*/

#ifndef __USB_HID_CORE_H_
#define __USB_HID_CORE_H_

#include  "usbd_ioreq.h"

/** @addtogroup AT32_USB_OTG_DEVICE_LIBRARY
  * @{
  */
  
/** @defgroup USBD_HID
  * @brief This file is the Header file for USBD_msc.c
  * @{
  */ 


/** @defgroup USBD_HID_Exported_Defines
  * @{
  */ 
//#define USB_HID_CONFIG_DESC_SIZ       34
#define CUSTOMHID_SIZ_CONFIG_DESC               41
#define USB_HID_DESC_SIZ              9
//#define HID_MOUSE_REPORT_DESC_SIZE    74
#define CUSTOMHID_SIZ_REPORT_DESC     126

#define HID_DESCRIPTOR_TYPE           0x21
#define HID_REPORT_DESC               0x22

#define HID_HS_BINTERVAL              0x07
#define HID_FS_BINTERVAL              0x02

#define HID_REQ_SET_PROTOCOL          0x0B
#define HID_REQ_GET_PROTOCOL          0x03

#define HID_REQ_SET_IDLE              0x0A
#define HID_REQ_GET_IDLE              0x02

#define HID_REQ_SET_REPORT            0x09
#define HID_REQ_GET_REPORT            0x01
/**
  * @}
  */ 


/** @defgroup USBD_CORE_Exported_TypesDefinitions
  * @{
  */


/**
  * @}
  */ 



/** @defgroup USBD_CORE_Exported_Macros
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USBD_CORE_Exported_Variables
  * @{
  */ 

extern USBD_Class_cb_Type  USBD_HID_cb;
/**
  * @}
  */ 

/** @defgroup USB_CORE_Exported_Functions
  * @{
  */ 
uint8_t usb_device_hid_sendreport (USB_OTG_CORE_HANDLE  *pusbdev, 
                                 uint8_t *report,
                                 uint16_t len);

uint32_t usb_device_hid_getpollinginterval (USB_OTG_CORE_HANDLE *pusbdev);

/**
  * @}
  */ 

#endif  /*  __USB_HID_CORE_H_ */
/**
  * @}
  */ 

/**
  * @}
  */ 
  
