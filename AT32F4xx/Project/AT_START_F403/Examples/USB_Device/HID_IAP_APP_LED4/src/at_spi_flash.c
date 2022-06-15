 /**
  ******************************************************************************
  * File   : at_spi_flash.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : SPI flash config and setup
  ******************************************************************************
  */
#include "at_spi_flash.h"
#include "at32f4xx_spi.h"

/** @addtogroup AT32F403A_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_HID_APP_Demo
  * @{
  */ 
  
/**
  * @brief  Select FLASH: Chip Select pin low
  */
#define FLASH_SPI_CS_LOW()       GPIO_ResetBits(FLASH_SPI_CS_GPIO, FLASH_SPI_CS_PIN)
/**
  * @brief  Deselect FLASH: Chip Select pin high
  */
#define FLASH_SPI_CS_HIGH()      GPIO_SetBits(FLASH_SPI_CS_GPIO, FLASH_SPI_CS_PIN)   


#define FLASH_DUMMY_BYTE    0xFF
#define FLAH_PAGE_SIZE      256
#define FLAH_SECTOR_SIZE      0x1000


uint8_t FLASH_SPI_ReadByte(void);
uint8_t FLASH_SPI_SendByte(uint8_t byte);
void FLASH_SPI_WriteEnable(void);
void FLASH_SPI_EraseSector(uint32_t SectorAddr);
void FLASH_SPI_WaitForWriteEnd(void);
void FLASH_SPI_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
uint32_t SPI_Flash_Size = 0x1000000;
uint32_t SPI_Flash_SectorSize = FLAH_SECTOR_SIZE;
uint32_t Device_ID = 0;

//uint8_t tmp_buffer[1024*16];
/**
  * @brief  Initializes the peripherals used by the SPI FLASH driver.
  * @param  None
  * @retval None
  */
 void Flash_spi_init(void)
 {
    GPIO_InitType GPIO_InitStructure;
    SPI_InitType SPI_InitStruct;
     
     /*RCC Config*/
    RCC_APB2PeriphClockCmd(FLASH_SPI_SCK_GPIO_CLK | FLASH_SPI_MISO_GPIO_CLK | 
                        FLASH_SPI_MOSI_GPIO_CLK | FLASH_SPI_CS_GPIO_CLK, ENABLE);
    RCC_APB1PeriphClockCmd (FLASH_SPI_CLK, ENABLE);
    /*Configure SPI SCK pin*/
    GPIO_InitStructure.GPIO_Pins  = FLASH_SPI_SCK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
    GPIO_Init(FLASH_SPI_SCK_GPIO, &GPIO_InitStructure);

     /*Configure SPI MISO pin*/
    GPIO_InitStructure.GPIO_Pins  = FLASH_SPI_MISO_PIN;
    GPIO_Init(FLASH_SPI_MISO_GPIO, &GPIO_InitStructure);
     
    /*Configure SPI MOSI pin*/
    GPIO_InitStructure.GPIO_Pins  = FLASH_SPI_MOSI_PIN;
    GPIO_Init(FLASH_SPI_MOSI_GPIO, &GPIO_InitStructure);
     
     /*Configure SPI CS pin*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
    GPIO_InitStructure.GPIO_Pins  = FLASH_SPI_CS_PIN;
    GPIO_Init(FLASH_SPI_CS_GPIO, &GPIO_InitStructure);
    
    /*Deselect FLASH*/
    FLASH_SPI_CS_HIGH();
    
    /*SPI Configuration*/
    SPI_InitStruct.SPI_TransMode = SPI_TRANSMODE_FULLDUPLEX;
    SPI_InitStruct.SPI_Mode = SPI_MODE_MASTER;
    SPI_InitStruct.SPI_FrameSize = SPI_FRAMESIZE_8BIT;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_HIGH;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_2EDGE;
    SPI_InitStruct.SPI_NSSSEL = SPI_NSSSEL_SOFT;
    SPI_InitStruct.SPI_MCLKP = SPI_MCLKP_4;
    SPI_InitStruct.SPI_FirstBit = SPI_FIRSTBIT_MSB;
    SPI_InitStruct.SPI_CPOLY = 7;
    SPI_Init(FLASH_SPI, &SPI_InitStruct);
    
    /*Enable SPI*/
    SPI_Enable(FLASH_SPI, ENABLE);
    
    Device_ID = FLASH_SPI_GetID();
    if ( Device_ID == 0xEF16 )
    {
        SPI_Flash_Size = 0x800000; /*8M byte*/
    }else if ( Device_ID == 0xEF17 )
    {
        SPI_Flash_Size = 0x1000000; /*16M byte*/
    }else
    {
        SPI_Flash_Size = 0;
    }
     
 }
 
 
/**
  * @brief  Reads a byte from the SPI Flash.
  * @note   This function must be used only if the Start_Read_Sequence function
  *         has been previously called.
  * @param  None
  * @retval Byte Read from the SPI Flash.
  */
uint8_t FLASH_SPI_ReadByte(void)
{
  return (FLASH_SPI_SendByte(FLASH_DUMMY_BYTE));
}

/**
  * @brief  Sends a byte through the SPI interface and return the byte received
  *         from the SPI bus.
  * @param  byte: byte to send.
  * @retval The value of the received byte.
  */
uint8_t FLASH_SPI_SendByte(uint8_t byte)
{
  /*!< Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(FLASH_SPI, SPI_I2S_FLAG_TE) == RESET);

  /*!< Send byte through the SPI1 peripheral */
  SPI_I2S_TxData(FLASH_SPI, byte);

  /*!< Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(FLASH_SPI, SPI_I2S_FLAG_RNE) == RESET);

  /*!< Return the byte read from the SPI bus */
  return SPI_I2S_RxData(FLASH_SPI);
}

/**
  * @brief  Enables the write access to the FLASH.
  * @param  None
  * @retval None
  */
void FLASH_SPI_WriteEnable(void)
{
  /*!< Select the FLASH: Chip Select low */
  FLASH_SPI_CS_LOW();

  /*!< Send "Write Enable" instruction */
  FLASH_SPI_SendByte(FLASH_CMD_WREN);

  /*!< Deselect the FLASH: Chip Select high */
  FLASH_SPI_CS_HIGH();
}

/**
  * @brief  Erases the specified FLASH sector.
  * @param  SectorAddr: address of the sector to erase.
  * @retval None
  */
void FLASH_SPI_EraseSector(uint32_t SectorAddr)
{
    /*!< Send write enable instruction */
    FLASH_SPI_WriteEnable();

    /*!< Sector Erase */
    /*!< Select the FLASH: Chip Select low */
    FLASH_SPI_CS_LOW();
    /*!< Send Sector Erase instruction */
    FLASH_SPI_SendByte(FLASH_CMD_ERASE_SECTOR);
    /*!< Send SectorAddr high nibble address byte */
    FLASH_SPI_SendByte((SectorAddr & 0xFF0000) >> 16);
    /*!< Send SectorAddr medium nibble address byte */
    FLASH_SPI_SendByte((SectorAddr & 0xFF00) >> 8);
    /*!< Send SectorAddr low nibble address byte */
    FLASH_SPI_SendByte(SectorAddr & 0xFF);
    /*!< Deselect the FLASH: Chip Select high */
    FLASH_SPI_CS_HIGH();

    /*!< Wait the end of Flash writing */
    FLASH_SPI_WaitForWriteEnd();
}


/**
  * @brief  Polls the status of the Write In Progress (WIP) flag in the FLASH's
  *         status register and loop until write opertaion has completed.
  * @param  None
  * @retval None
  */
void FLASH_SPI_WaitForWriteEnd(void)
{
    uint8_t flashstatus = 0;

    /*!< Select the FLASH: Chip Select low */
    FLASH_SPI_CS_LOW();

    /*!< Send "Read Status Register" instruction */
    FLASH_SPI_SendByte(FLASH_CMD_READSTS);

    /*!< Loop as long as the memory is busy with a write cycle */
    do
    {
       /*!< Send a dummy byte to generate the clock needed by the FLASH
        and put the value of the status register in FLASH_Status variable */
        flashstatus = FLASH_SPI_SendByte(FLASH_DUMMY_BYTE);

    }
    while ((flashstatus & 0x1) == SET); /* Write in progress */

    /*!< Deselect the FLASH: Chip Select high */
    FLASH_SPI_CS_HIGH();
}

/**
  * @brief  Writes more than one byte to the FLASH with a single WRITE cycle 
  *         (Page WRITE sequence).
  * @note   The number of byte can't exceed the FLASH page size.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *         to the FLASH.
  * @param  WriteAddr: FLASH's internal address to write to.
  * @param  NumByteToWrite: number of bytes to write to the FLASH, must be equal
  *         or less than "sFLASH_PAGESIZE" value.
  * @retval None
  */
void FLASH_SPI_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  /*!< Enable the write access to the FLASH */
  FLASH_SPI_WriteEnable();

  /*!< Select the FLASH: Chip Select low */
  FLASH_SPI_CS_LOW();
  /*!< Send "Write to Memory " instruction */
  FLASH_SPI_SendByte(FLASH_CMD_WRITEPAGE);
  /*!< Send WriteAddr high nibble address byte to write to */
  FLASH_SPI_SendByte((WriteAddr & 0xFF0000) >> 16);
  /*!< Send WriteAddr medium nibble address byte to write to */
  FLASH_SPI_SendByte((WriteAddr & 0xFF00) >> 8);
  /*!< Send WriteAddr low nibble address byte to write to */
  FLASH_SPI_SendByte(WriteAddr & 0xFF);

  /*!< while there is data to be written on the FLASH */
  while (NumByteToWrite--)
  {
    /*!< Send the current byte */
    FLASH_SPI_SendByte(*pBuffer);
    /*!< Point on the next byte to be written */
    pBuffer++;
  }

  /*!< Deselect the FLASH: Chip Select high */
  FLASH_SPI_CS_HIGH();

  /*!< Wait the end of Flash writing */
  FLASH_SPI_WaitForWriteEnd();
}


/**
  * @brief  Writes block of data to the FLASH. In this function, the number of
  *         WRITE cycles are reduced, using Page WRITE sequence.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *         to the FLASH.
  * @param  WriteAddr: FLASH's internal address to write to.
  * @param  NumByteToWrite: number of bytes to write to the FLASH.
  * @retval None
  */
void FLASH_SPI_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint32_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % FLAH_PAGE_SIZE;
  count = FLAH_PAGE_SIZE - Addr;
  NumOfPage =  NumByteToWrite / FLAH_PAGE_SIZE;
  NumOfSingle = NumByteToWrite % FLAH_PAGE_SIZE;

  if (Addr == 0) /*!< WriteAddr is FLAH_PAGE_SIZE aligned  */
  {
     /*need Erase Sector*/
    if ( (WriteAddr & 0xFFF) == 0 )
        FLASH_SPI_EraseSector(WriteAddr);
    if (NumOfPage == 0) /*!< NumByteToWrite < FLAH_PAGE_SIZE */
    {
      FLASH_SPI_WritePage(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /*!< NumByteToWrite > FLAH_PAGE_SIZE */
    {
      while (NumOfPage--)
      {
        FLASH_SPI_WritePage(pBuffer, WriteAddr, FLAH_PAGE_SIZE);
        WriteAddr +=  FLAH_PAGE_SIZE;
        pBuffer += FLAH_PAGE_SIZE;
      }
      /*Erase Sector*/
//      FLASH_SPI_EraseSector(WriteAddr);
      FLASH_SPI_WritePage(pBuffer, WriteAddr, NumOfSingle);
    }
  }
  else /*!< WriteAddr is not FLAH_PAGE_SIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < FLAH_PAGE_SIZE */
    {
      if (NumOfSingle > count) /*!< (NumByteToWrite + WriteAddr) > FLAH_PAGE_SIZE */
      {
        temp = NumOfSingle - count;

        FLASH_SPI_WritePage(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;

        FLASH_SPI_WritePage(pBuffer, WriteAddr, temp);
      }
      else
      {
        FLASH_SPI_WritePage(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /*!< NumByteToWrite > FLAH_PAGE_SIZE */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / FLAH_PAGE_SIZE;
      NumOfSingle = NumByteToWrite % FLAH_PAGE_SIZE;

      FLASH_SPI_WritePage(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;

      while (NumOfPage--)
      {
        FLASH_SPI_WritePage(pBuffer, WriteAddr, FLAH_PAGE_SIZE);
        WriteAddr +=  FLAH_PAGE_SIZE;
        pBuffer += FLAH_PAGE_SIZE;
      }

      if (NumOfSingle != 0)
      {
        FLASH_SPI_WritePage(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}


/**
  * @brief  Reads a block of data from the FLASH.
  * @param  pBuffer: pointer to the buffer that receives the data read from the FLASH.
  * @param  ReadAddr: FLASH's internal address to read from.
  * @param  NumByteToRead: number of bytes to read from the FLASH.
  * @retval None
  */
void FLASH_SPI_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
  /*!< Select the FLASH: Chip Select low */
  FLASH_SPI_CS_LOW();

  /*!< Send "Read from Memory " instruction */
  FLASH_SPI_SendByte(FLASH_CMD_READPAGE);

  /*!< Send ReadAddr high nibble address byte to read from */
  FLASH_SPI_SendByte((ReadAddr & 0xFF0000) >> 16);
  /*!< Send ReadAddr medium nibble address byte to read from */
  FLASH_SPI_SendByte((ReadAddr& 0xFF00) >> 8);
  /*!< Send ReadAddr low nibble address byte to read from */
  FLASH_SPI_SendByte(ReadAddr & 0xFF);

  while (NumByteToRead--) /*!< while there is data to be read */
  {
    /*!< Read a byte from the FLASH */
    *pBuffer = FLASH_SPI_SendByte(FLASH_DUMMY_BYTE);
    /*!< Point to the next location where the byte read will be saved */
    pBuffer++;
  }

  /*!< Deselect the FLASH: Chip Select high */
  FLASH_SPI_CS_HIGH();
}
/**
  * @brief  Reads FLASH identification.
  * @param  None
  * @retval FLASH identification
  */
uint32_t FLASH_SPI_GetID(void)
{
    /*用户可根据不同FLASH采用不同的指令获取ID*/
    uint16_t flash_id = 0;
    FLASH_SPI_CS_LOW();
    FLASH_SPI_SendByte(FLASH_GETID);
    FLASH_SPI_SendByte(0x00);
    FLASH_SPI_SendByte(0x00);
    FLASH_SPI_SendByte(0x00);
    flash_id |= FLASH_SPI_ReadByte() << 8;
    flash_id |= FLASH_SPI_ReadByte();
    FLASH_SPI_CS_HIGH();
    return flash_id;
}


/**
  * @brief  Get FLASH SIZE.
  * @param  None
  * @retval FLASH identification
  */
uint32_t FLASH_SPI_GetSize(void)
{
    return SPI_Flash_Size; 
}


/**
  * @brief  Get FLASH Sector SIZE.
  * @param  None
  * @retval FLASH identification
  */
uint32_t FLASH_SPI_SectorSize(void)
{
    return SPI_Flash_SectorSize; /*4K*/
}

/**
  * @brief  Get FLASH Page SIZE.
  * @param  None
  * @retval FLASH identification
  */
uint32_t FLASH_SPI_PageSize(void)
{
    //return FLAH_PAGE_SIZE; /*256Byte*/
    return 4096; /*可根据代码配置USB Buffer 一次可以接受多少数据， 最大4K*/
}

/**
  * @}
  */ 

/**
  * @}
  */




