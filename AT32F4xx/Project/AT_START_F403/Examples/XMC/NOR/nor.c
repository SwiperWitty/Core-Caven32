/* Includes ------------------------------------------------------------------*/
#include "nor.h"
/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup XMC_NOR
  * @{
  */ 

/** 
  * @brief  XMC Bank 1 NOR/SRAM2  
  */
#define Bank1_NOR2_ADDR       ((uint32_t)0x64000000)

/* Delay definition */   
#define BlockErase_Timeout    ((uint32_t)0x00A00000)
#define ChipErase_Timeout     ((uint32_t)0x30000000) 
#define Program_Timeout       ((uint32_t)0x00001400)     


#define ADDR_SHIFT(A) (Bank1_NOR2_ADDR + (2 * (A)))
#define NOR_WRITE(Address, Data)  (*(__IO uint16_t *)(Address) = (Data))  

void NOR_Init(void)
{
    XMC_NORSRAMInitType  XMC_NORSRAMInitStructure;
    XMC_NORSRAMTimingInitType  p;
    GPIO_InitType GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOD | RCC_APB2PERIPH_GPIOE | 
                         RCC_APB2PERIPH_GPIOF | RCC_APB2PERIPH_GPIOG, ENABLE);
  
    XMC_NORSRAMStructInit(&XMC_NORSRAMInitStructure);
    /*-- GPIO Configuration ------------------------------------------------------*/
    /*!< NOR Data lines configuration */
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_0 | GPIO_Pins_1 | GPIO_Pins_8 | GPIO_Pins_9 |
                                GPIO_Pins_10 | GPIO_Pins_14 | GPIO_Pins_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pins =  GPIO_Pins_7 | GPIO_Pins_8 | GPIO_Pins_9 | GPIO_Pins_10 |
                                    GPIO_Pins_11 | GPIO_Pins_12 | GPIO_Pins_13 |
                                    GPIO_Pins_14 | GPIO_Pins_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    /*!< NOR Address lines configuration */
    GPIO_InitStructure.GPIO_Pins =  GPIO_Pins_0 | GPIO_Pins_1 | GPIO_Pins_2 | GPIO_Pins_3 |
                                    GPIO_Pins_4 | GPIO_Pins_5 | GPIO_Pins_12 | GPIO_Pins_13 |
                                    GPIO_Pins_14 | GPIO_Pins_15;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pins =  GPIO_Pins_0 | GPIO_Pins_1 | GPIO_Pins_2 |
                                    GPIO_Pins_3 | GPIO_Pins_4 | GPIO_Pins_5;
    GPIO_Init(GPIOG, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pins =  GPIO_Pins_11 | GPIO_Pins_12 | GPIO_Pins_13;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pins =  GPIO_Pins_3 | GPIO_Pins_4 | GPIO_Pins_5 | GPIO_Pins_6;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    /*!< NOE and NWE configuration */
    GPIO_InitStructure.GPIO_Pins =  GPIO_Pins_4 | GPIO_Pins_5;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /*!< NE2 configuration */
    GPIO_InitStructure.GPIO_Pins =  GPIO_Pins_9;
    GPIO_Init(GPIOG, &GPIO_InitStructure);

    /*!< Configure PD6 for NOR memory Ready/Busy signal */
    GPIO_InitStructure.GPIO_Pins =  GPIO_Pins_6;
    GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
  
    /*-- FSMC Configuration ----------------------------------------------------*/
    p.XMC_AdrOpTime             = 0x02;
    p.XMC_AdrHoldTime           = 0x00;
    p.XMC_DataOpTime            = 0x0A;
    p.XMC_IntervalBetweenOP     = 0x00;
    p.XMC_CLKPsc                = 0x00;
    p.XMC_DataStableTime        = 0x00;
    p.XMC_Mode                  = XMC_Mode_B;

    XMC_NORSRAMInitStructure.XMC_Bank                   = XMC_Bank1_NORSRAM2;
    XMC_NORSRAMInitStructure.XMC_DataAdrMux             = XMC_DataAdrMux_Disable;
    XMC_NORSRAMInitStructure.XMC_Dev                    = XMC_Dev_NOR;
    XMC_NORSRAMInitStructure.XMC_BusType                = XMC_BusType_16b;
    XMC_NORSRAMInitStructure.XMC_EnableBurstMode        = XMC_BurstMode_Disable;
    XMC_NORSRAMInitStructure.XMC_EnableAsynWait         = XMC_AsynWait_Disable;  
    XMC_NORSRAMInitStructure.XMC_WaitSignalLv           = XMC_WaitSignalLv_Low;
    XMC_NORSRAMInitStructure.XMC_EnableBurstModeSplit   = XMC_BurstModeSplit_Disable;
    XMC_NORSRAMInitStructure.XMC_WaitSignalConfig       = XMC_WaitSignalConfig_BeforeWaitState;
    XMC_NORSRAMInitStructure.XMC_EnableWrite            = XMC_WriteOperation_Enable;
    XMC_NORSRAMInitStructure.XMC_EnableWaitSignal       = XMC_WaitSignal_Disable;
    XMC_NORSRAMInitStructure.XMC_EnableWriteTiming      = XMC_WriteTiming_Disable;
    XMC_NORSRAMInitStructure.XMC_WriteBurstSyn          = XMC_WriteBurstSyn_Disable;
    XMC_NORSRAMInitStructure.XMC_RWTimingStruct         = &p;
    XMC_NORSRAMInitStructure.XMC_WTimingStruct          = &p;

    XMC_NORSRAMInit(&XMC_NORSRAMInitStructure);

    /*!< Enable XMC Bank1_NOR Bank */
    XMC_NORSRAMCmd(XMC_Bank1_NORSRAM2, ENABLE);
}

/**
  * @brief  Reads NOR memory's Manufacturer and Device Code.
  * @param  NOR_ID: pointer to a NOR_IDTypeDef structure which will hold the 
  *         Manufacturer and Device Code.  
  * @retval None
  */
void NOR_ReadID(NOR_IDType* NOR_ID)
{
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
    NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x0090);

    NOR_ID->Manufacturer_Code = *(__IO uint16_t *) ADDR_SHIFT(0x0000);
    NOR_ID->Device_Code1 = *(__IO uint16_t *) ADDR_SHIFT(0x0001);
    NOR_ID->Device_Code2 = *(__IO uint16_t *) ADDR_SHIFT(0x000E);
    NOR_ID->Device_Code3 = *(__IO uint16_t *) ADDR_SHIFT(0x000F);
}

/**
  * @brief  Erases the specified Nor memory block.
  * @param  BlockAddr: address of the block to erase.
  * @retval NOR_Status: The returned value can be: NOR_SUCCESS, NOR_ERROR
  *         or NOR_TIMEOUT
  */
NOR_Status NOR_EraseBlock(uint32_t BlockAddr)
{
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
    NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x0080);
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
    NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
    NOR_WRITE((Bank1_NOR2_ADDR + BlockAddr), 0x30);

    return (NOR_GetStatus(BlockErase_Timeout));
}

/**
  * @brief  Erases the entire chip.
  * @param  None                      
  * @retval NOR_Status: The returned value can be: NOR_SUCCESS, NOR_ERROR
  *         or NOR_TIMEOUT
  */
NOR_Status NOR_EraseChip(void)
{
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
    NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x0080);
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
    NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x0010);

    return (NOR_GetStatus(ChipErase_Timeout));
}

/**
  * @brief  Writes a half-word to the NOR memory.
  * @param  WriteAddr: NOR memory internal address to write to.
  * @param  Data: Data to write. 
  * @retval NOR_Status: The returned value can be: NOR_SUCCESS, NOR_ERROR
  *         or NOR_TIMEOUT
  */
NOR_Status NOR_WriteHalfWord(uint32_t WriteAddr, uint16_t Data)
{
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
    NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x00A0);
    NOR_WRITE((Bank1_NOR2_ADDR + WriteAddr), Data);

    return (NOR_GetStatus(Program_Timeout));
}

/**
  * @brief  Writes a half-word buffer to the FSMC NOR memory. 
  * @param  pBuffer: pointer to buffer. 
  * @param  WriteAddr: NOR memory internal address from which the data will be 
  *         written.
  * @param  NumHalfwordToWrite: number of Half words to write. 
  * @retval NOR_Status: The returned value can be: NOR_SUCCESS, NOR_ERROR
  *         or NOR_TIMEOUT
  */
NOR_Status NOR_WriteBuffer(uint16_t* pBuffer, uint32_t WriteAddr, uint32_t NumHalfwordToWrite)
{
    NOR_Status status = NOR_ONGOING; 

    do
    {
        /*!< Transfer data to the memory */
        status = NOR_WriteHalfWord(WriteAddr, *pBuffer++);
        WriteAddr = WriteAddr + 2;
        NumHalfwordToWrite--;
    }
    while((status == NOR_SUCCESS) && (NumHalfwordToWrite != 0));
  
    return (status); 
}

/**
  * @brief  Writes a half-word buffer to the FSMC NOR memory. This function 
  *         must be used only with S29GL128P NOR memory.
  * @param  pBuffer: pointer to buffer. 
  * @param  WriteAddr: NOR memory internal address from which the data will be 
  *         written.
  * @param  NumHalfwordToWrite: number of Half words to write.
  *         The maximum allowed value is 32 Half words (64 bytes).
  * @retval NOR_Status: The returned value can be: NOR_SUCCESS, NOR_ERROR
  *         or NOR_TIMEOUT
  */
NOR_Status NOR_ProgramBuffer(uint16_t* pBuffer, uint32_t WriteAddr, uint32_t NumHalfwordToWrite)
{
    uint32_t lastloadedaddress  = 0x00;
    uint32_t currentaddress     = 0x00;
    uint32_t endaddress         = 0x00;

    /*!< Initialize variables */
    currentaddress      = WriteAddr;
    endaddress          = WriteAddr + NumHalfwordToWrite - 1;
    lastloadedaddress   = WriteAddr;

    /*!< Issue unlock command sequence */
    NOR_WRITE(ADDR_SHIFT(0x00555), 0x00AA);

    NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);  

    /*!< Write Write Buffer Load Command */
    NOR_WRITE(ADDR_SHIFT(WriteAddr), 0x0025);
    NOR_WRITE(ADDR_SHIFT(WriteAddr), (NumHalfwordToWrite - 1));

    /*!< Load Data into NOR Buffer */
    while(currentaddress <= endaddress)
    {
        /*!< Store last loaded address & data value (for polling) */
        lastloadedaddress = currentaddress;
 
        NOR_WRITE(ADDR_SHIFT(currentaddress), *pBuffer++);
        currentaddress += 1; 
    }

    NOR_WRITE(ADDR_SHIFT(lastloadedaddress), 0x29);
  
    return(NOR_GetStatus(Program_Timeout));
}

/**
  * @brief  Reads a half-word from the NOR memory. 
  * @param  ReadAddr: NOR memory internal address to read from.
  * @retval Half-word read from the NOR memory
  */
uint16_t NOR_ReadHalfWord(uint32_t ReadAddr)
{
    NOR_WRITE(ADDR_SHIFT(0x00555), 0x00AA); 
    NOR_WRITE(ADDR_SHIFT(0x002AA), 0x0055);  
    NOR_WRITE((Bank1_NOR2_ADDR + ReadAddr), 0x00F0 );

    return (*(__IO uint16_t *)((Bank1_NOR2_ADDR + ReadAddr)));
}

/**
  * @brief  Reads a block of data from the FSMC NOR memory.
  * @param  pBuffer: pointer to the buffer that receives the data read from the 
  *         NOR memory.
  * @param  ReadAddr: NOR memory internal address to read from.
  * @param  NumHalfwordToRead : number of Half word to read.
  * @retval None
  */
void NOR_ReadBuffer(uint16_t* pBuffer, uint32_t ReadAddr, uint32_t NumHalfwordToRead)
{
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
    NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
    NOR_WRITE((Bank1_NOR2_ADDR + ReadAddr), 0x00F0);

    for(; NumHalfwordToRead != 0x00; NumHalfwordToRead--) /*!< while there is data to read */
    {
        /*!< Read a Halfword from the NOR */
        *pBuffer++ = *(__IO uint16_t *)((Bank1_NOR2_ADDR + ReadAddr));
        ReadAddr = ReadAddr + 2; 
    }  
}

/**
  * @brief  Returns the NOR memory to Read mode.
  * @param  None
  * @retval NOR_SUCCESS
  */
NOR_Status NOR_ReturnToReadMode(void)
{
    NOR_WRITE(Bank1_NOR2_ADDR, 0x00F0);

    return (NOR_SUCCESS);
}

/**
  * @brief  Returns the NOR memory to Read mode and resets the errors in the NOR 
  *         memory Status Register.  
  * @param  None
  * @retval NOR_SUCCESS
  */
NOR_Status NOR_Reset(void)
{
    NOR_WRITE(ADDR_SHIFT(0x00555), 0x00AA); 
    NOR_WRITE(ADDR_SHIFT(0x002AA), 0x0055); 
    NOR_WRITE(Bank1_NOR2_ADDR, 0x00F0); 

    return (NOR_SUCCESS);
}

/**
  * @brief  Returns the NOR operation status.
  * @param  Timeout: NOR progamming Timeout
  * @retval NOR_Status: The returned value can be: NOR_SUCCESS, NOR_ERROR
  *         or NOR_TIMEOUT
  */
NOR_Status NOR_GetStatus(uint32_t Timeout)
{ 
    uint16_t val1 = 0x00, val2 = 0x00;
    NOR_Status status = NOR_ONGOING; 
    uint32_t timeout = Timeout;

    /*!< Poll on NOR memory Ready/Busy signal ----------------------------------*/
    while((GPIO_ReadInputDataBit(GPIOD, GPIO_Pins_6) != RESET) && (timeout > 0)) 
    {
        timeout--;
    }

    timeout = Timeout;
  
    while((GPIO_ReadInputDataBit(GPIOD, GPIO_Pins_6) == RESET) && (timeout > 0))   
    {
        timeout--;
    }
  
    /*!< Get the NOR memory operation status -----------------------------------*/
    while((Timeout != 0x00) && (status != NOR_SUCCESS))
    {
        Timeout--;

        /*!< Read DQ6 and DQ5 */
        val1 = *(__IO uint16_t *)(Bank1_NOR2_ADDR);
        val2 = *(__IO uint16_t *)(Bank1_NOR2_ADDR);

        /*!< If DQ6 did not toggle between the two reads then return NOR_Success */
        if((val1 & 0x0040) == (val2 & 0x0040)) 
        {
            return NOR_SUCCESS;
        }   

        if((val1 & 0x0020) != 0x0020)
        {
            status = NOR_ONGOING;
        }

        val1 = *(__IO uint16_t *)(Bank1_NOR2_ADDR);
        val2 = *(__IO uint16_t *)(Bank1_NOR2_ADDR);
    
        if((val1 & 0x0040) == (val2 & 0x0040)) 
        {
            return NOR_SUCCESS;
        }
        else if((val1 & 0x0020) == 0x0020)
        {
            return NOR_ERROR;
        }
    }

    if(Timeout == 0x00)
    {
        status = NOR_TIMEOUT;
    } 

    /*!< Return the operation status */
    return (status);
}

/**
  * @}
  */ 

/**
  * @}
  */ 

