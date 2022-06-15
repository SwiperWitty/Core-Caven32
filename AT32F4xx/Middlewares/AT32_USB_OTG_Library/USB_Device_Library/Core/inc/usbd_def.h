/**
  ******************************************************************************
  * File   : usbd_def.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : general defines for the usb device library 
  ******************************************************************************
  */   
  

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __USBD_DEF_H
#define __USBD_DEF_H

/* Includes ------------------------------------------------------------------*/
#include "usbd_conf.h"

/** @addtogroup AT32_USB_OTG_DEVICE_LIBRARY
  * @{
  */
  
/** @defgroup USB_DEF
  * @brief general defines for the usb device library file
  * @{
  */ 

/** @defgroup USB_DEF_Exported_Defines
  * @{
  */ 

#ifndef NULL
#define NULL    0
#endif

#define  USB_DEV_QUALIFIER_DESC_LEN                     0x0A
#define  USB_DEV_DESC_LEN                               0x12
#define  USB_CFG_DESC_LEN                               0x09
#define  USB_IF_DESC_LEN                                0x09
#define  USB_EPT_DESC_LEN                                0x07
#define  USB_OTG_DESC_LEN                               0x03

#define  USB_DEVICE_IDX_LANGID_STRING                            0x00 
#define  USB_DEVICE_IDX_MAFAC_STRING                               0x01 
#define  USB_DEVICE_IDX_PRODUCT_STRING                           0x02
#define  USB_DEVICE_IDX_SERIAL_STRING                            0x03 
#define  USB_DEVICE_IDX_CONFIG_STRING                            0x04 
#define  USB_DEVICE_IDX_INTERFACE_STRING                         0x05 

#define  USB_REQUEST_TYPE_STANDARD                          0x00
#define  USB_REQUEST_TYPE_CLASS                             0x20
#define  USB_REQUEST_TYPE_VENDOR                            0x40
#define  USB_REQUEST_TYPE_MASK                              0x60

#define  USB_REQUEST_RECIPIENT_DEVICE                       0x00
#define  USB_REQUEST_RECIPIENT_INTERFACE                    0x01
#define  USB_REQUEST_RECIPIENT_ENDPOINT                     0x02
#define  USB_REQUEST_RECIPIENT_MASK                         0x03

#define  USB_REQUEST_GET_STATUS                             0x00
#define  USB_REQUEST_CLEAR_FEATURE                          0x01
#define  USB_REQUEST_SET_FEATURE                            0x03
#define  USB_REQUEST_SET_ADDRESS                            0x05
#define  USB_REQUEST_GET_DESCRIPTOR                         0x06
#define  USB_REQUEST_SET_DESCRIPTOR                         0x07
#define  USB_REQUEST_GET_CONFIGURATION                      0x08
#define  USB_REQUEST_SET_CONFIGURATION                      0x09
#define  USB_REQUEST_GET_INTERFACE                          0x0A
#define  USB_REQUEST_SET_INTERFACE                          0x0B
#define  USB_REQUEST_SYNCH_FRAME                            0x0C

#define  USB_DESC_TYPE_DEVICE                              1
#define  USB_DESC_TYPE_CONFIGURATION                       2
#define  USB_DESC_TYPE_STRING                              3
#define  USB_DESC_TYPE_INTERFACE                           4
#define  USB_DESC_TYPE_ENDPOINT                            5
#define  USB_DESC_TYPE_DEVICE_QUALIFIER                    6
#define  USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION           7
#define  USB_DESC_TYPE_BOS                                 0x0F

#define USB_CONFIG_REMOTE_WAKEUP                           2
#define USB_CONFIG_SELF_POWERED                            1

#define USB_FEATURE_EP_HALT                                0
#define USB_FEATURE_REMOTE_WAKEUP                          1
#define USB_FEATURE_TEST_MODE                              2

/**
  * @}
  */ 


/** @defgroup USBD_DEF_Exported_TypesDefinitions
  * @{
  */
/**
  * @}
  */ 



/** @defgroup USBD_DEF_Exported_Macros
  * @{
  */ 
#define  SWAPBYTE(addr)        (((uint16_t)(*((uint8_t *)(addr)))) + \
                               (((uint16_t)(*(((uint8_t *)(addr)) + 1))) << 8))

#define LOBYTE(x)  ((uint8_t)((x) & 0x00FF))
#define HIBYTE(x)  ((uint8_t)(((x) & 0xFF00) >>8))
/**
  * @}
  */ 

/** @defgroup USBD_DEF_Exported_Variables
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USBD_DEF_Exported_FunctionsPrototype
  * @{
  */ 

/**
  * @}
  */ 

#endif /* __USBD_DEF_H */

/**
  * @}
  */ 

/**
* @}
*/ 
