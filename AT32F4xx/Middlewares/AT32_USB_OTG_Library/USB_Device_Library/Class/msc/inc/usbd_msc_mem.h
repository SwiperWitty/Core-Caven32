 /**
  ******************************************************************************
  * File   : usbd_msc_mem.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : header for the STORAGE DISK file file
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __USBD_MEM_H
#define __USBD_MEM_H
/* Includes ------------------------------------------------------------------*/
#include "usbd_def.h"


/** @addtogroup AT32_USB_OTG_DEVICE_LIBRARY
  * @{
  */
  
/** @defgroup USBD_MEM
  * @brief header file for the storage disk file
  * @{
  */ 

/** @defgroup USBD_MEM_Exported_Defines
  * @{
  */ 
#define USBD_STD_INQUIRY_LENGTH		36
/**
  * @}
  */ 


/** @defgroup USBD_MEM_Exported_TypesDefinitions
  * @{
  */

typedef struct _USBD_STORAGE
{
  int8_t (* Init) (uint8_t lun);
  int8_t (* GetCapacity) (uint8_t lun, uint32_t *block_num, uint32_t *block_size);
  int8_t (* IsReady) (uint8_t lun);
  int8_t (* IsWriteProtected) (uint8_t lun);
  int8_t (* Read) (uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
  int8_t (* Write)(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
  int8_t (* GetMaxLun)(void);
  int8_t *pInquiry;
  
}USBD_STORAGE_cb_TypeDef;
/**
  * @}
  */ 



/** @defgroup USBD_MEM_Exported_Macros
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USBD_MEM_Exported_Variables
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USBD_MEM_Exported_FunctionsPrototype
  * @{
  */ 
extern USBD_STORAGE_cb_TypeDef *USBD_STORAGE_fops;
/**
  * @}
  */ 

#endif /* __USBD_MEM_H */
/**
  * @}
  */ 

/**
  * @}
  */ 

/**
* @}
*/ 
