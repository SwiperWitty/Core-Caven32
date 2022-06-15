/**
  ******************************************************************************
  * File   : usbd_msc_data.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file provides all the vital inquiry pages and sense data.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usbd_msc_data.h"


/** @addtogroup AT32_USB_OTG_DEVICE_LIBRARY
  * @{
  */


/** @defgroup MSC_DATA 
  * @brief Mass storage info/data module
  * @{
  */ 

/** @defgroup MSC_DATA_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup MSC_DATA_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup MSC_DATA_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup MSC_DATA_Private_Variables
  * @{
  */ 


/* USB Mass storage Page 0 Inquiry Data */
const uint8_t  MSC_Page00_Inquiry_Data[] = {//7						
	0x00,		
	0x00, 
	0x00, 
	(LENGTH_INQUIRY_PAGE00 - 4),
	0x00, 
	0x80, 
	0x83 
};  
/* USB Mass storage sense 6  Data */
const uint8_t  MSC_Mode_Sense6_data[] = {
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00, 
	0x00,
	0x00
};	
/* USB Mass storage sense 10  Data */
const uint8_t  MSC_Mode_Sense10_data[] = {
	0x00,
	0x06, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00
};
/**
  * @}
  */ 


/** @defgroup MSC_DATA_Private_FunctionPrototypes
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup MSC_DATA_Private_Functions
  * @{
  */ 

/**
  * @}
  */ 


/**
  * @}
  */ 


/**
  * @}
  */ 

