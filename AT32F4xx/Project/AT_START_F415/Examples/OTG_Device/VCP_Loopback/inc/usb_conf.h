/**
  ******************************************************************************
  * File   : usb_conf.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : General low level driver configuration
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_CONF__H__
#define __USB_CONF__H__

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "at32_board.h"

/** @addtogroup USB_OTG_DRIVER
  * @{
  */
  
/** @defgroup USB_CONF
  * @brief USB low level driver configuration file
  * @{
  */ 

/** @defgroup USB_CONF_Exported_Defines
  * @{
  */ 

/* USB Core and PHY interface configuration.
   Tip: To avoid modifying these defines each time you need to change the USB
        configuration, you can declare the needed define in your toolchain
        compiler preprocessor.
   */
/****************** USB OTG FS PHY CONFIGURATION *******************************
*  The USB OTG FS Core supports one on-chip Full Speed PHY.
*  
*  The USE_EMBEDDED_PHY symbol is defined in the project compiler preprocessor 
*  when FS core is used.
*******************************************************************************/
#ifndef USE_USB_OTG_FS
 #define USE_USB_OTG_FS 
#endif /* USE_USB_OTG_FS */

#ifdef USE_USB_OTG_FS 
 #define USB_OTG_FS_CORE
#endif
 
/****************** USB OTG FS CONFIGURATION **********************************/
#ifdef USB_OTG_FS_CORE
 #define RX_FIFO_FS_SIZE                          128
 #define TX0_FIFO_FS_SIZE                          64
 #define TX1_FIFO_FS_SIZE                         128
 #define TX2_FIFO_FS_SIZE                          0
 #define TX3_FIFO_FS_SIZE                          0

// #define USB_OTG_FS_LOW_PWR_MGMT_SUPPORT 
/* #define USB_OTG_FS_SOF_OUTPUT_ENABLED */
#endif

/****************** USB OTG MISC CONFIGURATION ********************************/
#define VBUS_SENSING_ENABLED

/****************** USB OTG MODE CONFIGURATION ********************************/
/* #define USE_HOST_MODE */
#define USE_DEVICE_MODE
/* #define USE_OTG_MODE */

//#ifndef USE_DEVICE_MODE
// #ifndef USE_HOST_MODE
//    #error  "USE_DEVICE_MODE or USE_HOST_MODE should be defined"
// #endif
//#endif

// #ifndef USE_USB_OTG_FS
//    #error  "USE_USB_OTG_HS or USE_USB_OTG_FS should be defined"
//#endif

/****************** C Compilers dependant keywords ****************************/

/**
  * @}
  */ 


/** @defgroup USB_CONF_Exported_Types
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USB_CONF_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USB_CONF_Exported_Variables
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USB_CONF_Exported_FunctionsPrototype
  * @{
  */ 
/**
  * @}
  */ 


#endif /*__USB_CONF__H__*/


/**
  * @}
  */ 

/**
  * @}
  */ 


