/**
  ******************************************************************************
  * File   : sram.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file contains all the functions prototypes for the 
  *          sram firmware driver.
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AT_START_FSMC_SRAM_H
#define __AT_START_FSMC_SRAM_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"

/** @addtogroup Utilities
  * @{
  */
  
/** @addtogroup AT32_EVAL
  * @{
  */ 

/** @addtogroup AT_START
  * @{
  */
  
/** @addtogroup AT_START_FSMC_SRAM
  * @{
  */  

/** @defgroup AT_START_FSMC_SRAM_Exported_Types
  * @{
  */
/**
  * @}
  */
  
/** @defgroup AT_START_FSMC_SRAM_Exported_Constants
  * @{
  */ 
/**
  * @}
  */ 
  
/** @defgroup AT_START_FSMC_SRAM_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup AT_START_FSMC_SRAM_Exported_Functions
  * @{
  */      

void SRAM_Init(void);
void SRAM_WriteBuffer(uint16_t* pBuffer, uint32_t WriteAddr, uint32_t NumHalfwordToWrite);
void SRAM_ReadBuffer(uint16_t* pBuffer, uint32_t ReadAddr, uint32_t NumHalfwordToRead);

#ifdef __cplusplus
}
#endif

#endif /* __AT_START_FSMC_SRAM_H */
/**
  * @}
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

/**
  * @}
  */ 

