/**
  ******************************************************************************
  * File   : usbd_desc.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Peripheral Device Interface Layer
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __USB_DESC_H
#define __USB_DESC_H

/* Includes ------------------------------------------------------------------*/
#include "usbd_req.h"

/** @addtogroup AT32_USB_OTG_DEVICE_LIBRARY
  * @{
  */
  
/** @defgroup USB_DESC
  * @brief general defines for the usb device library file
  * @{
  */ 

/** @defgroup USB_DESC_Exported_Defines
  * @{
  */
#define USB_DEVICE_DESCRIPTOR_TYPE              0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE       0x02
#define USB_STRING_DESCRIPTOR_TYPE              0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE           0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE            0x05
#define USB_SIZ_DEVICE_DESC                     18
#define USB_SIZ_STRING_LANGID                   4

#define         DEVICE_ID1          (0x1FFFF7E8)
#define         DEVICE_ID2          (0x1FFFF7EA)
#define         DEVICE_ID3          (0x1FFFF7EC)  

#define  USB_SIZ_STRING_SERIAL       0x1A

/**
  * @}
  */ 


/** @defgroup USBD_DESC_Exported_TypesDefinitions
  * @{
  */
/**
  * @}
  */ 



/** @defgroup USBD_DESC_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBD_DESC_Exported_Variables
  * @{
  */ 
extern  uint8_t USB_Device_Desc  [USB_SIZ_DEVICE_DESC];
extern  uint8_t USB_Device_StrDesc[USB_MAX_STR_DESC_SIZ];
extern  uint8_t USB_Device_OtherSpeedCfgDesc[USB_CFG_DESC_LEN]; 
extern  uint8_t USB_Device_QualifierDesc[USB_DEV_QUALIFIER_DESC_LEN];
extern  uint8_t USB_Device_LangIDDesc[USB_SIZ_STRING_LANGID];
extern  USB_DEVICE_USER USER_desc; 
/**
  * @}
  */ 

/** @defgroup USBD_DESC_Exported_FunctionsPrototype
  * @{
  */ 


uint8_t *     USB_DEVICE_USER_DeviceDescriptor( uint8_t speed , uint16_t *length);
uint8_t *     USB_DEVICE_USER_LangIDStrDescriptor( uint8_t speed , uint16_t *length);
uint8_t *     USB_DEVICE_USER_ManufacturerStrDescriptor ( uint8_t speed , uint16_t *length);
uint8_t *     USB_DEVICE_USER_ProductStrDescriptor ( uint8_t speed , uint16_t *length);
uint8_t *     USB_DEVICE_USER_SerialStrDescriptor( uint8_t speed , uint16_t *length);
uint8_t *     USB_DEVICE_USER_ConfigStrDescriptor( uint8_t speed , uint16_t *length);
uint8_t *     USB_DEVICE_USER_InterfaceStrDescriptor( uint8_t speed , uint16_t *length);

#ifdef USB_SUPPORT_USER_STRING_DESC
uint8_t *     USB_DEVICE_USER_USRStringDesc (uint8_t speed, uint8_t idx , uint16_t *length);  
#endif /* USB_SUPPORT_USER_STRING_DESC */  
  
/**
  * @}
  */ 

#endif /* __USBD_DESC_H */

/**
  * @}
  */ 

/**
* @}
*/ 
