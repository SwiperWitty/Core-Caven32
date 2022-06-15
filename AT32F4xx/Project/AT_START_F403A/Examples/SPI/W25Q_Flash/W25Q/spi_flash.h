/**
  ******************************************************************************
  * File   : SPI/SPI_W25Q_Flash/spi_flash.h 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : W25Q flash header file
  ******************************************************************************
  */ 

#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

#include "at32f4xx.h"

/* Private define ------------------------------------------------------------*/
/* SPI define ----------------------------------------------------------------*/
/*
 * SPI_MODE 
 * 0 --> DMA
 * 1 --> pooling
 */
#define SPI_MODE                1

#define BUF_SIZE                SPIF_PAGE_SIZE
#define FLASH_SPI               SPI1
#define SPIx_RCC_CLK            RCC_APB2PERIPH_SPI1
#define SPIx_GPIO_RCC_CLK       RCC_APB2PERIPH_GPIOA 
#define SPIx_PIN_NSS            GPIO_Pins_4
#define SPIx_PORT_NSS           GPIOA
#define SPIx_PIN_SCK            GPIO_Pins_5
#define SPIx_PORT_SCK           GPIOA
#define SPIx_PIN_MISO           GPIO_Pins_6
#define SPIx_PORT_MISO          GPIOA
#define SPIx_PIN_MOSI           GPIO_Pins_7
#define SPIx_PORT_MOSI          GPIOA

#define SPIx_DMA                DMA1
#define SPIx_DMA_RCC_CLK        RCC_AHBPERIPH_DMA1
#define SPIx_Rx_DMA_Channel     DMA1_Channel2
#define SPIx_Rx_DMA_FLAG        DMA1_FLAG_TC2
#define SPIx_Tx_DMA_Channel     DMA1_Channel3
#define SPIx_Tx_DMA_FLAG        DMA1_FLAG_TC3

#define FLASH_CS_HIGH()         GPIO_SetBits(SPIx_PORT_NSS, SPIx_PIN_NSS)
#define FLASH_CS_LOW()          GPIO_ResetBits(SPIx_PORT_NSS, SPIx_PIN_NSS)

/* Flash define ---------------------------------------------------------------*/
#define W25Q80  0xEF13  
#define W25Q16  0xEF14
#define W25Q32  0xEF15
#define W25Q64  0xEF16
#define W25Q128 0xEF17   /* 16MB, the range of address:0~0xFFFF FF */

#define SPIF_CHIP_SIZE          0x1000000
#define SPIF_SECTOR_SIZE        4096
#define SPIF_PAGE_SIZE          256

#define SPIF_WriteEnable        0x06 
#define SPIF_WriteDisable       0x04 
#define SPIF_ReadStatusReg1     0x05 /* S7-S0 */
#define SPIF_WriteStatusReg1    0x01 
#define SPIF_ReadStatusReg2     0x35 /* S15-S8 */
#define SPIF_WriteStatusReg2    0x31 
#define SPIF_ReadStatusReg3     0x15 /* S23-S16 */
#define SPIF_WriteStatusReg3    0x11 
#define SPIF_ReadData           0x03 
#define SPIF_FastReadData       0x0B 
#define SPIF_FastReadDual       0x3B 
#define SPIF_PageProgram        0x02 
#define SPIF_BlockErase         0xD8 /* Block Size:64KB */
#define SPIF_SectorErase        0x20 
#define SPIF_ChipErase          0xC7 
#define SPIF_PowerDown          0xB9 
#define SPIF_ReleasePowerDown   0xAB 
#define SPIF_DeviceID           0xAB 
#define SPIF_ManufactDeviceID   0x90 
#define SPIF_JedecDeviceID      0x9F 
#define FLASH_SPI_DUMMY_BYTE    0xA5

/* Private function prototypes -----------------------------------------------*/
uint8_t SpiFlash_Init(void);
uint16_t SpiFlash_ReadID(void);
void SpiFlash_Erase_Sector(uint32_t dwDstAddr);             
void SpiFlash_Write_Page(uint8_t* pbBuffer,uint32_t dwWriteAddr,uint32_t dwNumByteToWrite);     
void SpiFlash_Read(uint8_t* pbBuffer,uint32_t dwReadAddr,uint32_t dwNumByteToRead);
void SpiFlash_Write(uint8_t* pbBuffer,uint32_t dwWriteAddr,uint32_t dwNumByteToWrite); 
void SpiFlash_Write_NoCheck(uint8_t* pbBuffer,uint32_t dwWriteAddr,uint32_t dwNumByteToWrite);   
static uint8_t SPI_WriteByte(uint8_t bWriteValue);    
static uint8_t SPI_ReadByte(void);  
static void SpiFlash_Write_Enable(void);
static void SpiFlash_Wait_Busy(void);      
static uint8_t SpiFlash_ReadSR1(void);    
static void SPI_ReadBytes(uint8_t *pbBuffer,uint32_t dwNumByteToRead);
static void SPI_WriteBytes(uint8_t* pbBuffer ,uint32_t dwNumByteToWrite);

#endif

