 /**
  ******************************************************************************
  * File   : at_spi_flash.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : SPI flash config and setup header
  ******************************************************************************
  */
#ifndef __AT_SPI_FLASH_H
#define __AT_SPI_FLASH_H
#include "at32f4xx.h"

#define FLASH_SPI                   SPI2
#define FLASH_SPI_CLK               RCC_APB1PERIPH_SPI2

/*SCK Pin*/
#define FLASH_SPI_SCK_PIN           GPIO_Pins_13
#define FLASH_SPI_SCK_GPIO          GPIOB
#define FLASH_SPI_SCK_GPIO_CLK      RCC_APB2PERIPH_GPIOB

/*MISO Pin*/
#define FLASH_SPI_MISO_PIN          GPIO_Pins_14
#define FLASH_SPI_MISO_GPIO         GPIOB
#define FLASH_SPI_MISO_GPIO_CLK     RCC_APB2PERIPH_GPIOB

/*MOSI Pin*/
#define FLASH_SPI_MOSI_PIN          GPIO_Pins_15
#define FLASH_SPI_MOSI_GPIO         GPIOB
#define FLASH_SPI_MOSI_GPIO_CLK     RCC_APB2PERIPH_GPIOB

/*CS Pin*/
#define FLASH_SPI_CS_PIN                GPIO_Pins_12
#define FLASH_SPI_CS_GPIO           GPIOB
#define FLASH_SPI_CS_GPIO_CLK       RCC_APB2PERIPH_GPIOB


/*FLASH instruction, 不同FLASH 指令不同,目前为Demo Flash 型号为W25Q128FV */
#define  FLASH_GETID              0x90
#define  FLASH_CMD_WREN           0x06  /*!< Write enable instruction */
#define  FLASH_CMD_ERASE_SECTOR	  0x20  /*!< Sector Erase instruction */
#define  FLASH_CMD_READSTS	      0x05  /*!< Read Status Register instruction */
#define  FLASH_CMD_WRITEPAGE	    0x02  /*!< Program Page Register instruction */
#define  FLASH_CMD_READPAGE	      0x03  /*!< Read Page Register instruction */


void Flash_spi_init(void);
void FLASH_SPI_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void FLASH_SPI_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
uint32_t FLASH_SPI_GetSize(void);
uint32_t FLASH_SPI_SectorSize(void);
uint32_t FLASH_SPI_PageSize(void);
uint32_t FLASH_SPI_GetID(void);
         
#endif

