/**
  ******************************************************************************
  * File   : USB_Device/VirtualMassStorage_IAP_Demo/inc/memory.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Memory management layer
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __memory_H
#define __memory_H

/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define TXFR_IDLE     0
#define TXFR_ONGOING  1

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Write_Memory (uint8_t lun, uint32_t Memory_Offset, uint32_t Transfer_Length);
void Read_Memory (uint8_t lun, uint32_t Memory_Offset, uint32_t Transfer_Length);
#endif /* __memory_H */

