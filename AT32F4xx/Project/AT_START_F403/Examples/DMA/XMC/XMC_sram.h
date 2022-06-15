 /**
 **************************************************************************
 * File   : DMA/XMC/XMC_sram.h
  * Version: V1.2.7
  * Date   : 2020-11-10
 * Brief  : Main program body
 **************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef XMC_SRAM_H
#define XMC_SRAM_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"


void SRAM_Init(void);
void SRAM_WriteBuffer(uint16_t* pBuffer, uint32_t WriteAddr, uint32_t NumHalfwordToWrite);
void SRAM_ReadBuffer(uint16_t* pBuffer, uint32_t ReadAddr, uint32_t NumHalfwordToRead);

#ifdef __cplusplus
}
#endif

#endif /* __AT32F403_XMC_SRAM_H */

