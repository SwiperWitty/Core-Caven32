/**
  **************************************************************************
  * File   : at32f403_nand.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Nand flash drive
  **************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "at32f403_nand.h"

/** @addtogroup AT32F403A_StdPeriph_Examples
  * @{
  */
  
/** @addtogroup XMC_NAND
  * @{
  */ 
  
/** 
  * @brief  XMC Bank 2 
  */
#define XMC_Bank_NAND     XMC_Bank2_NAND
#define Bank_NAND_ADDR     Bank2_NAND_ADDR 
#define Bank2_NAND_ADDR    ((uint32_t)0x70000000)     


 #define ROW_ADDRESS (Address.Page + (Address.Block + (Address.Zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE)  

/**
  * @brief  Configures the XMC and GPIOs to interface with the NAND memory.
  *         This function must be called before any write/read operation on the 
  *         NAND.
  * @param  None
  * @retval None
  */
void NAND_Init(void)
{
  GPIO_InitType GPIO_InitStructure; 
  XMC_NANDInitType XMC_NANDInitStructure;
  XMC_NAND_PCCARDTimingInitType  p;
  
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOD | RCC_APB2PERIPH_GPIOE | 
                         RCC_APB2PERIPH_GPIOF, ENABLE);
  
/*-- GPIO Configuration ------------------------------------------------------*/
/*!< CLE, ALE, D0->D3, NOE, NWE and NCE2  NAND pin configuration  */
  GPIO_InitStructure.GPIO_Pins =  GPIO_Pins_11 | GPIO_Pins_12 | GPIO_Pins_14 | GPIO_Pins_15 |  
                                 GPIO_Pins_0 | GPIO_Pins_1 | GPIO_Pins_4 | GPIO_Pins_5 | 
                                 GPIO_Pins_7;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;

  GPIO_Init(GPIOD, &GPIO_InitStructure); 

/*!< D4->D7 NAND pin configuration  */  
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_7 | GPIO_Pins_8 | GPIO_Pins_9 | GPIO_Pins_10;

  GPIO_Init(GPIOE, &GPIO_InitStructure);


/*!< NWAIT NAND pin configuration */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_6;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU;

  GPIO_Init(GPIOD, &GPIO_InitStructure); 

  /*-- XMC Configuration ------------------------------------------------------*/
  XMC_NANDStructInit(&XMC_NANDInitStructure);
  p.XMC_SetupTime = 254;
  p.XMC_OpTime = 254;
  p.XMC_HoldTime = 254;
  p.XMC_WriteSetupTime = 254;

  XMC_NANDInitStructure.XMC_Bank = XMC_Bank2_NAND;
  XMC_NANDInitStructure.XMC_EnableWait = XMC_WaitOperation_Enable;
  XMC_NANDInitStructure.XMC_BusType = XMC_BusType_8b;
  XMC_NANDInitStructure.XMC_EnableECC = XMC_ECCOperation_Enable;
  XMC_NANDInitStructure.XMC_ECCPageSize = XMC_ECCPageSize_512Bytes;
  XMC_NANDInitStructure.XMC_DelayTimeCR = 0x00;
  XMC_NANDInitStructure.XMC_DelayTimeAR = 0x00;
  XMC_NANDInitStructure.XMC_CommonSpaceTimingStruct = &p;
  XMC_NANDInitStructure.XMC_AttributeSpaceTimingStruct = &p;

  XMC_NANDInit(&XMC_NANDInitStructure);

  /*!< XMC NAND Bank Cmd Test */
  XMC_NANDCmd(XMC_Bank2_NAND, ENABLE);
}

/**
  * @brief  Reads NAND memory's ID.
  * @param  NAND_ID: pointer to a NAND_IDTypeDef structure which will hold
  *         the Manufacturer and Device ID.  
  * @retval None
  */
void NAND_ReadID(NAND_IDTypeDef* NAND_ID)
{
  uint32_t data = 0;

  /*!< Send Command to the command area */
  *(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = 0x90;
  *(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = 0x00;

   /*!< Sequence to read ID from NAND flash */
   data = *(__IO uint32_t *)(Bank_NAND_ADDR | DATA_AREA);

   NAND_ID->Maker_ID   = ADDR_1st_CYCLE (data);
   NAND_ID->Device_ID  = ADDR_2nd_CYCLE (data);
   NAND_ID->Third_ID   = ADDR_3rd_CYCLE (data);
   NAND_ID->Fourth_ID  = ADDR_4th_CYCLE (data);
}

/**
  * @brief  This routine is for writing one or several 512 Bytes Page size.
  * @param  pBuffer: pointer on the Buffer containing data to be written 
  * @param  Address: First page address
  * @param  NumPageToWrite: Number of page to write  
  * @retval New status of the NAND operation. This parameter can be:
  *              - NAND_TIMEOUT_ERROR: when the previous operation generate 
  *                a Timeout error
  *              - NAND_READY: when memory is ready for the next operation 
  *                And the new status of the increment address operation. It can be:
  *              - NAND_VALID_ADDRESS: When the new address is valid address
  *              - NAND_INVALID_ADDRESS: When the new address is invalid address  
  */
uint32_t NAND_WriteSmallPage(uint8_t *pBuffer, NAND_ADDRESS Address, uint32_t NumPageToWrite)
{
  uint32_t index = 0x00, numpagewritten = 0x00, addressstatus = NAND_VALID_ADDRESS;
  uint32_t status = NAND_READY, size = 0x00;

  while((NumPageToWrite != 0x00) && (addressstatus == NAND_VALID_ADDRESS) && (status == NAND_READY))
  {
    /*!< Page write command and address */
    *(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_AREA_A;
    *(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_WRITE0;

    *(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = 0x00;
    *(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(ROW_ADDRESS);
    *(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(ROW_ADDRESS);
    *(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_3rd_CYCLE(ROW_ADDRESS);
    *(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_4th_CYCLE(ROW_ADDRESS);            

    /*!< Calculate the size */
    size = NAND_PAGE_SIZE + (NAND_PAGE_SIZE * numpagewritten);

    /*!< Write data */
    for(; index < size; index++)
    {
      *(__IO uint8_t *)(Bank_NAND_ADDR | DATA_AREA) = pBuffer[index];
    }
    
    *(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_WRITE_TRUE1;

    /*!< Check status for successful operation */
    status = NAND_GetStatus();
    
    if(status == NAND_READY)
    {
      numpagewritten++;

      NumPageToWrite--;

      /*!< Calculate Next small page Address */
      addressstatus = NAND_AddressIncrement(&Address);
    }
  }
  
  return (status | addressstatus);
}

/**
  * @brief  This routine is for sequential read from one or several 512 Bytes Page size.
  * @param  pBuffer: pointer on the Buffer to fill
  * @param  Address: First page address
  * @param  NumPageToRead: Number of page to read  
  * @retval New status of the NAND operation. This parameter can be:
  *              - NAND_TIMEOUT_ERROR: when the previous operation generate 
  *                a Timeout error
  *              - NAND_READY: when memory is ready for the next operation 
  *                And the new status of the increment address operation. It can be:
  *              - NAND_VALID_ADDRESS: When the new address is valid address
  *              - NAND_INVALID_ADDRESS: When the new address is invalid address
  */
uint32_t NAND_ReadSmallPage(uint8_t *pBuffer, NAND_ADDRESS Address, uint32_t NumPageToRead)
{
  uint32_t index = 0x00, numpageread = 0x00, addressstatus = NAND_VALID_ADDRESS;
  uint32_t status = NAND_READY, size = 0x00 ,i;

  while((NumPageToRead != 0x0) && (addressstatus == NAND_VALID_ADDRESS))
  {
    /*!< Page Read command and page address */
    *(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_AREA_A;
   
    *(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = 0x00; 
    *(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(ROW_ADDRESS);
    *(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(ROW_ADDRESS);
    *(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_3rd_CYCLE(ROW_ADDRESS);
    *(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_4th_CYCLE(ROW_ADDRESS);      
    
    *(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_AREA_TRUE1;         

    for(i=0;i<25000;i++)
    {
        ;
    }      
    /*!< Calculate the size */
    size = NAND_PAGE_SIZE + (NAND_PAGE_SIZE * numpageread);
    
    /*!< Get Data into Buffer */    
    for(; index < size; index++)
    {
      pBuffer[index]= *(__IO uint8_t *)(Bank_NAND_ADDR | DATA_AREA);
    } 

    numpageread++;
    
    NumPageToRead--;

    /*!< Calculate page address */
    addressstatus = NAND_AddressIncrement(&Address);
  }

  status = NAND_GetStatus();
  
  return (status | addressstatus);
}

/**
  * @brief  This routine write the spare area information for the specified
  *         pages addresses.  
  * @param  pBuffer: pointer on the Buffer containing data to be written 
  * @param  Address: First page address
  * @param  NumSpareAreaTowrite: Number of Spare Area to write
  * @retval New status of the NAND operation. This parameter can be:
  *              - NAND_TIMEOUT_ERROR: when the previous operation generate 
  *                a Timeout error
  *              - NAND_READY: when memory is ready for the next operation 
  *                And the new status of the increment address operation. It can be:
  *              - NAND_VALID_ADDRESS: When the new address is valid address
  *              - NAND_INVALID_ADDRESS: When the new address is invalid address
  */
uint32_t NAND_WriteSpareArea(uint8_t *pBuffer, NAND_ADDRESS Address, uint32_t NumSpareAreaTowrite)
{
  uint32_t index = 0x00, numsparesreawritten = 0x00, addressstatus = NAND_VALID_ADDRESS;
  uint32_t status = NAND_READY, size = 0x00; 

  while((NumSpareAreaTowrite != 0x00) && (addressstatus == NAND_VALID_ADDRESS) && (status == NAND_READY))
  {
    /*!< Page write Spare area command and address */
    *(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_AREA_C;
    *(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_WRITE0;

    *(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = 0x00; 
    *(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(ROW_ADDRESS);
    *(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(ROW_ADDRESS);
    *(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_3rd_CYCLE(ROW_ADDRESS);
    *(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_4th_CYCLE(ROW_ADDRESS);      

    /*!< Calculate the size */ 
    size = NAND_SPARE_AREA_SIZE + (NAND_SPARE_AREA_SIZE * numsparesreawritten);

    /*!< Write the data */ 
    for(; index < size; index++)
    {
      *(__IO uint8_t *)(Bank_NAND_ADDR | DATA_AREA) = pBuffer[index];
    }
    
    *(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_WRITE_TRUE1;

    /*!< Check status for successful operation */
    status = NAND_GetStatus();

    if(status == NAND_READY)
    {
      numsparesreawritten++;

      NumSpareAreaTowrite--;
    
      /*!< Calculate Next page Address */
      addressstatus = NAND_AddressIncrement(&Address);
    }
  }
  
  return (status | addressstatus);
}

/**
  * @brief  This routine read the spare area information from the specified
  *         pages addresses.  
  * @param  pBuffer: pointer on the Buffer to fill 
  * @param  Address: First page address
  * @param  NumSpareAreaToRead: Number of Spare Area to read
  * @retval New status of the NAND operation. This parameter can be:
  *              - NAND_TIMEOUT_ERROR: when the previous operation generate 
  *                a Timeout error
  *              - NAND_READY: when memory is ready for the next operation 
  *                And the new status of the increment address operation. It can be:
  *              - NAND_VALID_ADDRESS: When the new address is valid address
  *              - NAND_INVALID_ADDRESS: When the new address is invalid address
  */
uint32_t NAND_ReadSpareArea(uint8_t *pBuffer, NAND_ADDRESS Address, uint32_t NumSpareAreaToRead)
{
  uint32_t numsparearearead = 0x00, index = 0x00, addressstatus = NAND_VALID_ADDRESS;
  uint32_t status = NAND_READY, size = 0x00 ,i;

  while((NumSpareAreaToRead != 0x0) && (addressstatus == NAND_VALID_ADDRESS))
  {     
    /*!< Page Read command and page address */
    *(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_AREA_C;

    *(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = 0x00; 
    *(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(ROW_ADDRESS);
    *(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(ROW_ADDRESS);
    *(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_3rd_CYCLE(ROW_ADDRESS);
    *(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_4th_CYCLE(ROW_ADDRESS); 

    *(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_AREA_TRUE1;      
            
    for(i=0;i<25000;i++)
    {
        ;
    }
          
    /*!< Data Read */
    size = NAND_SPARE_AREA_SIZE +  (NAND_SPARE_AREA_SIZE * numsparearearead);

    /*!< Get Data into Buffer */
    for ( ;index < size; index++)
    {
      pBuffer[index] = *(__IO uint8_t *)(Bank_NAND_ADDR | DATA_AREA);
    }
    
    numsparearearead++;
    
    NumSpareAreaToRead--;

    /*!< Calculate page address */
    addressstatus = NAND_AddressIncrement(&Address);
  }

  status = NAND_GetStatus();

  return (status | addressstatus);
}

/**
  * @brief  This routine erase complete block from NAND FLASH
  * @param  Address: Any address into block to be erased
  * @retval New status of the NAND operation. This parameter can be:
  *              - NAND_TIMEOUT_ERROR: when the previous operation generate 
  *                a Timeout error
  *              - NAND_READY: when memory is ready for the next operation 
  */
uint32_t NAND_EraseBlock(NAND_ADDRESS Address)
{
  *(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_ERASE0;

  *(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(ROW_ADDRESS);
  *(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(ROW_ADDRESS);
  *(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_3rd_CYCLE(ROW_ADDRESS);
  *(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = ADDR_4th_CYCLE(ROW_ADDRESS);    

  *(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_ERASE1; 

  return (NAND_GetStatus());
}

/**
  * @brief  This routine reset the NAND FLASH.
  * @param  None
  * @retval NAND_READY
  */
uint32_t NAND_Reset(void)
{
  *(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_RESET;

  return (NAND_READY);
}

/**
  * @brief  Get the NAND operation status.
  * @param  None
  * @retval New status of the NAND operation. This parameter can be:
  *              - NAND_TIMEOUT_ERROR: when the previous operation generate
  *                a Timeout error
  *              - NAND_READY: when memory is ready for the next operation
  */
uint32_t NAND_GetStatus(void)
{
  uint32_t timeout = 0x1000000, status = NAND_READY;

  status = NAND_ReadStatus();

  /*!< Wait for a NAND operation to complete or a TIMEOUT to occur */
  while ((status != NAND_READY) &&( timeout != 0x00))
  {
     status = NAND_ReadStatus();
     timeout --;
  }

  if(timeout == 0x00)
  {
    status =  NAND_TIMEOUT_ERROR;
  }

  /*!< Return the operation status */
  return (status);
}
  
/**
  * @brief  Reads the NAND memory status using the Read status command. 
  * @param  None
  * @retval The status of the NAND memory. This parameter can be:
  *              - NAND_BUSY: when memory is busy
  *              - NAND_READY: when memory is ready for the next operation
  *              - NAND_ERROR: when the previous operation gererates error
  */
uint32_t NAND_ReadStatus(void)
{
  uint32_t data = 0x00, status = NAND_BUSY;

  /*!< Read status operation ------------------------------------ */
  *(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_STATUS;
  data = *(__IO uint8_t *)(Bank_NAND_ADDR);

  if((data & NAND_ERROR) == NAND_ERROR)
  {
    status = NAND_ERROR;
  } 
  else if((data & NAND_READY) == NAND_READY)
  {
    status = NAND_READY;
  }
  else
  {
    status = NAND_BUSY; 
  }
  
  return (status);
}

/**
  * @brief  Increment the NAND memory address. 
  * @param  Address: address to be incremented.
  * @retval The new status of the increment address operation. It can be:
  *              - NAND_VALID_ADDRESS: When the new address is valid address
  *              - NAND_INVALID_ADDRESS: When the new address is invalid address   
  */
uint32_t NAND_AddressIncrement(NAND_ADDRESS* Address)
{
  uint32_t status = NAND_VALID_ADDRESS;
 
  Address->Page++;

  if(Address->Page == NAND_BLOCK_SIZE)
  {
    Address->Page = 0;
    Address->Block++;
    
    if(Address->Block == NAND_ZONE_SIZE)
    {
      Address->Block = 0;
      Address->Zone++;

      if(Address->Zone == NAND_MAX_ZONE)
      {
        status = NAND_INVALID_ADDRESS;
      }
    }
  } 
  
  return (status);
}


/**
  * @}
  */

/**
  * @}
  */

