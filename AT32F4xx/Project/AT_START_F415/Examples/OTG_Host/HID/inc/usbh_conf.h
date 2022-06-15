/**
  ******************************************************************************
  * File   : usbh_conf.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : General low level driver configuration
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBH_CONF__H__
#define __USBH_CONF__H__

/* Includes ------------------------------------------------------------------*/
/** @addtogroup USBH_OTG_DRIVER
  * @{
  */
  
/** @defgroup USBH_CONF
  * @brief usb otg low level driver configuration file
  * @{
  */ 

/** @defgroup USBH_CONF_Exported_Defines
  * @{
  */ 

#define USBH_MAX_NUM_ENDPOINTS                2  /* 1 bulk IN + 1 bulk Out */
                                                 /* + 1 additional interrupt IN* needed for some keys*/
#define USBH_MAX_NUM_INTERFACES               2
#define USBH_MSC_MAX_LUNS                     5  /* Up to 5 LUNs to be supported */
#ifdef USE_USB_OTG_FS 
#define USBH_MSC_MPS_SIZE                 0x40
#else
#define USBH_MSC_MPS_SIZE                 0x200
#endif
#define USBH_MAX_DATA_BUFFER              0x400

/**
  * @}
  */ 


/** @defgroup USBH_CONF_Exported_Types
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBH_CONF_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBH_CONF_Exported_Variables
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBH_CONF_Exported_FunctionsPrototype
  * @{
  */ 
/**
  * @}
  */ 


#endif //__USBH_CONF__H__


/**
  * @}
  */ 

/**
  * @}
  */ 

