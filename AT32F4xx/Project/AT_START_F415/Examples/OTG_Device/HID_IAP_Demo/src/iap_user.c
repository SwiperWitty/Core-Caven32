 /**
  ******************************************************************************
  * File   : USB_Device/HID_IAP_Demo/src/iap_user.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : IAP HID Demo protocol implementation
  ******************************************************************************
  */

#include "iap_user.h"
#include "at_spi_flash.h"

/** @addtogroup AT32F413_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_HID_IAP_Demo
  * @{
  */ 
  
IAP_InfoType IAP_Info;    ///IAP flow information
uint32_t g_dwFlashSize;   ///Flash size
#define SET_MSP(dwValue) __ASM volatile ("msr msp, "#dwValue)
void (*pfTarget)(void); 


static void IAP_init(void);
static void IAP_Respond(uint16_t iap_cmd, uint16_t status);
static uint32_t IAP_Firmware_CRC(uint32_t dwsAddr, uint16_t hwLength);
static uint32_t IAP_SPIFLASH_CRC(uint32_t dwsAddr, uint16_t hwLength);
static void IAP_clear_flag(void);
static void IAP_set_flag(void);

#define READ_BUFFER_LEN    4096            ///Default read buffer len
uint8_t IAPReadBuffer[READ_BUFFER_LEN];    ///IAP read buffer

/**
  * @brief  Initializes iap struct
  * @param  None
  * @retval None
  */
static void IAP_init(void)
{
    IAP_Info.IAP_Addr = 0;
    IAP_Info.IAP_Current_Data_LEN = 0;
    IAP_Info.IAP_APP_Addr = FLASH_APP1_ADDR;
    IAP_Info.IAP_Step = IAP_START;
    g_dwFlashSize =  KB_TO_B(FLASH_SIZE_REG());
    if ( g_dwFlashSize < FLASH_SIZE_256K )
    {
        IAP_Info.IAP_Sector_size = 1;
    }
    else
    {
        IAP_Info.IAP_Sector_size = 2;
    }
    
}


/**
  * @brief  iap respond status 
  * @param  iap_cmd : which command
  * @param  status (NACK/ACK)
  * @retval None
  */
static void IAP_Respond(uint16_t iap_cmd, uint16_t status)
{
    IAP_Info.IAP_Tx_Buffer[0] = (iap_cmd >> 8) & 0x00FF;
    IAP_Info.IAP_Tx_Buffer[1] = (iap_cmd) & 0x00FF;
    IAP_Info.IAP_Tx_Buffer[2] = (status >> 8) & 0x00FF;
    IAP_Info.IAP_Tx_Buffer[3] = (status) & 0x00FF;
    IAP_Info.IAP_Tx_Len = 4;
}


/**
  * @brief  Erase page
  * @param  Addr : Erase Start addr 
  * @retval None
  */
static IAP_ResultType IAP_Erase_Page(uint32_t addr)
{
    if ( addr < FLASH_APP1_ADDR )
        return IAP_FAILED;
    if ( IAP_Info.IAP_Sector_size == 1 )
    {
        FLASH_Unlock();	
		FLASH_ErasePage(addr);
		FLASH_Lock();
    }else
    {
        if ( (addr & 0x7FF) != 0x000 )
            return IAP_SUCCESS;
        else
        {
            FLASH_Unlock();	
		    FLASH_ErasePage(addr);
		    FLASH_Lock();
        }
    }
    return IAP_SUCCESS;
}

/**
  * @brief  Write data to Flash
  * @param  Addr : Write Flash Start address 
  * @retval Write Status IAP_SUCCESS
  */
static IAP_ResultType IAP_Write(uint32_t dwaddr, uint8_t *data, uint32_t len)
{
    uint32_t pFlashBuffer, *pbuffer;
    uint32_t dwlen = len / sizeof(uint32_t);
    
    pFlashBuffer = (dwaddr + (dwlen - 1)*sizeof(uint32_t));
    pbuffer = (uint32_t *)data + (dwlen - 1); 
    FLASH_Unlock();	
    while ( dwlen -- )
    {
        FLASH_ProgramWord(pFlashBuffer, *pbuffer);
        pFlashBuffer -= 4;
        pbuffer --;
    }
    FLASH_Lock();
    return IAP_SUCCESS;
}

/**
  * @brief  Save data to fifo
  * @param  dbuf : data buffer
  * @param  len  : data len
  * @retval 1: Fifo Full, 0 Fifo not Full
  */
static uint32_t IAP_Save_data(uint8_t *dbuf, uint32_t len)
{
    uint32_t i = 0, page_len = IAP_FIFO_SIZE;
    
    if ( IAP_Info.IAP_Step == IAP_SPI_ADDR )       
        page_len = FLASH_SPI_PageSize();
    
    if ( len + IAP_Info.IAP_Current_Data_LEN <= page_len )
    {
        for ( i = 0; i < len; i ++ )
        {
           IAP_Info.IAP_FiFo_Buffer[IAP_Info.IAP_Current_Data_LEN] = *dbuf;
           dbuf ++;
           IAP_Info.IAP_Current_Data_LEN ++;
        }
    }
    if ( IAP_Info.IAP_Current_Data_LEN == page_len)
        return 1;
    else 
        return 0;
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
    if ( (dwStkPtr < SRAM_BASE) || ((dwStkPtr & 0xFF000000) != SRAM_BASE || (dwJumpAddr < FLASH_APP1_ADDR) || ((dwJumpAddr & 0xFF000000) != FLASH_BASE)) )
        return;
    for ( i = 0; i < 1000000; i ++)
       __NOP();

 
    RCC->APB2RST = 0xFFFF;
    RCC->APB2RST = 0;
    RCC->APB1RST = 0xFFFF;
    RCC->APB1RST = 0;
    RCC->APB2EN = 0;
    RCC->APB1EN = 0;
    RCC->AHBRST = 0xFFFF;
    RCC->AHBRST = 0;
    
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
  * @brief  Calculate Firmware CRC
  * @param  dwsAddr : Calculate Start Addr 
  * @param  hwLength : Calculate length, 1K
  * @retval CRC Value: 
  */
static uint32_t IAP_Firmware_CRC(uint32_t dwsAddr, uint16_t hwLength)
{
    uint32_t dwValue, dwLen = (hwLength * INTERNAL_PAGE1K_BYTES) / sizeof(uint32_t);
    uint32_t *pwAddr = (uint32_t *)dwsAddr;
    /*Enable CRC Clock*/
    RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_CRC, ENABLE);
    /*Reset CRC*/
    CRC_ResetDT();
    
    while ( dwLen -- )
    {
        dwValue = *pwAddr;
        CRC_CalculateCRC(CONVERT_ENDIAN(dwValue));
        pwAddr ++;
        
    }
    return CRC_GetCRC();
}

/**
  * @brief  Calculate buffer crc
  * @param  data : Calculate buffer pointer
  * @param  len : Calculate length
  * @retval 0
  */
uint32_t CRC_Convertion(uint32_t *data, uint32_t len)
{
    uint32_t i = 0; 
    for ( i = 0; i < len; i ++)
        CRC_CalculateCRC(CONVERT_ENDIAN(*(data+i)));
    return 0;
}

 /**
  * @brief  Extern SPI Flash Calculate Firmware CRC
  * @param  dwsAddr : Calculate Start Addr 
  * @param  hwLength : Calculate length, 1K
  * @retval CRC Value: 
  */
static uint32_t IAP_SPIFLASH_CRC(uint32_t dwsAddr, uint16_t hwLength)
{
    uint32_t dwLen = (hwLength * FLASH_SPI_PageSize());
    uint32_t NumofRtime = dwLen / READ_BUFFER_LEN;
    uint32_t NumofSingle = dwLen % READ_BUFFER_LEN;
    
    /*Enable CRC Clock*/
    RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_CRC, ENABLE);
    /*Reset CRC*/
    CRC_ResetDT();
    
    if ( NumofRtime == 0 )
    {
        FLASH_SPI_ReadBuffer(IAPReadBuffer, dwsAddr, dwLen);
        CRC_Convertion((uint32_t *)IAPReadBuffer, dwLen/sizeof(uint32_t));
//        CRC_CalculateBlkCRC((uint32_t *)IAPReadBuffer, dwLen/sizeof(uint32_t));
    }
    else
    {
        while ( NumofRtime -- )
        {
            FLASH_SPI_ReadBuffer(IAPReadBuffer, dwsAddr, READ_BUFFER_LEN);
//            CRC_CalculateBlkCRC((uint32_t *)IAPReadBuffer, READ_BUFFER_LEN/sizeof(uint32_t));
            CRC_Convertion((uint32_t *)IAPReadBuffer, READ_BUFFER_LEN/sizeof(uint32_t));
            dwsAddr += READ_BUFFER_LEN;
        }
        if (NumofSingle)
        {
             FLASH_SPI_ReadBuffer(IAPReadBuffer, dwsAddr, NumofSingle);
//             CRC_CalculateBlkCRC((uint32_t *)IAPReadBuffer, NumofSingle/sizeof(uint32_t));
             CRC_Convertion((uint32_t *)IAPReadBuffer, NumofSingle/sizeof(uint32_t));
        }
    }
    return CRC_GetCRC();
}

/**
  * @brief  Clear download flag
  * @param  None
  * @retval NonE
  */
static void IAP_clear_flag(void)
{
    FLASH_Unlock();	
    FLASH_ErasePage(IAP_CMD_FINISH_FLAG_ADDR);
    FLASH_Lock();
}

/**
  * @brief  Set download flag
  * @param  None
  * @retval NonE
  */
static void IAP_set_flag(void)
{
    FLASH_Unlock();
    FLASH_ProgramWord(IAP_CMD_FINISH_FLAG_ADDR, IAP_DOWNLOAD_FLAG);
    FLASH_Lock();
}


/**
  * @brief  get download flag
  * @param  None
  * @retval IAP_ResultType
  */
IAP_ResultType IAP_get_flag(void)
{
    uint32_t iap_flag = *((uint32_t *)IAP_CMD_FINISH_FLAG_ADDR);
    if (iap_flag == IAP_DOWNLOAD_FLAG )
        return IAP_SUCCESS;
    else
        return IAP_FAILED;
        
}

/**
  * @brief  Parse IAP Command
  * @param  iap_buf : rx data from usb  
  * @param  pkt_size : rx data length
  * @retval IAP_ResultType: Success/Failed
  */
IAP_ResultType IAP_Command_Parse(uint8_t *iap_buf, uint32_t pkt_size)
{
//    static uint32_t tmp_addr = 0;
    uint32_t iap_crc = 0;
    uint16_t iap_cmd = stohw(iap_buf);
    uint8_t *iap_tmp = iap_buf + 2;
    uint8_t res = 0;
    
    /***Enter IAP Mode, Init IAP Flag***/
    if ( iap_cmd == IAP_CMD_IDLE )
    {
        IAP_init();
        IAP_Respond(IAP_CMD_IDLE, IAP_ACK);
        IAP_Info.IAP_Step = IAP_IDLE;
        res = 1;
    }
    
    
    if (iap_cmd == IAP_CMD_START)
    {
        IAP_init();
        IAP_Respond(IAP_CMD_START, IAP_ACK);
        IAP_Info.IAP_Step = IAP_START;
        res = 1;
    }
    
    if ( iap_cmd == IAP_CMD_SPIFLASH_GET )
    {
        uint32_t flash_size = FLASH_SPI_GetSize();
        uint32_t Sector_size = FLASH_SPI_SectorSize();
        uint32_t Page_size = FLASH_SPI_PageSize();
        IAP_Respond(IAP_CMD_SPIFLASH_GET, IAP_ACK);
        IAP_Info.IAP_Tx_Buffer[4] = (flash_size >> 24) & 0xFF;
        IAP_Info.IAP_Tx_Buffer[5] = (flash_size >> 16) & 0xFF;
        IAP_Info.IAP_Tx_Buffer[6] = (flash_size >> 8) & 0xFF;
        IAP_Info.IAP_Tx_Buffer[7] = (flash_size) & 0xFF;
        
        IAP_Info.IAP_Tx_Buffer[8] = (Sector_size >> 24) & 0xFF;
        IAP_Info.IAP_Tx_Buffer[9] = (Sector_size >> 16) & 0xFF;
        IAP_Info.IAP_Tx_Buffer[10] = (Sector_size >> 8) & 0xFF;
        IAP_Info.IAP_Tx_Buffer[11] = (Sector_size) & 0xFF;
        
        IAP_Info.IAP_Tx_Buffer[12] = (Page_size >> 24) & 0xFF;
        IAP_Info.IAP_Tx_Buffer[13] = (Page_size >> 16) & 0xFF;
        IAP_Info.IAP_Tx_Buffer[14] = (Page_size >> 8) & 0xFF;
        IAP_Info.IAP_Tx_Buffer[15] = (Page_size) & 0xFF;
        res = 1;
    }
    
    if (iap_cmd == IAP_CMD_SPIFLASH_ADDR )
    {
        
        IAP_Info.IAP_Addr = stow(iap_tmp);
 
        IAP_Respond(IAP_CMD_SPIFLASH_ADDR, IAP_ACK);
        IAP_Info.IAP_Step = IAP_SPI_ADDR;
        res = 1;
    }
    
    if (iap_cmd == IAP_CMD_SPIFLASH_DATA)
    {
        if ( IAP_Info.IAP_Step == IAP_SPI_ADDR )
        {
             /*Ð´FLASH ±ØÐë°´ÕÕPage¶ÔÆë*/
             if ( IAP_Save_data(iap_tmp + 2, stohw(iap_tmp)) )
             {
                 FLASH_SPI_WriteBuffer(IAP_Info.IAP_FiFo_Buffer, IAP_Info.IAP_Addr, FLASH_SPI_PageSize());
                 IAP_Info.IAP_Addr = 0;
                 IAP_Info.IAP_Current_Data_LEN = 0;
                 IAP_Respond(IAP_CMD_SPIFLASH_DATA, IAP_ACK);
                 res = 1;
             }
        }
    }
    
    /***Set download Address***/
    if (iap_cmd == IAP_CMD_ADDR)
    {
        IAP_Info.IAP_Addr = stow(iap_tmp);
        if (IAP_Info.IAP_Step == IAP_START )
            IAP_clear_flag();
        if ( IAP_Erase_Page(IAP_Info.IAP_Addr) == IAP_SUCCESS)
            IAP_Respond(IAP_CMD_ADDR, IAP_ACK);
        else
            IAP_Respond(IAP_CMD_ADDR, IAP_NACK);
        IAP_Info.IAP_Step = IAP_ADDR;
        res = 1;
    }
    
    /***Download Firmware Data***/
    if (iap_cmd == IAP_CMD_DATA)
    {
        if (IAP_Info.IAP_Step == IAP_ADDR )
        {
            /*When Fifo Full 1KByte, Write data from fifo to Flash*/
            if ( IAP_Save_data(iap_tmp + 2, stohw(iap_tmp)) )
            {
                /*write to flash*/
                IAP_Write(IAP_Info.IAP_Addr, IAP_Info.IAP_FiFo_Buffer, IAP_FIFO_SIZE);
                IAP_Info.IAP_Addr = 0;
                IAP_Info.IAP_Current_Data_LEN = 0;
                IAP_Respond(IAP_CMD_DATA, IAP_ACK);
                res = 1;
            }
        }
        else
            IAP_Respond(IAP_CMD_DATA, IAP_NACK);
    }
    
    /***Download firmware Data Finish***/
    if (iap_cmd == IAP_CMD_FINISH)
    {
        IAP_Info.IAP_Step = IAP_FINISH;
        IAP_set_flag();
        IAP_Respond(IAP_CMD_FINISH, IAP_ACK);
        res = 1;
    }
    
    
    /***Calculate Firmware CRC***/
    if (iap_cmd == IAP_CMD_CRC)
    {
        uint8_t *lenStr = iap_tmp+4;
        IAP_Info.IAP_Step = IAP_CRC;
        iap_crc = IAP_Firmware_CRC(stow(iap_tmp), stohw(lenStr));
        IAP_Respond(IAP_CMD_CRC, IAP_ACK);
        IAP_Info.IAP_Tx_Buffer[4] = (iap_crc >> 24) & 0xFF;
        IAP_Info.IAP_Tx_Buffer[5] = (iap_crc >> 16) & 0xFF;
        IAP_Info.IAP_Tx_Buffer[6] = (iap_crc >> 8) & 0xFF;
        IAP_Info.IAP_Tx_Buffer[7] = (iap_crc) & 0xFF;
        res = 1;
    }
    
    if ( iap_cmd == IAP_CMD_SPIFLASH_CRC)
    {
        uint8_t *lenStr = iap_tmp+4;
        IAP_Info.IAP_Step = IAP_SPI_CRC;
        iap_crc = IAP_SPIFLASH_CRC(stow(iap_tmp), stohw(lenStr));
        IAP_Respond(IAP_CMD_SPIFLASH_CRC, IAP_ACK);
        IAP_Info.IAP_Tx_Buffer[4] = (iap_crc >> 24) & 0xFF;
        IAP_Info.IAP_Tx_Buffer[5] = (iap_crc >> 16) & 0xFF;
        IAP_Info.IAP_Tx_Buffer[6] = (iap_crc >> 8) & 0xFF;
        IAP_Info.IAP_Tx_Buffer[7] = (iap_crc) & 0xFF;
        res = 1;
        
        
    }
    
    /*Jump to User app*/
    if ( iap_cmd == IAP_CMD_JMP)
    {
        IAP_Info.IAP_Step = IAP_JMP_PRE;
        IAP_Respond(IAP_CMD_JMP, IAP_ACK);
        res = 1;
    }
    
    /*Get IAP user code address*/
    if (iap_cmd == IAP_CMD_GET)
    {
        IAP_Respond(IAP_CMD_GET, IAP_ACK);
        IAP_Info.IAP_Tx_Buffer[4] = (FLASH_APP1_ADDR >> 24) & 0xFF;
        IAP_Info.IAP_Tx_Buffer[5] = (FLASH_APP1_ADDR >> 16) & 0xFF;
        IAP_Info.IAP_Tx_Buffer[6] = (FLASH_APP1_ADDR >> 8) & 0xFF;
        IAP_Info.IAP_Tx_Buffer[7] = (FLASH_APP1_ADDR) & 0xFF;
        res = 1;
        
    }
    /*Data Respond*/
    if (res == 1)
    {
        IAP_Info.IAP_Tx_status = 1;
//        EP1_Respond_Data(IAP_Info.IAP_Tx_Buffer, IAP_IN_MAXPKT_SIZE);
    }
    return IAP_SUCCESS;
}



/**
  * @brief  IAP LOOP
  * @param  NULL 
  * @retval NULL
  */
void IAP_handle_loop(void)
{
    if ( IAP_Info.IAP_Step == IAP_JUMP )
    {
        IAP_Info.IAP_Step = IAP_IDLE;
        IAP_Jump(FLASH_APP1_ADDR);
    }
}

/**
  * @}
  */ 

/**
  * @}
  */

