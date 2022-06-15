/**
  ******************************************************************************
  * File   : usbh_hcs.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Header file for usbh_hcs.c
  ******************************************************************************
  */   

/* Define to prevent recursive  ----------------------------------------------*/
#ifndef __USBH_HCS_H
#define __USBH_HCS_H

/* Includes ------------------------------------------------------------------*/
#include "usbh_core.h"

/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_LIB_CORE
* @{
*/
  
/** @defgroup USBH_HCS
  * @brief This file is the header file for usbh_hcs.c
  * @{
  */ 

/** @defgroup USBH_HCS_Exported_Defines
  * @{
  */
#define HCH_MAX           8

#define HCH_OK            0x0000
#define HCH_USED          0x8000
#define HCH_ERROR         0xFFFF
#define HCH_USED_MASK     0x7FFF
/**
  * @}
  */ 

/** @defgroup USBH_HCS_Exported_Types
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBH_HCS_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBH_HCS_Exported_Variables
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBH_HCS_Exported_FunctionsPrototype
  * @{
  */

uint8_t USB_Host_Alloc_Channel(USB_OTG_CORE_HANDLE *pusbdev, uint8_t ept_addr);

uint8_t USB_Host_Free_Channel  (USB_OTG_CORE_HANDLE *pusbdev, uint8_t idx);

uint8_t USB_Host_DeAllocate_AllChannel  (USB_OTG_CORE_HANDLE *pusbdev);

uint8_t USB_Host_Open_Channel  (USB_OTG_CORE_HANDLE *pusbdev,
                            uint8_t ch_num,
                            uint8_t dev_address,
                            uint8_t speed,
                            uint8_t ept_type,
                            uint16_t maxpsize);

uint8_t USB_Host_Modify_Channel (USB_OTG_CORE_HANDLE *pusbdev,
                            uint8_t hc_num,
                            uint8_t dev_address,
                            uint8_t speed,
                            uint8_t ept_type,
                            uint16_t maxpsize);
/**
  * @}
  */ 



#endif /* __USBH_HCS_H */


/**
  * @}
  */ 

/**
  * @}
  */ 

/**
* @}
*/ 



