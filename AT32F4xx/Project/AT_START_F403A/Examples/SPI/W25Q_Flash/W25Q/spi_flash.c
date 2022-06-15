/**
  ******************************************************************************
  * File   : SPI/SPI_W25Q_Flash/spi_flash.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : W25Q flash source code
  ******************************************************************************
  */ 

#include "at32f4xx.h"
#include "spi_flash.h"

uint8_t SpiFlash_SectorBuf[SPIF_SECTOR_SIZE];/* a sector size */	

uint8_t SpiFlash_Init(void)
{
  GPIO_InitType GPIO_InitStructure;
  SPI_InitType  SPI_InitStructure;

  RCC_APB2PeriphClockCmd(SPIx_RCC_CLK | SPIx_GPIO_RCC_CLK, ENABLE);
  RCC_AHBPeriphClockCmd(SPIx_DMA_RCC_CLK, ENABLE);

  /* Configure SPI_FLASH pins*/
  GPIO_InitStructure.GPIO_Pins =  SPIx_PIN_MOSI;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_10MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(SPIx_PORT_MOSI, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pins = SPIx_PIN_MISO;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_10MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(SPIx_PORT_MISO, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pins = SPIx_PIN_NSS;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_10MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
  GPIO_Init(SPIx_PORT_NSS, &GPIO_InitStructure);  

  GPIO_InitStructure.GPIO_Pins =  SPIx_PIN_SCK;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_10MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(SPIx_PORT_SCK, &GPIO_InitStructure);

  FLASH_CS_HIGH();
  /* SPI_FLASH configuration ------------------------------------------------------*/
  SPI_InitStructure.SPI_TransMode = SPI_TRANSMODE_FULLDUPLEX;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2EDGE;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_HIGH;
  SPI_InitStructure.SPI_CPOLY = 0;
  SPI_InitStructure.SPI_FirstBit = SPI_FIRSTBIT_MSB;
  SPI_InitStructure.SPI_FrameSize = SPI_FRAMESIZE_8BIT;
  SPI_InitStructure.SPI_MCLKP = SPI_MCLKP_8;
  SPI_InitStructure.SPI_NSSSEL = SPI_NSSSEL_SOFT;
  SPI_InitStructure.SPI_Mode = SPI_MODE_MASTER;
  SPI_Init(FLASH_SPI, &SPI_InitStructure);

  /* Enable SPI module */
  SPI_Enable(FLASH_SPI, ENABLE);

  return 1; 
}

/**
  * @brief  Write data to flash
  * @param  pbBuffer: buffer name
  * @param  dwWriteAddr: buffer address 
  * @param  dwNumByteToWrite: buffer length 
  * @retval none
  */
void SpiFlash_Write(uint8_t* pbBuffer, uint32_t dwWriteAddr, uint32_t dwNumByteToWrite)   
{ 
  uint32_t dwSectorPos;
  uint16_t wSectorOffset;
  uint16_t wSectorRemain;	   
 	uint16_t i;    
  uint8_t * SpiFlash_BUF;	  
  SpiFlash_BUF = SpiFlash_SectorBuf;	     
 	dwSectorPos = dwWriteAddr / SPIF_SECTOR_SIZE; // sector address  
  wSectorOffset = dwWriteAddr % SPIF_SECTOR_SIZE; // address offset in a sector
  wSectorRemain = SPIF_SECTOR_SIZE - wSectorOffset; // the remain in a sector 
 	if(dwNumByteToWrite <= wSectorRemain)
  {
    wSectorRemain = dwNumByteToWrite; // smaller than a sector size
  }
  while(1) 
  {	
    SpiFlash_Read(SpiFlash_BUF, dwSectorPos * SPIF_SECTOR_SIZE, SPIF_SECTOR_SIZE); // read a sector
    for(i=0; i<wSectorRemain; i++) // validate the read erea
    {
      if(SpiFlash_BUF[wSectorOffset + i]!=0xFF)
      {
         break; //t here are some data not equal 0xFF, so this secotr needs erased
      }
    }
    if(i < wSectorRemain) // need erase
    {
      SpiFlash_Erase_Sector(dwSectorPos); // erase the sector
      for(i=0; i<wSectorRemain; i++) // copy the write data
      {
        SpiFlash_BUF[i+wSectorOffset] = pbBuffer[i];	  
      }
      SpiFlash_Write_NoCheck(SpiFlash_BUF, dwSectorPos * SPIF_SECTOR_SIZE, SPIF_SECTOR_SIZE); // program the sector
    }
    else 
    {
      SpiFlash_Write_NoCheck(pbBuffer, dwWriteAddr, wSectorRemain); // write directly in the erased area                  
    }
    if(dwNumByteToWrite == wSectorRemain)
    {
      break; // write end
    }
    else // go on writing
    {
      dwSectorPos++;
      wSectorOffset = 0;	 

      pbBuffer += wSectorRemain;  				
      dwWriteAddr += wSectorRemain;				   
      dwNumByteToWrite -= wSectorRemain;			
      if(dwNumByteToWrite > SPIF_SECTOR_SIZE)
      {
        wSectorRemain = SPIF_SECTOR_SIZE; // Could not write the remain data in the next sector
      }
      else
      {
        wSectorRemain = dwNumByteToWrite; // Could write the remain data in the next sector
      }
    }	 
  }
}

/**
  * @brief  Read data from flash
  * @param  pbBuffer: buffer name
  * @param  dwReadAddr: buffer address
  * @param  dwNumByteToRead: buffer length
  * @retval none
  */
void SpiFlash_Read(uint8_t* pbBuffer, uint32_t dwReadAddr, uint32_t dwNumByteToRead)
{
  FLASH_CS_LOW();
  SPI_WriteByte(SPIF_ReadData); // send instruction
  SPI_WriteByte((uint8_t)((dwReadAddr) >> 16)); //send 24-bit address
  SPI_WriteByte((uint8_t)((dwReadAddr) >> 8));
  SPI_WriteByte((uint8_t)dwReadAddr);
  SPI_ReadBytes(pbBuffer,dwNumByteToRead);
  FLASH_CS_HIGH();
}

/**
  * @brief  Erase a sector data
  * @param  dwDstAddr: Sector address to erase
  * @retval none
  */
void SpiFlash_Erase_Sector(uint32_t dwDstAddr)
{
 	dwDstAddr*=SPIF_SECTOR_SIZE; // translate sector address to byte address
  SpiFlash_Write_Enable();
  SpiFlash_Wait_Busy();
  FLASH_CS_LOW();
  SPI_WriteByte(SPIF_SectorErase);
  SPI_WriteByte((uint8_t)((dwDstAddr) >> 16));
  SPI_WriteByte((uint8_t)((dwDstAddr) >> 8));
  SPI_WriteByte((uint8_t)dwDstAddr);
  FLASH_CS_HIGH();
  SpiFlash_Wait_Busy();
}

/**
  * @brief  Write data without check 
  * @param  pbBuffer: buffer name
  * @param  dwWriteAddr: buffer address 
  * @param  dwNumByteToWrite: buffer length 
  * @retval none
  */
void SpiFlash_Write_NoCheck(uint8_t* pbBuffer, uint32_t dwWriteAddr, uint32_t dwNumByteToWrite)   
{
  uint16_t wPageRemain;
  wPageRemain=SPIF_PAGE_SIZE - dwWriteAddr % SPIF_PAGE_SIZE; // remain bytes in a page
  if(dwNumByteToWrite<=wPageRemain)
  {
    wPageRemain=dwNumByteToWrite;// smaller than a page size
  }
	while(1)
	{
    SpiFlash_Write_Page(pbBuffer, dwWriteAddr, wPageRemain);
		if(dwNumByteToWrite==wPageRemain)
    {
      break; // all data are programmed 
    }
	 	else // NumByteToWrite>pageremain
		{
      pbBuffer += wPageRemain;
      dwWriteAddr += wPageRemain;
      dwNumByteToWrite -= wPageRemain; // the remain bytes to be prorammed
      if(dwNumByteToWrite > SPIF_PAGE_SIZE)
      {
        wPageRemain = SPIF_PAGE_SIZE; // can be progrmmed a page at a time
      }
      else
      {
        wPageRemain = dwNumByteToWrite; // smaller than a page size
      }
		}
	}
}

/**
  * @brief  Write a page data
  * @param  pbBuffer: buffer name
  * @param  dwWriteAddr: buffer address 
  * @param  dwNumByteToWrite: buffer length
  * @retval none
  */
void SpiFlash_Write_Page(uint8_t* pbBuffer, uint32_t dwWriteAddr, uint32_t dwNumByteToWrite)
{
  if((0 < dwNumByteToWrite) && (dwNumByteToWrite <= SPIF_PAGE_SIZE))
  {
    SpiFlash_Write_Enable(); // SET WEL
    FLASH_CS_LOW();
    SPI_WriteByte(SPIF_PageProgram); // send instruction
    SPI_WriteByte((uint8_t)((dwWriteAddr) >> 16)); // send 24-bit address
    SPI_WriteByte((uint8_t)((dwWriteAddr) >> 8));
    SPI_WriteByte((uint8_t)dwWriteAddr);
    SPI_WriteBytes(pbBuffer,dwNumByteToWrite);
    FLASH_CS_HIGH();
    SpiFlash_Wait_Busy(); // Wait for program end
  }
}

/**
  * @brief  Write data continuously
  * @param  pbBuffer: buffer name
  * @param  dwNumByteToWrite: buffer length
  * @retval none
  */
static void SPI_WriteBytes(uint8_t* pbBuffer, uint32_t dwNumByteToWrite)
{
  volatile uint8_t dummy_data;

#if (SPI_MODE == 0)
  DMA_InitType DMA_InitStructure;

  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&FLASH_SPI->DT);
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&dummy_data;
  DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALSRC;
  DMA_InitStructure.DMA_BufferSize = dwNumByteToWrite;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;
  DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_DISABLE;
  DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_BYTE;
  DMA_InitStructure.DMA_MemoryDataWidth = DMA_MEMORYDATAWIDTH_BYTE;
  DMA_InitStructure.DMA_Mode = DMA_MODE_NORMAL;
  DMA_InitStructure.DMA_Priority = DMA_PRIORITY_VERYHIGH;
  DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;
  DMA_Init(SPIx_Rx_DMA_Channel, &DMA_InitStructure);

  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&FLASH_SPI->DT);
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pbBuffer;
  DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALDST;
  DMA_InitStructure.DMA_BufferSize = dwNumByteToWrite;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;
  DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE;
  DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_BYTE;
  DMA_InitStructure.DMA_MemoryDataWidth = DMA_MEMORYDATAWIDTH_BYTE;
  DMA_InitStructure.DMA_Mode = DMA_MODE_NORMAL;
  DMA_InitStructure.DMA_Priority = DMA_PRIORITY_VERYHIGH;
  DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;
  DMA_Init(SPIx_Tx_DMA_Channel, &DMA_InitStructure);

  SPI_I2S_DMAEnable(FLASH_SPI, SPI_I2S_DMA_TX,ENABLE);
  SPI_I2S_DMAEnable(FLASH_SPI, SPI_I2S_DMA_RX,ENABLE);

  DMA_ChannelEnable(SPIx_Rx_DMA_Channel, ENABLE);
  DMA_ChannelEnable(SPIx_Tx_DMA_Channel, ENABLE);

  while(DMA_GetFlagStatus(SPIx_Rx_DMA_FLAG) != SET);
  DMA_ClearFlag(SPIx_Rx_DMA_FLAG);

  DMA_ChannelEnable(SPIx_Tx_DMA_Channel, DISABLE);
  DMA_ChannelEnable(SPIx_Rx_DMA_Channel, DISABLE);
  SPI_I2S_DMAEnable(FLASH_SPI, SPI_I2S_DMA_RX, DISABLE);
  SPI_I2S_DMAEnable(FLASH_SPI, SPI_I2S_DMA_TX, DISABLE);
#else

  while(dwNumByteToWrite--)
  {
    while(SPI_I2S_GetFlagStatus(FLASH_SPI, SPI_I2S_FLAG_TE) == RESET);
    SPI_I2S_TxData(FLASH_SPI, *pbBuffer);
    while(SPI_I2S_GetFlagStatus(FLASH_SPI, SPI_I2S_FLAG_RNE) == RESET);
    dummy_data = SPI_I2S_RxData(FLASH_SPI);
    pbBuffer ++;
  }
#endif
}

/**
  * @brief  Wait program done
  * @param  none
  * @retval none
  */
static void SpiFlash_Wait_Busy(void)
{
	while((SpiFlash_ReadSR1()&0x01) == 0x01);
} 

/**
  * @brief  Read SR1 register
  * @param  none
  * @retval none
  */
static uint8_t SpiFlash_ReadSR1(void)
{
  uint8_t bReadByte=0;
  FLASH_CS_LOW();
  SPI_WriteByte(SPIF_ReadStatusReg1);
  bReadByte=(uint8_t)SPI_ReadByte();
  FLASH_CS_HIGH();
	return (bReadByte);
}

/**
  * @brief  Enable write operation
  * @param  none
  * @retval none
  */
static void SpiFlash_Write_Enable(void)   
{
  FLASH_CS_LOW();
  SPI_WriteByte(SPIF_WriteEnable);
  FLASH_CS_HIGH();
}

/**
  * @brief  Read device ID
  * @param  none
  * @retval device ID
  */
uint16_t SpiFlash_ReadID(void)
{
	uint16_t wReceiveData = 0;
  FLASH_CS_LOW();
  SPI_WriteByte(SPIF_ManufactDeviceID);
  SPI_WriteByte(0x00);
  SPI_WriteByte(0x00);
  SPI_WriteByte(0x00);
  wReceiveData|=SPI_ReadByte() << 8;
  wReceiveData|=SPI_ReadByte();
  FLASH_CS_HIGH();
	return wReceiveData;
}  

/**
  * @brief  Write a byte to flash
  * @param  bWriteValue: Data to write
  * @retval Flash return data
  */
static uint8_t SPI_WriteByte(uint8_t bWriteValue)
{
  uint8_t bRxBuff;
  SPI_I2S_DMAEnable(FLASH_SPI, SPI_I2S_DMA_TX, DISABLE);
  SPI_I2S_DMAEnable(FLASH_SPI, SPI_I2S_DMA_RX, DISABLE);
  SPI_I2S_TxData(FLASH_SPI, bWriteValue);
  while(!SPI_I2S_GetFlagStatus(FLASH_SPI, SPI_I2S_FLAG_RNE));
  bRxBuff = SPI_I2S_RxData(FLASH_SPI);
  while(SPI_I2S_GetFlagStatus(FLASH_SPI, SPI_I2S_FLAG_BUSY)); 
  return bRxBuff;
}
 
/**
  * @brief  Read a byte to flash
  * @param  none
  * @retval Flash return data
  */ 
static uint8_t SPI_ReadByte(void)
{
  return (SPI_WriteByte(FLASH_SPI_DUMMY_BYTE));
}

/**
  * @brief  Read data continuously
  * @param  pbBuffer: Buffer to save data
  * @param  dwNumByteToRead: Buffer length
  * @retval none
  */
static void SPI_ReadBytes(uint8_t *pbBuffer, uint32_t dwNumByteToRead)
{
  uint8_t bWriteValue = FLASH_SPI_DUMMY_BYTE;

#if (SPI_MODE == 0)
  DMA_InitType DMA_InitStructure;

  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&FLASH_SPI->DT);
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&bWriteValue;
  DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALDST;
  DMA_InitStructure.DMA_BufferSize = dwNumByteToRead;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;
  DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_DISABLE;
  DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_BYTE;
  DMA_InitStructure.DMA_MemoryDataWidth = DMA_MEMORYDATAWIDTH_BYTE;
  DMA_InitStructure.DMA_Mode = DMA_MODE_NORMAL;
  DMA_InitStructure.DMA_Priority = DMA_PRIORITY_VERYHIGH;
  DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;
  DMA_Init(SPIx_Tx_DMA_Channel, &DMA_InitStructure);

  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&FLASH_SPI->DT);
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pbBuffer;
  DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALSRC;
  DMA_InitStructure.DMA_BufferSize = dwNumByteToRead;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;
  DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE;
  DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_BYTE;
  DMA_InitStructure.DMA_MemoryDataWidth = DMA_MEMORYDATAWIDTH_BYTE;
  DMA_InitStructure.DMA_Mode = DMA_MODE_NORMAL;
  DMA_InitStructure.DMA_Priority = DMA_PRIORITY_VERYHIGH;
  DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE; 
  DMA_Init(SPIx_Rx_DMA_Channel, &DMA_InitStructure); 

  SPI_I2S_DMAEnable(FLASH_SPI,SPI_I2S_DMA_TX, ENABLE);
  SPI_I2S_DMAEnable(FLASH_SPI,SPI_I2S_DMA_RX, ENABLE);    
  DMA_ChannelEnable(SPIx_Rx_DMA_Channel, ENABLE);
  DMA_ChannelEnable(SPIx_Tx_DMA_Channel, ENABLE);

  while(DMA_GetFlagStatus(SPIx_Rx_DMA_FLAG)!=SET);
  DMA_ClearFlag(SPIx_Rx_DMA_FLAG);

  DMA_ChannelEnable(SPIx_Tx_DMA_Channel, DISABLE);
  DMA_ChannelEnable(SPIx_Rx_DMA_Channel, DISABLE);
  SPI_I2S_DMAEnable(FLASH_SPI, SPI_I2S_DMA_TX, DISABLE);
  SPI_I2S_DMAEnable(FLASH_SPI, SPI_I2S_DMA_RX, DISABLE);
#else

  while(dwNumByteToRead--)
  {
    while(SPI_I2S_GetFlagStatus(FLASH_SPI, SPI_I2S_FLAG_TE) == RESET);
    SPI_I2S_TxData(FLASH_SPI, bWriteValue);
    while(SPI_I2S_GetFlagStatus(FLASH_SPI, SPI_I2S_FLAG_RNE) == RESET);
    *pbBuffer = SPI_I2S_RxData(FLASH_SPI);
    pbBuffer++;
  }
#endif /* SPI_MODE */
}

