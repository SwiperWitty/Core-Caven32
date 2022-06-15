/*
  **************************************************************************
  * File   : main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : 
  **************************************************************************
  */
#include "cf.h"
#include "at32_board.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup XMC_CF_CARD
  * @{
  */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  FAILED = 0,
  PASSED = 1
} TestStatus;

/* Private define ------------------------------------------------------------*/
#define Start_Sector	768

/* Private variables ---------------------------------------------------------*/
PCCARD_HandleType pccardHandle;
XMC_NAND_PCCARDTimingInitType PCCARD_PMEM4_Timing;
XMC_NAND_PCCARDTimingInitType PCCARD_PATT4_Timing;
XMC_NAND_PCCARDTimingInitType PCCARD_PIO4_Timing;
verification_result_struct verification_result;
uint8_t Test_Mode = 0;  // 0 -> Memory/PIO mode, 1 -> IO/PIO mode, 2 -> Memory/DMA mode, 3 -> IO/DMA mode , 4....
uint8_t pccard_TxBuffer[PCCARD_SECTOR_SIZE * sizeof(uint8_t)] __attribute__((at(0x20003000)));//when 16bit mode should begain at even addr
uint8_t pccard_RxBuffer[PCCARD_SECTOR_SIZE * sizeof(uint8_t)] __attribute__((at(0x20004000)));
uint8_t pccard_Long_TxBuffer[PCCARD_LONG_SECTOR_SIZE * sizeof(uint8_t)] __attribute__((at(0x20005000)));  //
uint8_t pccard_Long_RxBuffer[PCCARD_LONG_SECTOR_SIZE * sizeof(uint8_t)] __attribute__((at(0x20006000)));  // for Read/Write Long Sector
uint8_t pccard_25_Sectors_TxBuffer[PCCARD_25_SECTORS_SIZES * sizeof(uint8_t)] __attribute__((at(0x20007000)));  //
uint8_t pccard_25_Sectors_RxBuffer[PCCARD_25_SECTORS_SIZES * sizeof(uint8_t)] __attribute__((at(0x2000B000)));  // for Read/Write 2 Sector Test
uint8_t orig_default_Sector;
uint8_t orig_default_head;
uint32_t u32i;
/* Private function prototypes -----------------------------------------------*/
static void Fill_Buffer(uint8_t *pBuffer, uint32_t BufferLenght, uint32_t Offset);
static TestStatus Buffercmp(uint8_t* pBuffer, uint8_t* pBuffer1, uint32_t BufferLength);
static void Print_Buffer(uint8_t* pBuffer, uint32_t uwBufferLenght);
void peripheral_hardware_init(void);

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  AT32_Board_Init();
  UART_Print_Init(115200);
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  printf("\r\nCompact Flash Storage Card Test.please press the KEY....\r\n");

  while(AT32_BUTTON_Press() != BUTTON_WAKEUP);

  pccardHandle.Instance  = XMC_Bank4;
  /*
  192MHz 144MHz 108MHz 72MHz 36MHz
    12     9	   7      5     3
    61     47    36     25    14
    14     11    8      6     3
    0      0     0      0     0

    12     9     7      5     3
    99     75    57     39    21
    12     9     7      5     3
    1      1     1      1     1

    24     18    13     9     5
    61     47    36     25    14
    8      6     5      3     2
    1      1     1      1     1
  */
  PCCARD_PMEM4_Timing.XMC_SetupTime     =  12;
  PCCARD_PMEM4_Timing.XMC_OpTime = 61;
  PCCARD_PMEM4_Timing.XMC_HoldTime = 14 ;
  PCCARD_PMEM4_Timing.XMC_WriteSetupTime  = 0;

  PCCARD_PATT4_Timing.XMC_SetupTime     = 12;
  PCCARD_PATT4_Timing.XMC_OpTime = 99;
  PCCARD_PATT4_Timing.XMC_HoldTime =  12;
  PCCARD_PATT4_Timing.XMC_WriteSetupTime  = 1;

  PCCARD_PIO4_Timing.XMC_SetupTime     = 24;
  PCCARD_PIO4_Timing.XMC_OpTime = 61;
  PCCARD_PIO4_Timing.XMC_HoldTime = 8;
  PCCARD_PIO4_Timing.XMC_WriteSetupTime  = 1;


  pccardHandle.Init.XMC_EnableWait   = 1;
  pccardHandle.Init.XMC_DelayTimeCR = 0;
  pccardHandle.Init.XMC_DelayTimeAR  = 0;
  memset(&verification_result, 0, sizeof(verification_result));

  //////////////////////////////////////////
  // Set I/O or Memory Configuarations	//
  //
  // CF_MEM_MAP_MODE						//
  // CF_IO_16_MODE						//
  // CF_IO_PRICH_MODE (?)					//
  // CF_IO_SECCH_MODE (?)					//
  //////////////////////////////////////////
//  while(Test_Mode < 8)
  while(Test_Mode < 8)
  {
//    Test_Mode=4;//test
    // 0 -> 16-bit Memory/PIO mode, 1 -> 8-bit Memory/PIO mode
    // 2 -> 16-bit IO/PIO mode    , 3 -> 8-bit IO/PIO mode
    // 4 -> 16-bit Memory/DMA mode, 5 -> 8-bit Memory/DMA mode
    // 6 -> 16-bit IO/DMA mode    , 7 -> 8-bit IO/DMA mode
    switch(Test_Mode)
    {
      case 0:
        pccardHandle.CF.Protocol = CF_MEM_MAP_MODE;
        pccardHandle.CF.IOAddr = PCCARD_COMMON_SPACE_ADDRESS;
        pccardHandle.CF.DMAEnable = FALSE;
        pccardHandle.CF.Enable_8_bit_mode = FALSE;
        printf("\r\n16-bit Memory/PIO mode Start... ");
        break;

      case 1:
        pccardHandle.CF.Protocol = CF_MEM_MAP_MODE;
        pccardHandle.CF.IOAddr = PCCARD_COMMON_SPACE_ADDRESS;
        pccardHandle.CF.DMAEnable = FALSE;
        pccardHandle.CF.Enable_8_bit_mode = TRUE;
        printf("\r\n8-bit Memory/PIO mode Start... ");
        break;

      case 2:
        pccardHandle.CF.Protocol = CF_IO_16_MODE;
        pccardHandle.CF.IOAddr = PCCARD_IO_SPACE_ADDRESS;
        pccardHandle.CF.DMAEnable = FALSE;
        pccardHandle.CF.Enable_8_bit_mode = FALSE;
        printf("\r\n16-bit IO/PIO mode Start... ");
        break;

      case 3:
        pccardHandle.CF.Protocol = CF_IO_16_MODE;
        pccardHandle.CF.IOAddr = PCCARD_IO_SPACE_ADDRESS;
        pccardHandle.CF.DMAEnable = FALSE;
        pccardHandle.CF.Enable_8_bit_mode = TRUE;
        printf("\r\n8-bit IO/PIO mode Start... ");
        break;

      case 4:
        pccardHandle.CF.Protocol = CF_MEM_MAP_MODE;
        pccardHandle.CF.IOAddr = PCCARD_COMMON_SPACE_ADDRESS;
        pccardHandle.CF.DMAEnable = TRUE;
        pccardHandle.CF.Enable_8_bit_mode = FALSE;
        printf("\r\n16-bit Memory/DMA mode Start... ");
        break;

      case 5:
        pccardHandle.CF.Protocol = CF_MEM_MAP_MODE;
        pccardHandle.CF.IOAddr = PCCARD_COMMON_SPACE_ADDRESS;
        pccardHandle.CF.DMAEnable = TRUE;
        pccardHandle.CF.Enable_8_bit_mode = TRUE;
        printf("\r\n8-bit Memory/DMA mode Start... ");
        break;

      case 6:
        pccardHandle.CF.Protocol = CF_IO_16_MODE;
        pccardHandle.CF.IOAddr = PCCARD_IO_SPACE_ADDRESS;
        pccardHandle.CF.DMAEnable = TRUE;
        pccardHandle.CF.Enable_8_bit_mode = FALSE;
        printf("\r\n16-bit IO/DMA mode Start... ");
        break;

      case 7:
        pccardHandle.CF.Protocol = CF_IO_16_MODE;
        pccardHandle.CF.IOAddr = PCCARD_IO_SPACE_ADDRESS;
        pccardHandle.CF.DMAEnable = TRUE;
        pccardHandle.CF.Enable_8_bit_mode = TRUE;
        printf("\r\n8-bit IO/DMA mode Start... ");
        break;

      default:
        printf("\r\nSelect Test Mode Failed");
        break;
    }

    pccardHandle.CF.Drive = 0;

    //Init Compact Flash Controller
    CF_print(PRT_INIT_INFO, ("\r\nInit Compact Flash Card..."));

    if(PCCARD_Init(&pccardHandle, &PCCARD_PMEM4_Timing, &PCCARD_PATT4_Timing, &PCCARD_PIO4_Timing) != STATUS_OK)
    {
      printf("\r\nCompact Flash card Init Failed");
      break;
    }

    //Reset Compact Flash card //
    CF_print(PRT_INIT_INFO, ("\r\nReset Compact Flash Card..."));

    if(PCCARD_Reset(&pccardHandle))
    {
      verification_result.ATTRIBUTE_RW_PASS = TRUE;
    }

    CF_print(PRT_INIT_INFO, ("\r\nSet Wait Feature..."));

    if(Enable_Wait_Feature(&pccardHandle) != STATUS_OK)
    {
      printf("\r\nCompact Flash card Set Wait Feature Failed");
      break;
    }

    // Read CF Card CIS from CF card's attribute memory //
    if(ReadCFCardCISInformation(&pccardHandle))
    {
      verification_result.CIS_READ_PASS = TRUE;
    }

    /////////////////////////////////////
    // CF-ATA Command Set Test Started //
    /////////////////////////////////////
    CF_print(PRT_CMD_INFO, ("\r\nVerifying CF_ATA_IDENTIFY_DRIVE..."));

    // Read Compact Flash Card ID (CF_ATA_IDENTIFY_DRIVE) //
    if(PCCARD_Read_ID(&pccardHandle))
    {
      verification_result.ATA_IDENTIFY_DRIVE_PASS = TRUE;
    }

    // Execute Drive Diagnostic //
    CF_print(PRT_CMD_INFO, ("\r\nVerifying CF_ATA_EXECUTE_DRIVE_DIAG..."));

    if(PCCARD_Diagnostic(&pccardHandle))
    {
      verification_result.ATA_EXECUTE_DRIVE_DIAG_PASS = TRUE;
    }

    // Check Power Mode //
    CF_print(PRT_CMD_INFO, ("\r\nVerifying CF_ATA_CHECK_POWER_MODE..."));

    if(PCCARD_Check_Power_Mode(&pccardHandle))
    {
      verification_result.ATA_CHECK_POWER_MODE_PASS = TRUE;
    }

    // Idle Mode //
    CF_print(PRT_CMD_INFO, ("\r\nVerifying CF_ATA_IDLE..."));

    if(PCCARD_Idle(&pccardHandle, 5))
    {
      verification_result.ATA_IDLE_PASS = TRUE;
    }

    // Idle Immediate Mode //
    CF_print(PRT_CMD_INFO, ("\r\nVerifying CF_ATA_IDLE_IMMEDIATE..."));

    if(PCCARD_Idle_Immediate(&pccardHandle))
    {
      verification_result.ATA_IDLE_IMMEDIATE_PASS = TRUE;
    }

    // Set Sleep Mode //
    CF_print(PRT_CMD_INFO, ("\r\nVerifying CF_ATA_SET_SLEEP_MODE..."));

    if(PCCARD_Set_Sleep_Mode(&pccardHandle))
    {
      verification_result.ATA_SET_SLEEP_MODE_PASS = TRUE;
    }

    // Standby Mode //
    CF_print(PRT_CMD_INFO, ("\r\nVerifying CF_ATA_STANDBY..."));

    if(PCCARD_Standby(&pccardHandle))
    {
      verification_result.ATA_STANDBY_PASS = TRUE;
    }

    // Standby Immediate Mode //
    CF_print(PRT_CMD_INFO, ("\r\nVerifying CF_ATA_STANDBY_IMMEDIATE..."));

    if(PCCARD_Standby_Immediate(&pccardHandle))
    {
      verification_result.ATA_STANDBY_IMMEDIATE_PASS = TRUE;
    }

    // Write Compact Flash Card Sector //
    CF_print(PRT_CMD_INFO, ("\r\nVerifying CF_ATA_WRITE_SECTOR..."));
    memset(pccard_TxBuffer, 0, PCCARD_SECTOR_SIZE * sizeof(uint8_t));

    Fill_Buffer(pccard_TxBuffer, PCCARD_SECTOR_SIZE, 0x00);	// Fill the buffer to write

    if(!PCCARD_Write_Sector(&pccardHandle, pccard_TxBuffer, Start_Sector, 1))
    {
      printf("\r\nCompact Flash card Write Secor Failed");
      break;
    }

    // Read Compact Flash Card Sector //
    CF_print(PRT_CMD_INFO, ("\r\nVerifying CF_ATA_READ_SECTOR..."));
    memset(pccard_RxBuffer, 0, PCCARD_SECTOR_SIZE * sizeof(uint8_t));

    if(!PCCARD_Read_Sector(&pccardHandle, pccard_RxBuffer, Start_Sector, 1))
    {
      printf("\r\nCompact Flash card Read Secor Failed");
      break;
    }

    if(Buffercmp(pccard_TxBuffer, pccard_RxBuffer, PCCARD_SECTOR_SIZE) == PASSED)	// Checking data integrity
    {
      verification_result.ATA_READ_SECTOR_PASS = TRUE;
      verification_result.ATA_WRITE_SECTOR_PASS = TRUE;
    }
    else
    {
      verification_result.ATA_READ_SECTOR_PASS = FALSE;
      verification_result.ATA_WRITE_SECTOR_PASS = FALSE;
    }

    Print_Buffer(pccard_RxBuffer, PCCARD_SECTOR_SIZE);

    // Write Compact Flash Card Buffer //
    CF_print(PRT_CMD_INFO, ("\r\nVerifying CF_ATA_WRITE_BUFFER..."));
    memset(pccard_TxBuffer, 0, PCCARD_SECTOR_SIZE * sizeof(uint8_t));

    Fill_Buffer(pccard_TxBuffer, PCCARD_SECTOR_SIZE, 0x00);	// Fill the buffer to write

    if(!PCCARD_Write_Buffer(&pccardHandle, pccard_TxBuffer))
    {
      printf("\r\nCompact Flash card Write Buffer Failed");
      break;
    }

    // Read Compact Flash Card Buffer //
    CF_print(PRT_CMD_INFO, ("\r\nVerifying CF_ATA_READ_BUFFER..."));
    memset(pccard_RxBuffer, 0, PCCARD_SECTOR_SIZE * sizeof(uint8_t));

    if(!PCCARD_Read_Buffer(&pccardHandle, pccard_RxBuffer))
    {
      printf("\r\nCompact Flash card Read Buffer Failed");
      break;
    }

    if(Buffercmp(pccard_TxBuffer, pccard_RxBuffer, PCCARD_SECTOR_SIZE) == PASSED)	// Checking data integrity
    {
      verification_result.ATA_READ_BUFFER_PASS = TRUE;
      verification_result.ATA_WRITE_BUFFER_PASS = TRUE;
    }
    else
    {
      verification_result.ATA_READ_BUFFER_PASS = FALSE;
      verification_result.ATA_WRITE_BUFFER_PASS = FALSE;
    }

    // Request Sense //
    CF_print(PRT_CMD_INFO, ("\r\nVerifying CF_ATA_REQUEST_SENSE..."));

    if(PCCARD_Write_Sector(&pccardHandle, pccard_TxBuffer, pccardHandle.CF.CFCardInfo.Total_Sector + 1, 1))
    {
      printf("\r\nCompact Flash card Write Address Overflow Failed");
      break;
    }

    if(PCCARD_Reuqest_Sense(&pccardHandle))
    {
      verification_result.ATA_REQUEST_SENSE_PASS = TRUE;
    }

    // Recalibrate //
    CF_print(PRT_CMD_INFO, ("\r\nVerifying CF_ATA_RECALIBRATE..."));

    if(PCCARD_Recalibrate(&pccardHandle))
    {
      verification_result.ATA_RECALIBRATE_PASS = TRUE;
    }

    // Initialize Drive Parameters //
    CF_print(PRT_CMD_INFO, ("\r\nVerifying CF_ATA_Init_Drive_Para..."));
    orig_default_Sector = pccardHandle.CF.CFCardInfo.Current_Sector;  //  backup the original default sectors value
    orig_default_head = pccardHandle.CF.CFCardInfo.Current_Head;      //  backup the original default heads value


    pccardHandle.CF.CFCardInfo.Current_Sector -= 1;
    pccardHandle.CF.CFCardInfo.Current_Head -= 1;

    if(!PCCARD_Init_Drive_Para(&pccardHandle, pccardHandle.CF.CFCardInfo.Current_Sector, pccardHandle.CF.CFCardInfo.Current_Head - 1))
    {
      printf("\r\nInit Drive Para fail");
      break;
    }

    if(Vaild_Init_Drive_Para(&pccardHandle, pccardHandle.CF.CFCardInfo.Current_Sector, pccardHandle.CF.CFCardInfo.Current_Head))  //  check the command is vailded
    {
      verification_result.ATA_INIT_DRIVE_PARA_PASS = TRUE;
    }

    pccardHandle.CF.CFCardInfo.Current_Sector = orig_default_Sector;
    pccardHandle.CF.CFCardInfo.Current_Head = orig_default_head;

    if(!PCCARD_Init_Drive_Para(&pccardHandle, orig_default_Sector, orig_default_head - 1))
    {
      printf("\r\nInit Drive Para fail");
      break;
    }

    if(!Vaild_Init_Drive_Para(&pccardHandle, pccardHandle.CF.CFCardInfo.Current_Sector, pccardHandle.CF.CFCardInfo.Current_Head))  //  check restore is vailded
    {
      verification_result.ATA_INIT_DRIVE_PARA_PASS = FALSE;
    }

    // Seek //
    CF_print(PRT_CMD_INFO, ("\r\nVerifying CF_ATA_SEEK..."));

    if(PCCARD_Seek(&pccardHandle, pccardHandle.CF.CFCardInfo.Default_Head - 1, pccardHandle.CF.CFCardInfo.Default_Cylinder + 1))
    {
      verification_result.ATA_SEEK_PASS = TRUE;
    }

    // Read Compact Flash Card Long Sector //
    CF_print(PRT_CMD_INFO, ("\r\nVerifying CF_ATA_READ_LONG_SECTOR..."));
    memset(pccard_Long_RxBuffer, 0, PCCARD_LONG_SECTOR_SIZE * sizeof(uint8_t));

    if(PCCARD_Read_Long_Sector(&pccardHandle, Start_Sector, pccard_Long_RxBuffer))
    {
      verification_result.ATA_READ_LONG_SECTOR_PASS = TRUE;
    }

    // Write Compact Flash Card Long Sector //
    CF_print(PRT_CMD_INFO, ("\r\nVerifying CF_ATA_WRITE_LONG_SECTOR..."));
    memset(pccard_Long_TxBuffer, 0, PCCARD_LONG_SECTOR_SIZE * sizeof(uint8_t));

    if(PCCARD_Write_Long_Sector(&pccardHandle, Start_Sector, pccard_Long_RxBuffer))
    {
      verification_result.ATA_WRITE_LONG_SECTOR_PASS = TRUE;
    }

    // Erase Compact Flash Card Sector //
    CF_print(PRT_CMD_INFO, ("\r\nVerifying CF_ATA_ERASE_SECTOR..."));
    memset(pccard_TxBuffer, 0, PCCARD_SECTOR_SIZE * sizeof(uint8_t));

    Fill_Buffer(pccard_TxBuffer, PCCARD_SECTOR_SIZE, 0x00);	// Fill the buffer to write

    if(!PCCARD_Write_Sector(&pccardHandle, pccard_TxBuffer, Start_Sector, 1))
    {
      printf("\r\nCompact Flash card Write Secor Failed");
      break;
    }

    if(!PCCARD_Erase_Sector(&pccardHandle, Start_Sector, 1))
    {
      printf("\r\nCompact Flash card Erase Secor Failed");
      break;
    }

    memset(pccard_RxBuffer, 0, PCCARD_SECTOR_SIZE * sizeof(uint8_t));

    if(!PCCARD_Read_Sector(&pccardHandle, pccard_RxBuffer, Start_Sector, 1))
    {
      printf("\r\nCompact Flash card Read Secor Failed");
      break;
    }
    else
    {
      verification_result.ATA_ERASE_SECTOR_PASS = TRUE;

      for(u32i = 0; u32i < PCCARD_SECTOR_SIZE; u32i++)
      {
        if(*(pccard_RxBuffer + u32i) != 0x0)
        {
          verification_result.ATA_ERASE_SECTOR_PASS = FALSE;
          CF_print(PRT_ERR_INFO, ("\r\nErase sector error at address offset 0x%0.2x:(0x%0.2x)", u32i, *(pccard_RxBuffer + u32i)));
          break;
        }
      }
    }

    // Write Compact Flash Card Write Sector Without Erase //
    CF_print(PRT_CMD_INFO, ("\r\nVerifying CF_ATA_WRITE_SECTOR_WO_ERASE..."));

    if(!PCCARD_Erase_Sector(&pccardHandle, Start_Sector, 1))
    {
      printf("\r\nCompact Flash card Erase Secor Failed");
      break;
    }

    memset(pccard_TxBuffer, 0, PCCARD_SECTOR_SIZE * sizeof(uint8_t));

    Fill_Buffer(pccard_TxBuffer, PCCARD_SECTOR_SIZE, 0x00);	// Fill the buffer to write

    if(!PCCARD_Write_Sector_WO_ERASE(&pccardHandle, pccard_TxBuffer, Start_Sector, 1))
    {
      printf("\r\nCompact Flash card Write Secor Without Erase Failed");
      break;
    }

    memset(pccard_RxBuffer, 0, PCCARD_SECTOR_SIZE * sizeof(uint8_t));

    if(!PCCARD_Read_Sector(&pccardHandle, pccard_RxBuffer, Start_Sector, 1))
    {
      printf("\r\nCompact Flash card Read Secor Failed");
      break;
    }

    if(Buffercmp(pccard_TxBuffer, pccard_RxBuffer, PCCARD_SECTOR_SIZE) == PASSED)	// Checking data integrity
    {
      verification_result.ATA_WRITE_SECTOR_WO_ERASE_PASS = TRUE;
    }
    else
    {
      verification_result.ATA_WRITE_SECTOR_WO_ERASE_PASS = FALSE;
    }

    // Format Track //
    CF_print(PRT_CMD_INFO, ("\r\nVerifying CF_ATA_FORMAT_TRACK..."));
    memset(pccard_TxBuffer, 0, PCCARD_SECTOR_SIZE * sizeof(uint8_t));

    Fill_Buffer(pccard_TxBuffer, PCCARD_SECTOR_SIZE, 0x00);	// Fill the buffer to write

    if(!PCCARD_Write_Sector(&pccardHandle, pccard_TxBuffer, Start_Sector, 1))
    {
      printf("\r\nCompact Flash card Write Secor Failed");
      break;
    }

    if(!PCCARD_Format_Track(&pccardHandle, pccard_TxBuffer, Start_Sector, 1))
    {
      printf("\r\nCompact Flash card Format Track Failed");
      break;
    }

    memset(pccard_RxBuffer, 0, PCCARD_SECTOR_SIZE * sizeof(uint8_t));

    if(!PCCARD_Read_Sector(&pccardHandle, pccard_RxBuffer, Start_Sector, 1))
    {
      printf("\r\nCompact Flash card Read Secor Failed");
      break;
    }
    else
    {
      verification_result.ATA_FORMAT_TRACK_PASS = TRUE;

      for(u32i = 0; u32i < PCCARD_SECTOR_SIZE; u32i++)
      {
        if(!(*(pccard_RxBuffer + u32i) == 0xff || *(pccard_RxBuffer + u32i) == 0x0))
        {
          verification_result.ATA_FORMAT_TRACK_PASS = FALSE;
          CF_print(PRT_ERR_INFO, ("\r\nFormat track error at address offset 0x%0.2x:(0x%0.2x)", u32i, *(pccard_RxBuffer + u32i)));
          break;
        }
      }
    }

    // Read Verify Sector //
    CF_print(PRT_CMD_INFO, ("\r\nVerifying CF_ATA_READ_VERIFY_SECTOR..."));

    if(PCCARD_Read_Verify_Sector(&pccardHandle, Start_Sector, 1))
    {
      verification_result.ATA_READ_VERIFY_SECTOR_PASS = TRUE;
    }

    // Write verify //
    CF_print(PRT_CMD_INFO, ("\r\nVerifying CF_ATA_WRITE_VERIFY..."));
    memset(pccard_TxBuffer, 0, PCCARD_SECTOR_SIZE * sizeof(uint8_t));

    Fill_Buffer(pccard_TxBuffer, PCCARD_SECTOR_SIZE, 0x00);	// Fill the buffer to write

    if(!PCCARD_Write_Verify(&pccardHandle, pccard_TxBuffer, Start_Sector, 1))
    {
      printf("\r\nCompact Flash card Write Verify Failed");
      break;
    }

    // Read Compact Flash Card Sector //
    memset(pccard_RxBuffer, 0, PCCARD_SECTOR_SIZE * sizeof(uint8_t));

    if(!PCCARD_Read_Sector(&pccardHandle, pccard_RxBuffer, Start_Sector, 1))
    {
      printf("\r\nCompact Flash card Read Secor Failed");
      break;
    }

    if(Buffercmp(pccard_TxBuffer, pccard_RxBuffer, PCCARD_SECTOR_SIZE) == PASSED)	// Checking data integrity
    {
      verification_result.ATA_WRITE_VERIFY_PASS = TRUE;
    }
    else
    {
      verification_result.ATA_WRITE_VERIFY_PASS = FALSE;
    }

    // Translate Sector //
    CF_print(PRT_CMD_INFO, ("\r\nVerifying CF_ATA_TRANSLATE_SECTOR..."));

    if(!PCCARD_Translate_Sector(&pccardHandle, Start_Sector))
    {
      printf("\r\nCompact Flash card Translate Sector Failed");
      break;
    }

    memset(pccard_RxBuffer, 0, PCCARD_SECTOR_SIZE * sizeof(uint8_t));

    if(!PCCARD_Read_Buffer(&pccardHandle, pccard_RxBuffer))
    {
      printf("\r\nCompact Flash card Read Buffer Failed");
      break;
    }

    if((pccard_RxBuffer[1]) == pccardHandle.CF.CFAddr.Cylinder &&
        (pccard_RxBuffer[2]) == pccardHandle.CF.CFAddr.Head &&
        (pccard_RxBuffer[3]) == pccardHandle.CF.CFAddr.Sector)
    {
      verification_result.ATA_TRANSLATE_SECTOR_PASS = TRUE;
    }

    // Set Multiple Mode //
    CF_print(PRT_CMD_INFO, ("\r\nVerifying CF_ATA_SET_MULTIPLE_MODE..."));
    PCCARD_Set_Multiple_Mode(&pccardHandle, pccardHandle.CF.CFCardInfo.Max_Mutiple_Sector + 1);

    if(PCCARD_Read_ERROR_REG(&pccardHandle) & ABRT_BIT)
    {
      verification_result.ATA_SET_MULTIPLE_MODE_PASS = TRUE;
    }

    PCCARD_Set_Multiple_Mode(&pccardHandle, pccardHandle.CF.CFCardInfo.Max_Mutiple_Sector);

    if(PCCARD_Read_ERROR_REG(&pccardHandle))
    {
      verification_result.ATA_SET_MULTIPLE_MODE_PASS = FALSE;
    }

    // Set Feature //
    CF_print(PRT_CMD_INFO, ("\r\nVerifying CF_ATA_SET_FEATURE..."));

    // send a valid feature to CF card, we expect there are no bit will be assert in Error register .
    PCCARD_Set_Features(&pccardHandle, 0, NOP);

    if(PCCARD_Read_ERROR_REG(&pccardHandle))
    {
      verification_result.ATA_SET_FEATURE_PASS = FALSE;
    }
    else
    {
      verification_result.ATA_SET_FEATURE_PASS = TRUE;
    }

    // Write Multiple //
    CF_print(PRT_CMD_INFO, ("\r\nVerifying CF_ATA_WRITE_MULTIPLE..."));
    // Note that the maximun number of sectors on Read/Write Multiple command in our CF test card is 1,
    // so it will work same as Read/Write Sector command.
    memset(pccard_TxBuffer, 0, PCCARD_SECTOR_SIZE * sizeof(uint8_t));

    Fill_Buffer(pccard_TxBuffer, PCCARD_SECTOR_SIZE, 0x00);	// Fill the buffer to write

    if(!PCCARD_Write_Multiple(&pccardHandle, pccard_TxBuffer, Start_Sector, 1))
    {
      printf("\r\nCompact Flash card Write Multiple Secors Failed");
      break;
    }

    // Read Multiple //
    CF_print(PRT_CMD_INFO, ("\r\nVerifying CF_ATA_READ_MULTIPLE..."));
    // Note that the maximun number of sectors on Read/Write Multiple command in our CF test card is 1,
    // so it will work same as Read/Write Sector command.
    memset(pccard_RxBuffer, 0, PCCARD_SECTOR_SIZE * sizeof(uint8_t));

    if(!PCCARD_Read_Multiple(&pccardHandle, pccard_RxBuffer, Start_Sector, 1))
    {
      printf("\r\nCompact Flash card Read Multiple Secors Failed");
      break;
    }

    if(Buffercmp(pccard_TxBuffer, pccard_RxBuffer, PCCARD_SECTOR_SIZE) == PASSED)	// Checking data integrity
    {
      verification_result.ATA_READ_MULTIPLE_PASS = TRUE;
      verification_result.ATA_WRITE_MULTIPLE_PASS = TRUE;
    }
    else
    {
      verification_result.ATA_READ_MULTIPLE_PASS = FALSE;
      verification_result.ATA_WRITE_MULTIPLE_PASS = FALSE;
    }

    // Write Multiple Without Erase //
    CF_print(PRT_CMD_INFO, ("\r\nVerifying CF_ATA_WRITE_MULTIPLE_WO_ERASE..."));

    // Note that the maximun number of sectors on Read/Write Multiple command in our CF test card is 1,
    // so it will work same as Read/Write Sector command.
    if(!PCCARD_Erase_Sector(&pccardHandle, Start_Sector, 1))
    {
      printf("\r\nCompact Flash card Erase Secor Failed");
      break;
    }

    memset(pccard_TxBuffer, 0, PCCARD_SECTOR_SIZE * sizeof(uint8_t));

    Fill_Buffer(pccard_TxBuffer, PCCARD_SECTOR_SIZE, 0x00);	// Fill the buffer to write

    if(!PCCARD_Write_Multiple_WO_ERASE(&pccardHandle, pccard_TxBuffer, Start_Sector, 1))
    {
      printf("\r\nCompact Flash card Write Multiple Secors Without Erase Failed");
      break;
    }

    memset(pccard_RxBuffer, 0, PCCARD_SECTOR_SIZE * sizeof(uint8_t));

    if(!PCCARD_Read_Multiple(&pccardHandle, pccard_RxBuffer, Start_Sector, 1))
    {
      printf("\r\nCompact Flash card Read Multiple Secors Failed");
      break;
    }

    if(Buffercmp(pccard_TxBuffer, pccard_RxBuffer, PCCARD_SECTOR_SIZE) == PASSED)	// Checking data integrity
    {
      verification_result.ATA_WRITE_MULTIPLE_WO_ERASE_PASS = TRUE;
    }
    else
    {
      verification_result.ATA_WRITE_MULTIPLE_WO_ERASE_PASS = FALSE;
    }


    //  disable the Read/Write Multiple command, we expect Abort bit in Error register will be assert.
    if(!PCCARD_Set_Multiple_Mode(&pccardHandle, 0))
    {
      printf("\r\nSet multiple mode to 0 Failed");
      break;
    }

    // Test Read/Write 50 sectors //
    CF_print(PRT_CMD_INFO, ("\r\nVerifying Test Read/Write 50 sectors..."));

    memset(pccard_25_Sectors_TxBuffer, 0, PCCARD_25_SECTORS_SIZES * sizeof(uint8_t));

    Fill_Buffer(pccard_25_Sectors_TxBuffer, PCCARD_25_SECTORS_SIZES, 0x78);	// Fill the buffer to write



    if(!PCCARD_Write_Sector(&pccardHandle, pccard_25_Sectors_TxBuffer, Start_Sector, 25))
    {
      printf("\r\nCompact Flash card Write 50 Secors Failed");
      break;
    }

    memset(pccard_25_Sectors_RxBuffer, 0, PCCARD_25_SECTORS_SIZES * sizeof(uint8_t));

    if(!PCCARD_Read_Sector(&pccardHandle, pccard_25_Sectors_RxBuffer, Start_Sector, 25))
    {
      printf("\r\nCompact Flash card Read 50 Secors Failed");
      break;
    }

    if(Buffercmp(pccard_25_Sectors_TxBuffer, pccard_25_Sectors_RxBuffer, PCCARD_25_SECTORS_SIZES) == PASSED)	// Checking data integrity
    {
      verification_result.ACESS_25_SECTORS_PASS = TRUE;
    }
    else
    {
      verification_result.ACESS_25_SECTORS_PASS = FALSE;
    }

    // Show the Compact Flash Storage Card Test results //
    show_verification_result(&verification_result);

    Test_Mode++;
  }

  printf("\r\nCompact Flash Storage Card Test End");

  // Infinite loop //
  while (1)
  {

  }
}

/**
  * @brief  Fills buffer with user predefined data.
  * @param  pBuffer: pointer on the buffer to fill
  * @param  uwBufferLenght: size of the buffer to fill
  * @param  uwOffset: first value to fill on the buffer
  * @retval None
  */
static void Fill_Buffer(uint8_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset)
{
  uint32_t index = 0;

  /* Put in global buffer same values */
  for (index = 0; index < uwBufferLenght; index++ )
  {
    pBuffer[index] = index + uwOffset;
  }
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer: pointer to the buffers.
  * @param  pBuffer1: pointer to the buffers1.
  * @param  uwBufferLenght: Compared buffer's length
  * @retval 1: pBuffer identical to pBuffer1
  *         0: pBuffer differs from pBuffer1
  */
static TestStatus Buffercmp(uint8_t* pBuffer, uint8_t* pBuffer1, uint32_t uwBufferLenght)
{
  uint32_t counter = 0;

  while(uwBufferLenght--)
  {
    if(*pBuffer != *pBuffer1)
    {
      printf("\r\nFormat track error at address offset 0x%0.2x:(0x%0.2x) <-> (0x%0.2x)", counter, *(pBuffer), *(pBuffer1));
      return FAILED;
    }

    pBuffer++;
    pBuffer1++;
    counter++;
  }
  return PASSED;
}

/**
  * @brief  print the buffer.
  * @param  pBuffer: pointer to the buffer.
  * @param  uwBufferLenght: buffer's length.
  * @retval None.
  */
static void Print_Buffer(uint8_t* pBuffer, uint32_t uwBufferLenght)
{
#if 0
  uint32_t counter = 0;
  printf("\r\n");

  while(uwBufferLenght--)
  {
    printf("%#x ", *pBuffer);

    if(uwBufferLenght % 16 == 0)
    {
      printf("\r\n");
    }

    pBuffer++;
    counter++;
  }
#endif
}

/**
  * @}
  */

/**
  * @}
  */
