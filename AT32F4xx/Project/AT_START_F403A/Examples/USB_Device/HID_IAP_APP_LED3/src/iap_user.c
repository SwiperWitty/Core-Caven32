 /**
  ******************************************************************************
  * File   : iap_user.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : IAP HID APP Demo protocol implementation
  ******************************************************************************
  */

#include "iap_user.h"
#include "usb_sil.h"
#include "usb_istr.h"
#include "at_spi_flash.h"

/** @addtogroup AT32F403A_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_HID_APP_Demo
  * @{
  */ 
  
IAP_InfoType IAP_Info;    ///IAP flow information
uint32_t g_dwFlashSize;   ///Flash size
#define SET_MSP(dwValue) __ASM volatile ("msr msp, "#dwValue)
void (*pfTarget)(void); 


static void IAP_init(void);
static void IAP_Respond(uint16_t iap_cmd, uint16_t status);
static void IAP_clear_flag(void);

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
    uint16_t iap_cmd = stohw(iap_buf);
    uint8_t res = 0;
    
    /***Enter IAP Mode, Init IAP Flag***/
    if ( iap_cmd == IAP_CMD_IDLE )
    {
        IAP_init();
        IAP_Respond(IAP_CMD_IDLE, IAP_ACK);
        IAP_Info.IAP_Step = IAP_JMP_PRE;
        res = 1;
    }
    
    
    /*Data Respond*/
    if (res == 1)
    {
        IAP_Info.IAP_Tx_status = 1;
        EP1_Respond_Data(IAP_Info.IAP_Tx_Buffer, IAP_IN_MAXPKT_SIZE);
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
        IAP_clear_flag();
        NVIC_SystemReset();
    }
}

/**
  * @}
  */ 

/**
  * @}
  */

