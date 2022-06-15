/**
  ******************************************************************************
  * File   : USB_Device/VirtualMassStorage_IAP_Demo/src/flash_if.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : FLASH read and write API
  ******************************************************************************
  */
#include "flash_if.h"
#include "at32f4xx_flash.h"
#include "at32f4xx_crc.h"
/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_VirtualMassStrorage_IAP_Demo
  * @{
  */ 
uint32_t AT_FLASH_PAGE_SIZE = FLASH_PAGE2K_SIZE;
uint32_t AT_FLASH_SIZE = 0;
uint32_t AT_FLASH_PAGE_MASK = FLASH_PAGE2K_ALIGNED_MASK;
uint32_t FLASH_FIRMWARE_LENGTH = 0;
uint32_t FLASH_WRCRCValue = 0;
uint32_t FLASH_RDCRCValue = 0;

#define SET_MSP(dwValue) __ASM volatile ("msr msp, "#dwValue)
void (*pfTarget)(void); 
/**
  * @brief  init flash information
  * @param  None
  * @retval None
  */

void AT_FLASH_Init(void)
{
    uint32_t FlashS = *(uint32_t *)0x1FFFF7E0;
    AT_FLASH_SIZE = FlashS << 10;
    
    /*if flash size < 256K Byte, page size is 1k Byte*/
    if ( FlashS < 0x100)
    {
        AT_FLASH_PAGE_SIZE = FLASH_PAGE1K_SIZE;
        AT_FLASH_PAGE_MASK = FLASH_PAGE1K_ALIGNED_MASK;
    }else
    {   /*flash size >= 256K byte, page size is 2kByte*/
        AT_FLASH_PAGE_SIZE = FLASH_PAGE2K_SIZE;
        AT_FLASH_PAGE_MASK = FLASH_PAGE2K_ALIGNED_MASK;
    }
}

/**
  * @brief  Writes data to the FLASH. In this function, the number of
  *         WRITE cycles are reduced, using Page WRITE sequence.
  * @param  Wdata: pointer to the buffer  containing the data to be written
  *         to the FLASH.
  * @param  Address: FLASH's internal address to write to.
  * @param  NumByteToWrite: number of bytes to write to the FLASH.
   * @retval Status: 0 OK, 1 Faild, 2 CRC Faild
  */
uint8_t AT_FLASH_WriteBuffer(uint32_t Address, uint8_t *Wdata, uint32_t NumByteToWrite)
{
    uint32_t *dwBuffer, dwFlashAddr, BufCRC = 0, RdCRC = 0;
    uint32_t dwNumByte, dwRemainByte, dwLastWordMask = 0xFFFFFFFF;
    dwNumByte = NumByteToWrite / sizeof(uint32_t);
    dwRemainByte = NumByteToWrite % sizeof(uint32_t);
    
    if ( (Address + NumByteToWrite) > (FLASH_BASE_ADDR + AT_FLASH_SIZE) )
        return 1;
    
    if ( (Address & AT_FLASH_PAGE_MASK ) == 0 )
    {
        FLASH_Unlock();
        FLASH_ErasePage(Address);
    
        if ( AT_FLASH_PAGE_MASK == FLASH_PAGE1K_ALIGNED_MASK )
        {
            if ( NumByteToWrite > FLASH_PAGE1K_SIZE )
            {
                FLASH_ErasePage(Address + FLASH_PAGE1K_SIZE);
            }
        }
        FLASH_Lock();
    }
    if ( dwRemainByte)
    {
        dwNumByte ++;
        dwLastWordMask <<= (dwRemainByte * 8);
    }
    
    dwBuffer = (uint32_t *)Wdata +  (dwNumByte - 1);
    dwFlashAddr = (Address + (dwNumByte - 1) * sizeof(uint32_t));
    if ( dwRemainByte )
    {
        *dwBuffer |= dwLastWordMask;
    }
    FLASH_Unlock();
    while ( dwNumByte -- )
    {
        FLASH_ProgramWord(dwFlashAddr, *dwBuffer--);
        dwFlashAddr -= 4;
    }
    FLASH_Lock();
    
    BufCRC = AT_CalculateCRC_Buffer((uint32_t *)Wdata, NumByteToWrite);
    RdCRC = AT_CalculateCRC_Flash(Address, NumByteToWrite);
    if ( BufCRC != RdCRC )
        return 2;
    return 0;
}

/**
  * @brief  Reads a block of data from the FLASH.
  * @param  Rdata: pointer to the buffer that receives the data read from the FLASH.
  * @param  Address: FLASH's internal address to read from.
  * @param  NumByteToRead: number of bytes to read from the FLASH.
  * @retval None
  */
void AT_FLASH_ReadBuffer(uint32_t Address, uint8_t *Rdata, uint32_t NumByteToRead)
{
    uint32_t i;
    for ( i = 0; i < NumByteToRead; i ++ )
    {
        Rdata[i] = *(uint8_t *)Address;
        Address ++;
    }
}

/**
  * @brief  Read flash data Calculate CRC.
  * @param  dwAddr: Flash Base Addr.
  * @param  NumByteToCrc: number of bytes to Calculate CRC.
  * @retval None
  */
uint32_t AT_CalculateCRC_Flash(uint32_t dwAddr, uint32_t NumByteToCrc)
{
    uint32_t i = 0, TmpCrc = 0, dwNumByte = NumByteToCrc / sizeof(uint32_t), RemainByte = NumByteToCrc % (sizeof (uint32_t ));
    uint32_t *pwAddr = (uint32_t *)dwAddr;
    if ( RemainByte )
    {
        dwNumByte += 1;
    }
    /*Enable CRC Clock*/
    BIT_SET(RCC->AHBEN, RCC_AHBEN_CRCEN);
    /*Reset CRC*/
    CRC_ResetDT();
    
    for ( i = 0; i < dwNumByte; i ++ )
    {
        TmpCrc = CRC_CalculateCRC(*pwAddr++);
    }
    /*Reset CRC*/
    CRC_ResetDT();
    /*Disable CRC clock*/
    BIT_CLEAR(RCC->AHBEN, RCC_AHBEN_CRCEN);
    return TmpCrc;
    
}
/**
  * @brief  Calculate buffer CRC.
  * @param  buffer: pointer to the buffer that need Calculate CRC.
  * @param  NumByteToCrc: number of bytes to Calculate CRC.
  * @retval None
  */
uint32_t AT_CalculateCRC_Buffer(uint32_t *buffer, uint32_t NumByteToCrc)
{
    uint32_t i = 0, TmpCrc = 0, dwNumByte = NumByteToCrc / sizeof(uint32_t), RemainByte = NumByteToCrc % (sizeof (uint32_t ));
    /*Enable CRC Clock*/
    BIT_SET(RCC->AHBEN, RCC_AHBEN_CRCEN);
    /*Reset CRC*/
    CRC_ResetDT();
    
    if ( RemainByte )
    {
        dwNumByte += 1;
        buffer[dwNumByte - 1] |= 0xFFFFFFFF << ( RemainByte * 8);
    }
    
    for ( i = 0; i < dwNumByte; i ++ )
    {
        TmpCrc = CRC_CalculateCRC(*buffer++);
    }
    /*Reset CRC*/
    CRC_ResetDT();
    /*Disable CRC clock*/
    BIT_CLEAR(RCC->AHBEN, RCC_AHBEN_CRCEN);
    return TmpCrc;
}

 /**
  * @brief  Jump to User Code
  * @param  dwAddr: User Code Start Address.
  * @retval None
  */
void IAP_Jump(uint32_t dwAddr)
{
    uint32_t dwStkPtr, dwJumpAddr;
    uint32_t i = 0;
    dwStkPtr = *(UINT32 *)dwAddr;
    dwJumpAddr = *(UINT32 *)(dwAddr + sizeof(UINT32));
    if ( (dwStkPtr < SRAM_BASE) || ((dwStkPtr & 0xFF000000) != SRAM_BASE || (dwJumpAddr < FLASH_START_ADDR) || ((dwJumpAddr & 0xFF000000) != FLASH_BASE)) )
        return;
    for ( i = 0; i < 1000000; i ++)
       __NOP();

    RCC->APB2RST = 0xFFFF;
    RCC->APB2RST = 0;
    RCC->APB1RST = 0xFFFF;
    RCC->APB1RST = 0;
    RCC->APB2EN = 0;
    RCC->APB1EN = 0;
    
    BIT_SET(RCC->CTRL, RCC_CTRL_HSIEN);

    /* Reset SW, AHBPSC, APB1PSC, APB2PSC, ADCPSC and CLKOUT bits */
    BIT_CLEAR(RCC->CFG, RCC_CFG_SYSCLKSEL | RCC_CFG_AHBPSC | \
            RCC_CFG_APB1PSC | RCC_CFG_APB2PSC | \
            RCC_CFG_ADCPSC | RCC_CFG_CLKOUT);

    /* Reset HSEEN, HSECFDEN and PLLEN bits */
    BIT_CLEAR(RCC->CTRL, RCC_CTRL_HSEEN | RCC_CTRL_HSECFDEN | \
            RCC_CTRL_PLLEN);

    /* Reset HSEBYPS bit */
    BIT_CLEAR(RCC->CTRL, RCC_CTRL_HSEBYPS);

    /* Reset PLLRC, PLLHSEPSC, PLLMUL, USBPSC and PLLRANGE bits */
     BIT_CLEAR(RCC->CFG, RCC_CFG_PLLRC | RCC_CFG_PLLHSEPSC | \
     RCC_CFG_PLLMULT | RCC_CFG_USBPSC | RCC_CFG_PLLRANGE);
     /* Disable all interrupts and clear pending bits  */
     RCC->CLKINT = RCC_CLKINT_LSISTBLFC | RCC_CLKINT_LSESTBLFC | \
                RCC_CLKINT_HSISTBLFC | RCC_CLKINT_HSESTBLFC | \
                RCC_CLKINT_PLLSTBLFC | RCC_CLKINT_HSECFDFC;
    
    SET_MSP(dwStkPtr);
    pfTarget = (void (*)(void))dwJumpAddr;
    pfTarget();

    
}

 /**
  * @brief  Write flag to optionbyte
  * @param  Flag
  * @retval None
  */
void AT_WriteOptionByte(uint8_t Flag)
{
    UOB_Type tUobType;
    uint16_t wHalfWordLength = sizeof ( UOB_Type), *UobData = (uint16_t *)(&tUobType);
    uint32_t UobAddr = UOB_BASE + 2, i;
    tUobType = *UOPTB;
    tUobType.HID[0] = Flag;
    FLASH_Unlock();  
    FLASH_EraseUserOptionBytes();
    UobData++;
    for ( i = 0; i < ((wHalfWordLength >> 1) - 1); i ++, UobData++)
    {
        FLASH_ProgramUserOptionByteData(UobAddr, *(uint8_t *)UobData);
        UobAddr += 2;
    }
    FLASH_Lock();
}


 /**
  * @brief  read flag from optionbyte
  * @param  None
  * @retval flag
  */
uint8_t AT_ReadOptionByte(void)
{
    return (uint8_t)UOPTB->HID[0];
}
/**
  * @}
  */ 

/**
  * @}
  */



