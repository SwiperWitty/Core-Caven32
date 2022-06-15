/**
  ******************************************************************************
  * File   : psram.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file contains all the functions prototypes for the 
  *          psram firmware driver.
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PSRAM_H
#define __PSRAM_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
/** @addtogroup XMC_SRAM
  * @{
  */ 
  
/** @addtogroup XMC_Utilities
  * @{
  */
 

/** @defgroup AT32F403_XMC_SRAM_Exported_Functions
  * @{
  */      

void PSRAM_Init(void);
void PSRAM_WriteBuffer(uint16_t* pBuffer, uint32_t WriteAddr, uint32_t NumHalfwordToWrite);
void PSRAM_ReadBuffer(uint16_t* pBuffer, uint32_t ReadAddr, uint32_t NumHalfwordToRead);

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

