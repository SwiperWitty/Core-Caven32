/**
  ******************************************************************************
  * File   : usbd_printer_core.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Peripheral Device Interface Layer
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#ifndef __USB_PRINTER_CORE_H_
#define __USB_PRINTER_CORE_H_

#include  "usbd_ioreq.h"

/** @addtogroup AT32_USB_OTG_DEVICE_LIBRARY
  * @{
  */
  
/** @defgroup usbd_printer
  * @brief This file is the Header file for usb_printer_core.c
  * @{
  */ 


/** @defgroup usbd_printer_Exported_Defines
  * @{
  */ 
#define USB_PRINTER_CONFIG_DESC_SIZ                (32)
#define USB_PRINTER_DESC_SIZ                       (67-9)

#define DEVICE_CLASS_PRINTER                        0x07
#define DEVICE_SUBCLASS_PRINTER                     0x01

#define PRINTER_DATA_IN_PACKET_SIZE                PRINTER_DATA_MAX_PACKET_SIZE
        
#define PRINTER_DATA_OUT_PACKET_SIZE               PRINTER_DATA_MAX_PACKET_SIZE

#define DEVICE_ID_LEN                   24

#define GET_DEVICE_ID                   0x00
#define GET_PORT_STATUS                 0x01
#define SOFT_RESET                      0x02


/**
  * @}
  */ 
/** @defgroup USBD_CORE_Exported_TypesDefinitions
  * @{
  */
typedef struct _CDC_IF_PROP
{
  uint16_t (*pIf_DataTx)   (void);
  uint16_t (*pIf_DataRx)   (uint32_t Len);
}
CDC_IF_Prop_TypeDef;
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

extern USBD_Class_cb_Type  USBD_Printer_cb;
/**
  * @}
  */ 

/** @defgroup USB_CORE_Exported_Functions
  * @{
  */
/**
  * @}
  */ 

#endif  /* __USB_CDC_CORE_H_ */
/**
  * @}
  */ 

/**
  * @}
  */ 
  
