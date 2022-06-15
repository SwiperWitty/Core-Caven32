/**
  ******************************************************************************
  * File   : usb_bsp.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Specific api's relative to the used hardware platform
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_BSP__H__
#define __USB_BSP__H__

/* Includes ------------------------------------------------------------------*/
#include "usb_core.h"

/** @addtogroup USB_OTG_DRIVER
  * @{
  */
  
/** @defgroup USB_BSP
  * @brief This file is the 
  * @{
  */ 


/** @defgroup USB_BSP_Exported_Defines
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USB_BSP_Exported_Types
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USB_BSP_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USB_BSP_Exported_Variables
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USB_BSP_Exported_FunctionsPrototype
  * @{
  */ 
void BSP_Init(void);

void USB_OTG_BSP_Init (USB_OTG_CORE_HANDLE *pusbdev);
void USB_OTG_BSP_uDelay (const uint32_t usec);
void USB_OTG_BSP_mDelay (const uint32_t msec);
void USB_OTG_BSP_EnableInterrupt (USB_OTG_CORE_HANDLE *pusbdev);
void USB_OTG_BSP_TimerIRQ (void);
#ifdef USE_HOST_MODE
void USB_OTG_BSP_ConfigVBUS(USB_OTG_CORE_HANDLE *pusbdev);
void USB_OTG_BSP_DriveVBUS(USB_OTG_CORE_HANDLE *pusbdev,uint8_t state);
void USB_OTG_BSP_Resume(USB_OTG_CORE_HANDLE *pusbdev) ;                                                                
void USB_OTG_BSP_Suspend(USB_OTG_CORE_HANDLE *pusbdev);

#endif /* USE_HOST_MODE */
/**
  * @}
  */ 

#endif /* __USB_BSP__H__ */

/**
  * @}
  */ 

/**
  * @}
  */ 

