/**
  **************************************************************************
  * File   : at32f403_nand.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : 
  **************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AT32F403_NAND_H
#define __AT32F403_NAND_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"

typedef struct
{
  uint8_t Maker_ID;
  uint8_t Device_ID;
  uint8_t Third_ID;
  uint8_t Fourth_ID;
}NAND_IDTypeDef;

typedef struct 
{
  uint16_t Zone;
  uint16_t Block;
  uint16_t Page;
} NAND_ADDRESS;  
/**
  * @}
  */
  
/** 
  * @brief  NAND Area definition 
  */  
#define CMD_AREA                   (uint32_t)(1<<16)  /* A16 = CLE  high */
#define ADDR_AREA                  (uint32_t)(1<<17)  /* A17 = ALE  high */

#define DATA_AREA                  ((uint32_t)0x00000000) 

/** 
  * @brief  XMC NAND memory command  
  */  
#define NAND_CMD_AREA_A            ((uint8_t)0x00)
#define NAND_CMD_AREA_B            ((uint8_t)0x01)
#define NAND_CMD_AREA_C            ((uint8_t)0x50)
#define NAND_CMD_AREA_TRUE1        ((uint8_t)0x30)

#define NAND_CMD_WRITE0            ((uint8_t)0x80)
#define NAND_CMD_WRITE_TRUE1       ((uint8_t)0x10)

#define NAND_CMD_ERASE0            ((uint8_t)0x60)
#define NAND_CMD_ERASE1            ((uint8_t)0xD0)

#define NAND_CMD_READID            ((uint8_t)0x90)
#define NAND_CMD_STATUS            ((uint8_t)0x70)
#define NAND_CMD_LOCK_STATUS       ((uint8_t)0x7A)
#define NAND_CMD_RESET             ((uint8_t)0xFF)

/** 
  * @brief  NAND memory status  
  */  
#define NAND_VALID_ADDRESS         ((uint32_t)0x00000100)
#define NAND_INVALID_ADDRESS       ((uint32_t)0x00000200)
#define NAND_TIMEOUT_ERROR         ((uint32_t)0x00000400)
#define NAND_BUSY                  ((uint32_t)0x00000000)
#define NAND_ERROR                 ((uint32_t)0x00000001)
#define NAND_READY                 ((uint32_t)0x00000040)

/** 
  * @brief  XMC NAND memory parameters  
  */  
#define NAND_PAGE_SIZE             ((uint16_t)0x0800) /* 2048 bytes per page w/o Spare Area */
#define NAND_BLOCK_SIZE            ((uint16_t)0x0040) /* 64x2048 bytes pages per block */
#define NAND_ZONE_SIZE             ((uint16_t)0x0400) /* 1024 Block per zone */
#define NAND_SPARE_AREA_SIZE       ((uint16_t)0x0040) /* last 64 bytes as spare area */
#define NAND_MAX_ZONE              ((uint16_t)0x0001) /* 1 zones of 1024 block */

/** 
  * @brief  XMC NAND memory address computation  
  */  
#define ADDR_1st_CYCLE(ADDR)       (uint8_t)((ADDR)& 0xFF)               /* 1st addressing cycle */
#define ADDR_2nd_CYCLE(ADDR)       (uint8_t)(((ADDR)& 0xFF00) >> 8)      /* 2nd addressing cycle */
#define ADDR_3rd_CYCLE(ADDR)       (uint8_t)(((ADDR)& 0xFF0000) >> 16)   /* 3rd addressing cycle */
#define ADDR_4th_CYCLE(ADDR)       (uint8_t)(((ADDR)& 0xFF000000) >> 24) /* 4th addressing cycle */   
/**
  * @}
  */ 


void NAND_Init(void);
void NAND_ReadID(NAND_IDTypeDef* NAND_ID);
uint32_t NAND_WriteSmallPage(uint8_t *pBuffer, NAND_ADDRESS Address, uint32_t NumPageToWrite);
uint32_t NAND_ReadSmallPage (uint8_t *pBuffer, NAND_ADDRESS Address, uint32_t NumPageToRead);
uint32_t NAND_WriteSpareArea(uint8_t *pBuffer, NAND_ADDRESS Address, uint32_t NumSpareAreaTowrite);
uint32_t NAND_ReadSpareArea(uint8_t *pBuffer, NAND_ADDRESS Address, uint32_t NumSpareAreaToRead);
uint32_t NAND_EraseBlock(NAND_ADDRESS Address);
uint32_t NAND_Reset(void);
uint32_t NAND_GetStatus(void);
uint32_t NAND_ReadStatus(void);
uint32_t NAND_AddressIncrement(NAND_ADDRESS* Address);

#ifdef __cplusplus
}
#endif

#endif 
/**
  * @}
  */
