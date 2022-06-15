/**
  ******************************************************************************
  * File   : USB_Device/VirtualMassStorage_IAP_Demo/inc/flash_if.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : flash header
  ******************************************************************************
  */
#ifndef __FLASH_IF_H__
#define __FLASH_IF_H__
#include "at32f4xx.h"

#define FLASH_BASE_ADDR         0x08000000
#define FLASH_START_ADDR        0x08005000       // Flash start address

#define FLASH_SIZE              (1*1024*1024)  // 100M
#define FLASH_PAGE2K_SIZE         0x800         // 2k Bytes per page
#define FLASH_PAGE1K_SIZE         0x400         // 2k Bytes per page
#define FLASH_WAIT_TIMEOUT      100000

#define FLASH_PAGE1K_ALIGNED_MASK       0x3FF
#define FLASH_PAGE2K_ALIGNED_MASK       0x7FF


extern uint32_t AT_FLASH_PAGE_MASK;
extern uint32_t AT_FLASH_PAGE_SIZE;
extern uint32_t AT_FLASH_SIZE;
extern uint32_t FLASH_FIRMWARE_LENGTH;
extern uint32_t FLASH_WRCRCValue;
extern uint32_t FLASH_RDCRCValue;

void AT_FLASH_Init(void);
uint8_t AT_FLASH_WriteBuffer(uint32_t address, uint8_t *data, uint32_t len);
void AT_FLASH_ReadBuffer(uint32_t address, uint8_t *rdata, uint32_t len);
void IAP_Jump(uint32_t dwAddr);
uint8_t AT_ReadOptionByte(void);
void AT_WriteOptionByte(uint8_t Flag);
uint32_t AT_CalculateCRC_Buffer(uint32_t *buffer, uint32_t NumByteToCrc);
uint32_t AT_CalculateCRC_Flash(uint32_t dwAddr, uint32_t NumByteToCrc);
#endif
