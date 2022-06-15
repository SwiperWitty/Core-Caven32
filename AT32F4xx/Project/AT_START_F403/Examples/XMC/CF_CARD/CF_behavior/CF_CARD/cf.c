/*
  **************************************************************************
  * File   : cf.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : CF card operation source file.
  **************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "cf.h"
#include "at32_board.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup XMC_CF_CARD
  * @{
  */

DMA_InitType DMA_InitStructure;
u16 DMA1_MEM_LEN;
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @defgroup PCCARD_Private_Constants PCCARD Private Constants
  * @{
  */

#define PCCARD_TIMEOUT_READ_ID      (uint32_t)0x0000FFFF
#define PCCARD_TIMEOUT_SECTOR       (uint32_t)0x0000FFFF
#define PCCARD_TIMEOUT_STATUS       (uint32_t)0x01000000

#define PCCARD_STATUS_OK            (uint8_t)0x58
#define PCCARD_STATUS_WRITE_OK      (uint8_t)0x50
/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions ---------------------------------------------------------*/

/** @defgroup PCCARD_Exported_Functions PCCARD Exported Functions
  * @{
  */

/** @defgroup PCCARD_Exported_Functions_Group1 Initialization and de-initialization functions
  * @brief    Initialization and Configuration functions
  *
  @verbatim
  ==============================================================================
          ##### PCCARD Initialization and de-initialization functions #####
  ==============================================================================
  [..]
    This section provides functions allowing to initialize/de-initialize
    the PCCARD memory

@endverbatim
  * @{
  */
  /**
  * @brief  DMA configuration
  * @param  DMA_CHx: pointer to a DMA_Channel_Type structure
  * @param  cpar_src: the Memory 1 addr
  * @param  cmar_dst: the Memory 2 addr
  * @param  cndtr: transfer size
  * @param  Transfer_Type: 
  *     @arg Read_Transfer: from Memory 1 to Memory 2
  *     @arg Write_Transfer: from Memory 2 to Memory 1
  * @param  Transfer_Width: BYTE or HALFWORD
  * @retval None
  */
void MYDMA_Config(DMA_Channel_Type* DMA_CHx, uint32_t cpar_src, uint32_t cmar_dst, uint32_t cndtr, uint8_t Transfer_Type, uint8_t Transfer_Width)
{
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA1, ENABLE);

  DMA_Reset(DMA_CHx);

  if(Transfer_Type == Read_Transfer)
  {
    DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;
    DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE;
  }
  else
  {
    DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_ENABLE;
    DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_DISABLE;
  }

  if(Transfer_Width == Enable_8_bit_Transfer)
  {
    DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_BYTE;
    DMA_InitStructure.DMA_MemoryDataWidth = DMA_PERIPHERALDATAWIDTH_BYTE;
  }
  else
  {
    DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_HALFWORD;
    DMA_InitStructure.DMA_MemoryDataWidth = DMA_MEMORYDATAWIDTH_HALFWORD;
  }

  DMA1_MEM_LEN = cndtr;
  DMA_InitStructure.DMA_PeripheralBaseAddr = cpar_src;
  DMA_InitStructure.DMA_MemoryBaseAddr = cmar_dst;
  DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALSRC;
  DMA_InitStructure.DMA_BufferSize = cndtr;
  DMA_InitStructure.DMA_Mode = DMA_MODE_NORMAL;
  DMA_InitStructure.DMA_Priority = DMA_PRIORITY_MEDIUM;
  DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_ENABLE;
  DMA_Init(DMA_CHx, &DMA_InitStructure);

  DMA_ChannelEnable(DMA_CHx, ENABLE);
}
/**
  * @brief  XMC InitCtrl
  * @param  Device: pointer to a XMC_Bank4_Type structure
  * @param  Init: pointer to a XMC_PCCARDInitType structure
  * @param  Set_reg_8_bit: choose 8/16bit
  * @retval STATUS_OK or STATUS_ERROR
  */
StatusType XMC_PCCARD_InitCtrl(XMC_Bank4_Type *Device, XMC_PCCARDInitType *Init, uint8_t Set_reg_8_bit)
{
  uint32_t select_PWID;

  if(Set_reg_8_bit == 1)
  {
    select_PWID = XMC_BusType_8b;
  }
  else
  {
    select_PWID = XMC_BusType_16b;
  }

  /* Check the parameters */
  assert_param(IS_XMC_PCCARD_DEVICE(Device));
  assert_param(IS_XMC_WAIT_OPERATION(Init->Waitfeature));
  assert_param(IS_XMC_DELAY_CR_TIME(Init->TCLRSetupTime));
  assert_param(IS_XMC_DELAY_AR_TIME(Init->TARSetupTime));

  /* Set XMC_PCCARD device control parameters */
  REG_CHANGE(Device->BK4CTRL,                                  									\
             (XMC_BK4CTRL_DEV | XMC_BK4CTRL_BUSTYPE  | XMC_BK4CTRL_DLYCR | XMC_BK4CTRL_DLYAR), 	\
             (XMC_BKxCTRL_DEV_PCCARD | select_PWID | (Init->XMC_DelayTimeCR << 9) | (Init->XMC_DelayTimeAR << 13)));

  return STATUS_OK;

}
/**
  * @brief  XMC Enable Wait Feature
  * @param  Device: pointer to a XMC_Bank4_Type structure
  * @param  Init: pointer to a XMC_PCCARDInitType structure
  * @retval STATUS_OK or STATUS_ERROR
  */
StatusType XMC_Enable_Wait_Feature(XMC_Bank4_Type *Device, XMC_PCCARDInitType *Init)
{
  assert_param(IS_XMC_WAIT_OPERATION(Init->Waitfeature));

  REG_CHANGE(Device->BK4CTRL, (XMC_BK4CTRL_WAITEN), ((Init->XMC_EnableWait << 1)));

  return STATUS_OK;
}
/**
  * @brief  Set 16bit_Feature
  * @param  Device: pointer to a XMC_Bank4_Type structure
  * @param  Init: pointer to a XMC_PCCARDInitType structure
  * @param  Set_reg_8_bit: choose 8/16bit
  * @retval HAL status
  */
StatusType XMC_Enable_16bit_Feature(XMC_Bank4_Type *Device, XMC_PCCARDInitType *Init, uint8_t Set_reg_8_bit)
{
  uint32_t select_PWID;

  if(Set_reg_8_bit == 1)
  {
    select_PWID = XMC_BusType_8b;
  }
  else
  {
    select_PWID = XMC_BusType_16b;
  }

  REG_CHANGE(Device->BK4CTRL, (XMC_BK4CTRL_BUSTYPE), ((select_PWID << 4)));

  return STATUS_OK;
}
/**
  * @brief  Init CommonSpace Timing
  * @param  Device: pointer to a XMC_Bank4_Type structure
  * @param  Timing: pointer to a Common space timing structure
  * @retval HAL status
  */
StatusType XMC_PCCARD_InitCommonSpaceTiming(XMC_Bank4_Type *Device, XMC_NAND_PCCARDTimingInitType *Timing)
{
  /* Check the parameters */
  assert_param(IS_XMC_PCCARD_DEVICE(Device));
  assert_param(IS_XMC_SETUP_TIME(Timing->SetupTime));
  assert_param(IS_XMC_OP_TIME(Timing->WaitSetupTime));
  assert_param(IS_XMC_HOLD_TIME(Timing->HoldSetupTime));
  assert_param(IS_XMC_WRITE_SETUP_TIME(Timing->HiZSetupTime));

  /* Set PCCARD timing parameters */
  REG_CHANGE(Device->BK4TMGMEM, BKxTMGMEM_CLEAR_MASK,                            \
             (Timing->XMC_SetupTime                                              | \
              ((Timing->XMC_OpTime) << 8)  | \
              ((Timing->XMC_HoldTime) << 16)  | \
              ((Timing->XMC_WriteSetupTime) << 24)));

  return STATUS_OK;
}
/**
  * @brief  Init AttributeSpace Timing
  * @param  Device: pointer to a XMC_Bank4_Type structure
  * @param  Timing: pointer to a Attribute space timing structure
  * @retval HAL status
  */
StatusType XMC_PCCARD_InitAttributeSpaceTiming(XMC_Bank4_Type *Device, XMC_NAND_PCCARDTimingInitType *Timing)
{
  /* Check the parameters */
  assert_param(IS_XMC_PCCARD_DEVICE(Device));
  assert_param(IS_XMC_SETUP_TIME(Timing->SetupTime));
  assert_param(IS_XMC_OP_TIME(Timing->WaitSetupTime));
  assert_param(IS_XMC_HOLD_TIME(Timing->HoldSetupTime));
  assert_param(IS_XMC_WRITE_SETUP_TIME(Timing->HiZSetupTime));

  /* Set PCCARD timing parameters */
  REG_CHANGE(Device->BK4TMGATT, BKxTMGATT_CLEAR_MASK,                          \
             (Timing->XMC_SetupTime                                              | \
              ((Timing->XMC_OpTime) << 8)  | \
              ((Timing->XMC_HoldTime) << 16)   | \
              ((Timing->XMC_WriteSetupTime) << 24)));

  return STATUS_OK;
}
/*****************************************************************/
/**
  * @brief  Init IOSpace Timing
  * @param  Device: pointer to a XMC_Bank4_Type structure
  * @param  Timing: pointer to a IO space timing structure
  * @retval HAL status
  */
StatusType XMC_PCCARD_InitIOSpaceTiming(XMC_Bank4_Type *Device, XMC_NAND_PCCARDTimingInitType *Timing)
{
  /* Check the parameters */
  assert_param(IS_XMC_PCCARD_DEVICE(Device));
  assert_param(IS_XMC_SETUP_TIME(Timing->SetupTime));
  assert_param(IS_XMC_OP_TIME(Timing->WaitSetupTime));
  assert_param(IS_XMC_HOLD_TIME(Timing->HoldSetupTime));
  assert_param(IS_XMC_WRITE_SETUP_TIME(Timing->HiZSetupTime));

  /* Set XMC_PCCARD device timing parameters */
  REG_CHANGE(Device->BK4TMGIO, BK4TMGIO_CLEAR_MASK,                         \
             (Timing->XMC_SetupTime                                            | \
              (Timing->XMC_OpTime   << 8) | \
              (Timing->XMC_HoldTime   << 16) | \
              (Timing->XMC_WriteSetupTime    << 24)));

  return STATUS_OK;
}
/*****************************************************************/
/**
  * @brief  Perform the PCCARD memory Initialization sequence
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @param  ComSpaceTiming: Common space timing structure
  * @param  AttSpaceTiming: Attribute space timing structure
  * @param  IOSpaceTiming: IO space timing structure
  * @retval HAL status
  */
StatusType PCCARD_Init(PCCARD_HandleType *hpccard, XMC_NAND_PCCARDTimingInitType *ComSpaceTiming, XMC_NAND_PCCARDTimingInitType *AttSpaceTiming, XMC_NAND_PCCARDTimingInitType *IOSpaceTiming)
{
  GPIO_InitType  GPIO_InitStructure;
  uint8_t Set_reg_8_bit;

  /* Check the PCCARD controller state */
  if(hpccard == NULL)
  {
    return STATUS_ERROR;
  }

// if(hpccard->CF.Enable_8_bit_mode == TRUE)
  Set_reg_8_bit = 1;
// else
//	  Set_reg_8_bit = 0;


  if(hpccard->State == PCCARD_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hpccard->Lock = UNLOCKED;

    /* Initialize the low level hardware (MSP) */
    PCCARD_MspInit(hpccard);
  }

  /* Initialize the PCCARD state */
  hpccard->State = PCCARD_STATE_BUSY;

//XMC_PCCARDInit(&hpccard->Init);
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOD | RCC_APB2PERIPH_GPIOE | RCC_APB2PERIPH_GPIOF | RCC_APB2PERIPH_GPIOG, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_XMC, ENABLE);


  // AF_PP -> PD 0,1,4,5,8,9,10,14,15
  GPIO_InitStructure.GPIO_Pins = 0xC733;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  // AF_PP -> PE 7-15
  GPIO_InitStructure.GPIO_Pins = 0xFF80;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  // AF_PP -> PF 0-8,12,13,14,15
  GPIO_InitStructure.GPIO_Pins = 0xF1FF;
  GPIO_Init(GPIOF, &GPIO_InitStructure);

  // AF_PP -> PG 0,10,11
  GPIO_InitStructure.GPIO_Pins = 0x0C01;
  GPIO_Init(GPIOG, &GPIO_InitStructure);

  // IN_FLOATING -> PD
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_6; // XMC_NWAIT
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  // IN_FLOATING -> PF
  GPIO_InitStructure.GPIO_Pins = 0xE00; // PF_9 -> XMC_CD, PF_10 -> XMC_INTR, PF_11 -> XMC_NIOS16
  GPIO_Init(GPIOF, &GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pins_7;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);

// Initialize PCCARD control Interface //
  XMC_PCCARD_InitCtrl(hpccard->Instance, &(hpccard->Init), Set_reg_8_bit);

// Init PCCARD common space timing Interface //
  XMC_PCCARD_InitCommonSpaceTiming(hpccard->Instance, ComSpaceTiming);

// Init PCCARD attribute space timing Interface //
  XMC_PCCARD_InitAttributeSpaceTiming(hpccard->Instance, AttSpaceTiming);

// Init PCCARD IO space timing Interface //
  XMC_PCCARD_InitIOSpaceTiming(hpccard->Instance, IOSpaceTiming);

// Enable the PCCARD device //
  XMC_PCCARDCmd(ENABLE);

// Update the PCCARD state //
  hpccard->State = PCCARD_STATE_READY;

  return STATUS_OK;

}
/**
  * @brief  Enable Wait Feature
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @retval STATUS_OK or STATUS_ERROR
  */
StatusType Enable_Wait_Feature(PCCARD_HandleType *hpccard)
{
  /* Check the PCCARD controller state */
  if(hpccard == NULL)
  {
    return STATUS_ERROR;
  }

  if(hpccard->State == PCCARD_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hpccard->Lock = UNLOCKED;

    /* Initialize the low level hardware (MSP) */
    PCCARD_MspInit(hpccard);
  }

  /* Initialize the PCCARD state */
  hpccard->State = PCCARD_STATE_BUSY;

  XMC_Enable_Wait_Feature(hpccard->Instance, &(hpccard->Init));

  hpccard->State = PCCARD_STATE_READY;

  return STATUS_OK;
}

/**
  * @brief  Perform the PCCARD memory De-initialization sequence
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @retval HAL status
  */
StatusType  PCCARD_DeInit(PCCARD_HandleType *hpccard)
{
  /* De-Initialize the low level hardware (MSP) */
  PCCARD_MspDeInit(hpccard);

  /* Configure the PCCARD registers with their reset values */
//XMC_PCCARD_Reset(hpccard->Instance);
  XMC_PCCARDReset();

  /* Update the PCCARD controller state */
  hpccard->State = PCCARD_STATE_RESET;
  return STATUS_OK;
}

/**
  * @brief  PCCARD MSP Init
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @retval None
  */
__weak void PCCARD_MspInit(PCCARD_HandleType *hpccard)
{
  /* Prevent unused argument(s) compilation warning */
//  UNUSED(hpccard);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the PCCARD_MspInit could be implemented in the user file
   */
}

/**
  * @brief  PCCARD MSP DeInit
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @retval None
  */
__weak void PCCARD_MspDeInit(PCCARD_HandleType *hpccard)
{
  /* Prevent unused argument(s) compilation warning */
//  UNUSED(hpccard);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the PCCARD_MspDeInit could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup PCCARD_Exported_Functions_Group2 Input Output and memory functions
  * @brief    Input Output and memory control functions
  *
  @verbatim
  ==============================================================================
                ##### PCCARD Input Output and memory functions #####
  ==============================================================================
  [..]
    This section provides functions allowing to use and control the PCCARD memory

@endverbatim
  * @{
  */
/**
  * @brief  Identify
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @retval TRUE or FALSE.
  */
BOOL PCCARD_Identify(PCCARD_HandleType *hpccard)
{
  uint8_t Reg;

  Reg = hpccard->CF.CFAddr.Drv;

  *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_CARD_HEAD)     = Reg;

  if(!CF_SendCommand(hpccard, ATA_IDENTIFY_DRIVE_CMD))
  {
    return FALSE;
  }

  return TRUE;
}


/**
  * @brief  Read Compact Flash's ID.
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @param  CompactFlash_ID: Compact flash ID structure.
  * @retval HAL status
  *
  */
BOOL PCCARD_Read_ID(PCCARD_HandleType *hpccard)
{
  uint8_t CardInfo[PCCARD_SECTOR_SIZE * sizeof(uint8_t)] = {0};

  memset(CardInfo, 0, PCCARD_SECTOR_SIZE * sizeof(uint8_t));
  PCCARD_Read_STATUS_REG(hpccard);

  if(PCCARD_Identify(hpccard))
  {
    while(1)
    {
      if(TaskFileRegIsValid(hpccard))
      {
        if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT | DRQ_BIT))
        {
          // Read CF ID bytes //

          Read_Sector(hpccard, CardInfo, PCCARD_SECTOR_SIZE);

          break;
        }
        else if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT | ERR_BIT))
        {
          printf("\r\nCommand Pass, but ERR bit is high");
          printf("\r\nCF Status Reg=%#x", PCCARD_Read_STATUS_REG(hpccard));
          printf("\r\nCF Error  Reg=%#x", PCCARD_Read_ERROR_REG(hpccard));
          return FALSE;
        }
      }
    }
  }

  while(1)
  {
    if(TaskFileRegIsValid(hpccard))
    {
      if((PCCARD_Read_STATUS_REG(hpccard) & (RDY_BIT | DSC_BIT | ERR_BIT)) == 0x50)
      {
        break;
      }
      else if((PCCARD_Read_STATUS_REG(hpccard) & (RDY_BIT | DSC_BIT | ERR_BIT)) == 0x51)
      {
        printf("\r\nSend Command Failed");
        printf("\r\nCF Status Reg=%#x", PCCARD_Read_STATUS_REG(hpccard));
        printf("\r\nCF Error  Reg=%#x", PCCARD_Read_ERROR_REG(hpccard));
        return FALSE;
      }
    }
  }

#if 0
  printf("\r\n===============================================================\r\n");

  for(i = 0; i < 512; i++)
  {
    printf("0x%x ", CardInfo[i]);

    if(i % 16 == 0)
    {
      printf("\r\n");
    }
  }

  printf("\r\n===============================================================\r\n");
#endif
#if 1

  printf("\r\n===============================================================\n");
  printf("\r\n\nIdentify Drive Information : ");
  printf("\r\n%s%#x", "Signature : ", ((uint16_t)CardInfo[0] | (uint16_t)CardInfo[1] << 8));
  printf("\r\n%s%u", "Default number of cylinders : ", ((uint16_t)CardInfo[2] | (uint16_t)CardInfo[3] << 8));
  printf("\r\n%s%u", "Default number of heads : ", ((uint16_t)CardInfo[6] | (uint16_t)CardInfo[7] << 8));
  printf("\r\n%s%u", "Number of unformatted bytes per tract : ", ((uint16_t)CardInfo[8] | (uint16_t)CardInfo[9] << 8));
  printf("\r\n%s%u", "Number of unformatted bytes per sector : ", ((uint16_t)CardInfo[10] | (uint16_t)CardInfo[11] << 8));
  printf("\r\n%s%u", "Default number of sectors per track : ", ((uint16_t)CardInfo[12] | (uint16_t)CardInfo[13] << 8));
  printf("\r\n%s%u", "Number of sectors per card : ", (uint32_t)((uint16_t)CardInfo[16] | (uint16_t)CardInfo[17] << 8) | ((uint32_t)((uint16_t)CardInfo[14] | (uint16_t)CardInfo[15] << 8) << 16));
  printf("\r\n%s%u", "Vendor Unique : ", ((uint16_t)CardInfo[18] | (uint16_t)CardInfo[19] << 8));
//	printf("\r\n%s ", "Serial number in ASCII :");
  printf("\r\n%s%#x", "Buffer type : ", ((uint16_t)CardInfo[40] | (uint16_t)CardInfo[41] << 8));
  printf("\r\n%s%u", "Buffer size in 512 byte increments : ", ((uint16_t)CardInfo[42] | (uint16_t)CardInfo[43] << 8));
  printf("\r\n%s%u", "#of ECC bytes passed on Read/Write Long Commands : ", ((uint16_t)CardInfo[44] | (uint16_t)CardInfo[45] << 8));
//	printf("\r\n%s ", "Firmware revision in ASCII : ");
  printf("\r\n%s%u", "Maximum number of sectors on Read/Write Multiple command : ", ((uint16_t)CardInfo[94] | (uint16_t)CardInfo[95] << 8));
  printf("\r\n%s%u", "Double Word not supported : ", ((uint16_t)CardInfo[96] | (uint16_t)CardInfo[97] << 8));
  printf("\r\n%s%u", "Capabilities : ", ((uint16_t)CardInfo[98] | (uint16_t)CardInfo[99] << 8));
  printf("\r\n%s%u", "PIO data transfer cycle timing mode : ", ((uint16_t)CardInfo[102] | (uint16_t)CardInfo[103] << 8));
  printf("\r\n%s%u", "DMA data transfer cycle timing mode: ", ((uint16_t)CardInfo[104] | (uint16_t)CardInfo[105] << 8));
  printf("\r\n%s%u", "Translation parameters are valid : ", ((uint16_t)CardInfo[106] | (uint16_t)CardInfo[107] << 8));
  printf("\r\n%s%u", "Current numbers of cylinders : ", ((uint16_t)CardInfo[108] | (uint16_t)CardInfo[109] << 8));
  printf("\r\n%s%u", "Current numbers of heads : ", ((uint16_t)CardInfo[110] | (uint16_t)CardInfo[111] << 8));
  printf("\r\n%s%u", "Current sectors per track : ", ((uint16_t)CardInfo[112] | (uint16_t)CardInfo[113] << 8));
  printf("\r\n%s%u", "Current capacity in sectors : ", (uint32_t)((uint16_t)CardInfo[116] | (uint16_t)CardInfo[117] << 8) | ((uint32_t)((uint16_t)CardInfo[114] | (uint16_t)CardInfo[115] << 8) << 16));
  printf("\r\n%s%#x", "Multiple sector setting : ", ((uint16_t)CardInfo[118] | (uint16_t)CardInfo[119] << 8));
//	printf("\r\n%s%u", "Total number of sectors addressable in LBA Mode : ", (uint32_t)CardInfo[60] | ((uint32_t)CardInfo[61] << 16));
  printf("\r\n%s%u", "Total number of sectors addressable in LBA Mode : ", (uint32_t)((uint16_t)CardInfo[120] | (uint16_t)CardInfo[121] << 8) | ((uint32_t)((uint16_t)CardInfo[122] | (uint16_t)CardInfo[123] << 8) << 16));
  printf("\r\n%s%#x", "Security status : ",  ((uint16_t)CardInfo[256] | (uint16_t)CardInfo[257] << 8));
//	printf("\r\n%s ", "Vendor unique bytes : ");
  printf("\r\n%s%#x", "Power requirement description : ",  ((uint16_t)CardInfo[320] | (uint16_t)CardInfo[321] << 8));
  printf("\r\n===============================================================");

#endif

  // signature of CF storage //
  if(((uint16_t)CardInfo[0] | (uint16_t)CardInfo[1] << 8) == 0x848A)
  {
    hpccard->CF.CFCardInfo.Default_Cylinder = ((uint16_t)CardInfo[2] | (uint16_t)CardInfo[3] << 8);
    hpccard->CF.CFCardInfo.Default_Head = ((uint16_t)CardInfo[6] | (uint16_t)CardInfo[7] << 8);
    hpccard->CF.CFCardInfo.Default_Sector = ((uint16_t)CardInfo[12] | (uint16_t)CardInfo[13] << 8);
    hpccard->CF.CFCardInfo.Current_Cylinder = ((uint16_t)CardInfo[108] | (uint16_t)CardInfo[109] << 8);
    hpccard->CF.CFCardInfo.Current_Head = ((uint16_t)CardInfo[110] | (uint16_t)CardInfo[111] << 8);
    hpccard->CF.CFCardInfo.Current_Sector = ((uint16_t)CardInfo[112] | (uint16_t)CardInfo[113] << 8);

    hpccard->CF.CFCardInfo.Total_Sector = (uint32_t)((uint16_t)CardInfo[16] | (uint16_t)CardInfo[17] << 8) | ((uint32_t)((uint16_t)CardInfo[14] | (uint16_t)CardInfo[15] << 8) << 16);
    hpccard->CF.CFCardInfo.Total_LBA_Sector = (uint32_t)((uint16_t)CardInfo[120] | (uint16_t)CardInfo[121] << 8) | ((uint32_t)((uint16_t)CardInfo[122] | (uint16_t)CardInfo[123] << 8) << 16);
    hpccard->CF.CFCardInfo.Max_Mutiple_Sector = ((uint16_t)CardInfo[94] | (uint16_t)CardInfo[95] << 8);
    hpccard->CF.CFCardInfo.Mutiple_Sector_Setting = ((uint16_t)CardInfo[118] | (uint16_t)CardInfo[119] << 8);
  }
  else
  {

    return FALSE;
  }

  return TRUE;
}

/**
  * @brief  Read sector from PCCARD memory
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @param  pBuffer: pointer to destination read buffer
  * @param  Sector_Address: Sector address to read
  * @retval HAL status
  */
BOOL PCCARD_Read_Sector(PCCARD_HandleType *hpccard, uint8_t *pBuffer, uint32_t Sector_Address, uint32_t Sector_Count)
{
  uint16_t Count = 0;
  uint8_t Reg;

  if(Sector_Count == 0)
  {
    Sector_Count = 256;
  }

  while(Count < Sector_Count)
  {

    Translate_CHSAddr(hpccard, Sector_Address, Count, Sector_Count);
    Reg = hpccard->CF.CFAddr.Head | hpccard->CF.CFAddr.Drv | 0xA0;

    *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CYLINDER_HIGH) = (uint8_t)(0xFF & hpccard->CF.CFAddr.Cylinder >> 8);
    *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CYLINDER_LOW)  = (uint8_t)(0xFF & hpccard->CF.CFAddr.Cylinder);
    *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_SECTOR_NUMBER) = hpccard->CF.CFAddr.Sector;
    *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_SECTOR_COUNT)  = (uint8_t)hpccard->CF.CFAddr.Sector_Count;
    *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CARD_HEAD)     = Reg;

    if(CF_SendCommand(hpccard, ATA_READ_SECTOR_CMD))
    {
      while(1)
      {
        if(TaskFileRegIsValid(hpccard))
        {
          if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT | DRQ_BIT))
          {
            pBuffer = Read_Sector(hpccard, pBuffer, PCCARD_SECTOR_SIZE);

            Sector_Address += 1;
            Count += 1;

            if (Count == Sector_Count)
            {
              break;
            }

            if (Count <= (Sector_Count / hpccard->CF.CFCardInfo.Default_Sector) * hpccard->CF.CFCardInfo.Default_Sector)
              if (Count % hpccard->CF.CFAddr.Sector_Count == 0)
              {
                break;
              }
          }
          else if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT | ERR_BIT))
          {
            return FALSE;
          }
        }
      }
    }

    while(1)
    {
      if(TaskFileRegIsValid(hpccard))
      {
        if((PCCARD_Read_STATUS_REG(hpccard) & (RDY_BIT | DSC_BIT | ERR_BIT)) == 0x50)
        {
          break;
        }
        else if((PCCARD_Read_STATUS_REG(hpccard) & (RDY_BIT | DSC_BIT | ERR_BIT)) == 0x51)
        {
//										printf("Error Code=%X\n", PCCARD_Read_STATUS_REG(hpccard));
          return FALSE;
        }
      }
    }
  }

  return TRUE;
}


/**
  * @brief  Write sector to PCCARD memory
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @param  pBuffer: pointer to source write buffer
  * @param  Sector_Address: Sector address to write
  * @retval HAL status
  */
BOOL PCCARD_Write_Sector(PCCARD_HandleType *hpccard, uint8_t *pBuffer, uint32_t Sector_Address, uint32_t Sector_Count)
{
  uint16_t Count = 0;
  uint8_t Reg;

  if(Sector_Count == 0)
  {
    Sector_Count = 256;
  }

  while(Count < Sector_Count)
  {
    // Set the parameters to write a sector //
    Translate_CHSAddr(hpccard, Sector_Address, Count, Sector_Count);
    Reg = hpccard->CF.CFAddr.Head | hpccard->CF.CFAddr.Drv | 0xA0;

    //*(__IO uint16_t *)(hpccard->CF.IOAddr | ATA_CYLINDER_LOW)  = hpccard->CF.CFAddr.Cylinder;
    *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CYLINDER_HIGH) = (uint8_t)(0xFF & hpccard->CF.CFAddr.Cylinder >> 8);
    *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CYLINDER_LOW)  = (uint8_t)(0xFF & hpccard->CF.CFAddr.Cylinder);
    *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_SECTOR_NUMBER) = hpccard->CF.CFAddr.Sector;
    *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_SECTOR_COUNT)  = (uint8_t)hpccard->CF.CFAddr.Sector_Count;
    *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CARD_HEAD)     = Reg;

    if(CF_SendCommand(hpccard, ATA_WRITE_SECTOR_CMD))
    {
      while(1)
      {
        if(TaskFileRegIsValid(hpccard))
        {
          if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT | DRQ_BIT))
          {
            pBuffer = Write_Sector(hpccard, pBuffer, PCCARD_SECTOR_SIZE);

            Sector_Address += 1;
            Count += 1;

            if (Count == Sector_Count)
            {
              break;
            }

            if (Count <= (Sector_Count / hpccard->CF.CFCardInfo.Default_Sector) * hpccard->CF.CFCardInfo.Default_Sector)
              if (Count % hpccard->CF.CFAddr.Sector_Count == 0)
              {
                break;
              }
          }
          else if (PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT | ERR_BIT))
          {
            return FALSE;
          }
        }
      }
    }

    while(1)
    {
      if(TaskFileRegIsValid(hpccard))
      {
        if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT))
        {
          break;
        }
        else if((PCCARD_Read_STATUS_REG(hpccard) & (RDY_BIT | DSC_BIT | ERR_BIT)) == 0x51)
        {
//										printf("Error Code=%X\n", PCCARD_Read_STATUS_REG(hpccard));
          return FALSE;
        }
      }
    }
  }

  return TRUE;
}


/**
  * @brief  Erase sector from PCCARD memory
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @param  Sector_Address: Sector address to erase
  * @retval HAL status
  */
BOOL PCCARD_Erase_Sector(PCCARD_HandleType *hpccard, uint32_t Sector_Address, uint32_t Sector_Count)
{
  uint8_t Reg;
  uint16_t Count = 0;

  while (Count < Sector_Count)
  {

    Translate_CHSAddr(hpccard, Sector_Address, 0, Sector_Count);
    Reg = hpccard->CF.CFAddr.Head | hpccard->CF.CFAddr.Drv | 0xA0;

    *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CYLINDER_HIGH) = (uint8_t)(0xFF & hpccard->CF.CFAddr.Cylinder >> 8);
    *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CYLINDER_LOW)  = (uint8_t)(0xFF & hpccard->CF.CFAddr.Cylinder);
    *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_SECTOR_NUMBER) = hpccard->CF.CFAddr.Sector;
    *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_SECTOR_COUNT)  = hpccard->CF.CFAddr.Sector_Count;
    *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_CARD_HEAD)     = Reg;

    if(CF_SendCommand(hpccard, ATA_ERASE_SECTOR_CMD))
    {
      Count += hpccard->CF.CFAddr.Sector_Count;
      Sector_Address += hpccard->CF.CFAddr.Sector_Count;
    }

    while(1)
    {
      if(TaskFileRegIsValid(hpccard))
      {
        if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT))
        {
          break;
        }
        else if((PCCARD_Read_STATUS_REG(hpccard) & (RDY_BIT | DSC_BIT | ERR_BIT)) == 0x51)
        {
          return FALSE;
        }
      }
    }
  }

  return TRUE;
}
/**
  * @brief  Diagnostic
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @retval TRUE or FALSE.
  */
BOOL PCCARD_Diagnostic(PCCARD_HandleType *hpccard)
{
  uint8_t ERROR_REG;

// Set the parameters //
  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CARD_HEAD)     = hpccard->CF.CFAddr.Drv;

  if(!CF_SendCommand(hpccard, ATA_EXECUTE_DRIVE_DIAG_CMD))
  {
    return FALSE;
  }

  ERROR_REG = PCCARD_Read_ERROR_REG(hpccard);

  if((ERROR_REG == 0x01) || (ERROR_REG == 0x02) || (ERROR_REG == 0x03) || (ERROR_REG == 0x04) || (ERROR_REG == 0x05) || (ERROR_REG == 0x80))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
/**
  * @brief  Check Power Mode
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @retval TRUE or FALSE.
  */
BOOL PCCARD_Check_Power_Mode(PCCARD_HandleType *hpccard)
{
  uint8_t SECTOR_COUNT_REG;

  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_SECTOR_COUNT)  = 0x80;		// set sector_count register 0x80, after check_power_mode
  // the register will be change to 0x00 or 0xFF

  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CARD_HEAD)     = hpccard->CF.CFAddr.Drv;

  if(!CF_SendCommand(hpccard, ATA_CHECK_POWER_MODE_CMD))
  {
    return FALSE;
  }

  SECTOR_COUNT_REG =  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_SECTOR_COUNT);

  if(SECTOR_COUNT_REG == 0xFF || SECTOR_COUNT_REG == 0x00)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
/**
  * @brief  Set idle 
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @param  Sector_Count: the sector count 
  * @retval TRUE or FALSE.
  */
BOOL PCCARD_Idle(PCCARD_HandleType *hpccard, uint32_t Sector_Count)
{
  uint8_t SECTOR_COUNT_REG;

  hpccard->CF.CFAddr.Sector_Count = Sector_Count;

  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_SECTOR_COUNT)  = hpccard->CF.CFAddr.Sector_Count;
  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CARD_HEAD)     = hpccard->CF.CFAddr.Drv;

  if(!CF_SendCommand(hpccard, ATA_IDLE_CMD))
  {
    return FALSE;
  }


// Check Power Mode //
  if(!PCCARD_Check_Power_Mode(hpccard))
  {
    printf("Compact Flash Check Power Mode Failed \n");
    return FALSE;
  }

  SECTOR_COUNT_REG =  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_SECTOR_COUNT);

  if(SECTOR_COUNT_REG == 0xFF)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
/**
  * @brief  Idle immediate 
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @retval TRUE or FALSE.
  */
BOOL PCCARD_Idle_Immediate(PCCARD_HandleType *hpccard)
{
  uint8_t SECTOR_COUNT_REG;

  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_SECTOR_COUNT)  = 0x80;		// set sector_count register 0x80, after check_power_mode
  // the register will be change to 0x00 or 0xFF

  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CARD_HEAD)     = hpccard->CF.CFAddr.Drv;

  if(!CF_SendCommand(hpccard, ATA_IDLE_IMMEDIATE_CMD))
  {
    return FALSE;
  }

// Check Power Mode //
  if(!PCCARD_Check_Power_Mode(hpccard))
  {
    printf("Compact Flash Check Power Mode Failed \n");
    return FALSE;
  }

  SECTOR_COUNT_REG =  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_SECTOR_COUNT);

  if(SECTOR_COUNT_REG == 0xFF)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
/**
  * @brief  Set sleep mode 
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @retval TRUE or FALSE.
  */
BOOL PCCARD_Set_Sleep_Mode(PCCARD_HandleType *hpccard)
{
  uint8_t SECTOR_COUNT_REG;

  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_SECTOR_COUNT)  = 0x80;		// set sector_count register 0x80, after check_power_mode
  // the register will be change to 0x00 or 0xFF

  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CARD_HEAD)     = hpccard->CF.CFAddr.Drv;

  if(!CF_SendCommand(hpccard, ATA_SET_SLEEP_MODE_CMD))
  {
    return FALSE;
  }

// Check Power Mode //
  if(!PCCARD_Check_Power_Mode(hpccard))
  {
    printf("Compact Flash Check Power Mode Failed \n");
    return FALSE;
  }

  SECTOR_COUNT_REG =  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_SECTOR_COUNT);

  if(SECTOR_COUNT_REG == 0x00)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
/**
  * @brief  Set Standby 
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @retval TRUE or FALSE.
  */
BOOL PCCARD_Standby(PCCARD_HandleType *hpccard)
{
  uint8_t SECTOR_COUNT_REG;

  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_SECTOR_COUNT)  = 0x80;		// set sector_count register 0x80, after check_power_mode
  // the register will be change to 0x00 or 0xFF

  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CARD_HEAD)     = hpccard->CF.CFAddr.Drv;

  if(!CF_SendCommand(hpccard, ATA_STANDBY_CMD))
  {
    return FALSE;
  }

// Check Power Mode //
  if(!PCCARD_Check_Power_Mode(hpccard))
  {
    printf("Compact Flash Check Power Mode Failed \n");
    return FALSE;
  }

  SECTOR_COUNT_REG =  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_SECTOR_COUNT);

  if(SECTOR_COUNT_REG == 0x00)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
/**
  * @brief  Standby Immediate 
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @retval TRUE or FALSE.
  */
BOOL PCCARD_Standby_Immediate(PCCARD_HandleType *hpccard)
{
  uint8_t SECTOR_COUNT_REG;

  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_SECTOR_COUNT)  = 0x80;		// set sector_count register 0x80, after check_power_mode
  // the register will be change to 0x00 or 0xFF

  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CARD_HEAD)     = hpccard->CF.CFAddr.Drv;

  if(!CF_SendCommand(hpccard, ATA_STANDBY_IMMEDIATE_CMD))
  {
    return FALSE;
  }

// Check Power Mode //
  if(!PCCARD_Check_Power_Mode(hpccard))
  {
    printf("Compact Flash Check Power Mode Failed \n");
    return FALSE;
  }

  SECTOR_COUNT_REG =  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_SECTOR_COUNT);

  if(SECTOR_COUNT_REG == 0x00)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
/**
  * @brief  Reuqest Sense the pccard 
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @retval TRUE or FALSE.
  */
BOOL PCCARD_Reuqest_Sense(PCCARD_HandleType *hpccard)
{
  uint8_t Reg, ERROR_REG;

  Reg = hpccard->CF.CFAddr.Drv | 0xA0;

  *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_CARD_HEAD)     = Reg;

  if(!CF_SendCommand(hpccard, ATA_REQUEST_SENSE_CMD))
  {
    return FALSE;
  }

  ERROR_REG = PCCARD_Read_ERROR_REG(hpccard);

  if((ERROR_REG == 0x2F) || (ERROR_REG == 0x21))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
/**
  * @brief  Recalibrate the pccard 
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @retval TRUE or FALSE.
  */
BOOL PCCARD_Recalibrate(PCCARD_HandleType *hpccard)
{
  uint8_t Reg, ERROR_REG;

  Reg = hpccard->CF.CFAddr.Drv | 0xA0;

  *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_CARD_HEAD)     = Reg;

  if(!CF_SendCommand(hpccard, ATA_RECALIBRATE_CMD))
  {
    return FALSE;
  }

  ERROR_REG = PCCARD_Read_ERROR_REG(hpccard);

  if((ERROR_REG == 0x00))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
/**
  * @brief  Init drive para 
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @param  Sector_Count: the sector count 
  * @param  Head_Count: the head count.
  * @retval TRUE or FALSE.
  */
BOOL PCCARD_Init_Drive_Para(PCCARD_HandleType *hpccard, uint16_t Sector_Count, uint8_t Head_Count)
{
  uint8_t Reg;

  hpccard->CF.CFAddr.Head = (uint8_t)Head_Count;
  hpccard->CF.CFAddr.Sector_Count = (uint8_t)Sector_Count;

  Reg = hpccard->CF.CFAddr.Head | hpccard->CF.CFAddr.Drv;

  *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_SECTOR_COUNT)  = hpccard->CF.CFAddr.Sector_Count;
  *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_CARD_HEAD)     = Reg;

  if(!CF_SendCommand(hpccard, ATA_INIT_DRIVE_PARA_CMD))
  {
    return FALSE;
  }

  return TRUE;
}
/**
  * @brief  seek pccard 
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @param  Head_Count: the head count.
  * @param  Cylinder_Count: the cylinder count 
  * @retval TRUE or FALSE.
  */
BOOL PCCARD_Seek(PCCARD_HandleType *hpccard, uint8_t Head_Count, uint16_t Cylinder_Count)
{
  uint8_t Reg, ERROR_REG;

  hpccard->CF.CFAddr.Head = (uint8_t)Head_Count;
  hpccard->CF.CFAddr.Cylinder = (uint16_t)Cylinder_Count;

  Reg = hpccard->CF.CFAddr.Head | hpccard->CF.CFAddr.Drv | 0xA0;

  *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_CARD_HEAD)     = Reg;
  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CYLINDER_HIGH) = (uint8_t)(0xFF & hpccard->CF.CFAddr.Cylinder >> 8);
  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CYLINDER_LOW)  = (uint8_t)(0xFF & hpccard->CF.CFAddr.Cylinder);


  if(CF_SendCommand(hpccard, ATA_SEEK_CMD)) // This command should return false
  {
    return FALSE;
  }

  ERROR_REG = PCCARD_Read_ERROR_REG(hpccard);

  if((ERROR_REG != 0x00))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
/**
  * @brief  format track 
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @param  pBuffer: pointer to the data buffer.
  * @param  Sector_Address: the sector addr 
  * @param  Sector_Count: the sector count 
  * @retval TRUE or FALSE.
  */
BOOL PCCARD_Format_Track(PCCARD_HandleType *hpccard, uint8_t *pBuffer, uint32_t Sector_Address, uint32_t Sector_Count)
{
  uint16_t Count = 0;
  uint8_t  Reg;

  if(Sector_Count == 0)
  {
    Sector_Count = 256;
  }

  while(Count < Sector_Count)
  {
    // Set the parameters to write a sector //
    Translate_CHSAddr(hpccard, Sector_Address, Count, Sector_Count);
    Reg = hpccard->CF.CFAddr.Head | hpccard->CF.CFAddr.Drv | 0xA0;

    *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CYLINDER_HIGH) = (uint8_t)(0xFF & hpccard->CF.CFAddr.Cylinder >> 8);
    *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CYLINDER_LOW)  = (uint8_t)(0xFF & hpccard->CF.CFAddr.Cylinder);
    *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_SECTOR_NUMBER) = hpccard->CF.CFAddr.Sector;
    *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_SECTOR_COUNT)  = hpccard->CF.CFAddr.Sector_Count;
    *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_CARD_HEAD)     = Reg;

    if(CF_SendCommand(hpccard, ATA_FORMAT_TRACK_CMD))
    {
      while(1)
      {
        if(TaskFileRegIsValid(hpccard))
        {
          if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT | DRQ_BIT))
          {
//            Write_Sector(hpccard, pBuffer, PCCARD_SECTOR_SIZE);

            Sector_Address += 1;
            Count += 1;

            if (Count == Sector_Count)
            {
              break;
            }

            if (Count <= (Sector_Count / hpccard->CF.CFCardInfo.Default_Sector) * hpccard->CF.CFCardInfo.Default_Sector)
              if (Count % hpccard->CF.CFAddr.Sector_Count == 0)
              {
                break;
              }
          }
          else if (PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT | ERR_BIT))
          {
            return FALSE;
          }
        }
      }
    }

    while(1)
    {
      if(TaskFileRegIsValid(hpccard))
      {
        if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT | DRQ_BIT))
        {
          break;
        }
        else if((PCCARD_Read_STATUS_REG(hpccard) & (RDY_BIT | DSC_BIT | ERR_BIT)) == 0x51)
        {
//										printf("Error Code=%X\n", PCCARD_Read_STATUS_REG(hpccard));
          return FALSE;
        }
      }
    }
  }

  return TRUE;
}
/**
  * @brief  read Verify Sector
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @param  Sector_Address: the sector addr 
  * @param  Sector_Count: the sector count 
  * @retval TRUE or FALSE.
  */
BOOL PCCARD_Read_Verify_Sector(PCCARD_HandleType *hpccard, uint32_t Sector_Address, uint16_t Sector_Count)
{
  uint16_t Count = 0;
  uint8_t Reg;

  if(Sector_Count == 0)
  {
    Sector_Count = 256;
  }

  while(Count < Sector_Count)
  {

    Translate_CHSAddr(hpccard, Sector_Address, Count, Sector_Count);
    Reg = hpccard->CF.CFAddr.Head | hpccard->CF.CFAddr.Drv | 0xA0;

    *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CYLINDER_HIGH) = (uint8_t)(0xFF & hpccard->CF.CFAddr.Cylinder >> 8);
    *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CYLINDER_LOW)  = (uint8_t)(0xFF & hpccard->CF.CFAddr.Cylinder);
    *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_SECTOR_NUMBER) = hpccard->CF.CFAddr.Sector;
    *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_SECTOR_COUNT)  = hpccard->CF.CFAddr.Sector_Count;
    *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_CARD_HEAD)     = Reg;

    if(CF_SendCommand(hpccard, ATA_READ_VERIFY_SECTOR_CMD))
    {
      while(1)
      {
        if(TaskFileRegIsValid(hpccard))
        {
          if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT))
          {
            if(PCCARD_Read_STATUS_REG(hpccard) == DRQ_BIT)  //  DRQ_BIT should not be assert in this command
            {
              return FALSE;
            }
            else
            {
              Sector_Address += 1;
              Count += 1;

              if (Count == Sector_Count)
              {
                break;
              }

              if (Count <= (Sector_Count / hpccard->CF.CFCardInfo.Default_Sector) * hpccard->CF.CFCardInfo.Default_Sector)
                if (Count % hpccard->CF.CFAddr.Sector_Count == 0)
                {
                  break;
                }
            }
          }
          else if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT | ERR_BIT))
          {
            return FALSE;
          }
        }
      }
    }

    while(1)
    {
      if(TaskFileRegIsValid(hpccard))
      {
        if((PCCARD_Read_STATUS_REG(hpccard) & (RDY_BIT | DSC_BIT | ERR_BIT)) == 0x50)
        {
          break;
        }
        else if((PCCARD_Read_STATUS_REG(hpccard) & (RDY_BIT | DSC_BIT | ERR_BIT)) == 0x51)
        {
//										printf("Error Code=%X\n", PCCARD_Read_STATUS_REG(hpccard));
          return FALSE;
        }
      }
    }
  }

  return TRUE;
}
/**
  * @brief  write Verify 
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @param  pBuffer: pointer to the data buffer.
  * @param  Sector_Address: the sector addr 
  * @param  Sector_Count: the sector count 
  * @retval TRUE or FALSE.
  */
BOOL PCCARD_Write_Verify(PCCARD_HandleType *hpccard, uint8_t *pBuffer, uint32_t Sector_Address, uint32_t Sector_Count)
{
  uint16_t Count = 0;
  uint8_t  Reg;

  if(Sector_Count == 0)
  {
    Sector_Count = 256;
  }

  while(Count < Sector_Count)
  {
    // Set the parameters to write a sector //
    Translate_CHSAddr(hpccard, Sector_Address, Count, Sector_Count);
    Reg = hpccard->CF.CFAddr.Head | hpccard->CF.CFAddr.Drv | 0xA0;

    *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CYLINDER_HIGH) = (uint8_t)(0xFF & hpccard->CF.CFAddr.Cylinder >> 8);
    *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CYLINDER_LOW)  = (uint8_t)(0xFF & hpccard->CF.CFAddr.Cylinder);
    *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_SECTOR_NUMBER) = hpccard->CF.CFAddr.Sector;
    *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_SECTOR_COUNT)  = hpccard->CF.CFAddr.Sector_Count;
    *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_CARD_HEAD)     = Reg;

    if(CF_SendCommand(hpccard, ATA_WRITE_VERIFY_CMD))
    {
      while(1)
      {
        if(TaskFileRegIsValid(hpccard))
        {
          if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT | DRQ_BIT))
          {
            Write_Sector(hpccard, pBuffer, PCCARD_SECTOR_SIZE);

            Sector_Address += 1;
            Count += 1;

            if (Count == Sector_Count)
            {
              break;
            }

            if (Count <= (Sector_Count / hpccard->CF.CFCardInfo.Default_Sector) * hpccard->CF.CFCardInfo.Default_Sector)
              if (Count % hpccard->CF.CFAddr.Sector_Count == 0)
              {
                break;
              }
          }
          else if (PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT | ERR_BIT))
          {
            return FALSE;
          }
        }
      }
    }

    while(1)
    {
      if(TaskFileRegIsValid(hpccard))
      {
        if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT))
        {
          break;
        }
        else if((PCCARD_Read_STATUS_REG(hpccard) & (RDY_BIT | DSC_BIT | ERR_BIT)) == 0x51)
        {
//										printf("Error Code=%X\n", PCCARD_Read_STATUS_REG(hpccard));
          return FALSE;
        }
      }
    }
  }

  return TRUE;
}
/**
  * @brief  set multiple mode 
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @param  Sector_Count: the sector count 
  * @retval TRUE or FALSE.
  */
BOOL PCCARD_Set_Multiple_Mode(PCCARD_HandleType *hpccard, uint32_t Sector_Count)
{
  hpccard->CF.CFAddr.Sector_Count = Sector_Count;

  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_SECTOR_COUNT)  = hpccard->CF.CFAddr.Sector_Count;
  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CARD_HEAD)     = hpccard->CF.CFAddr.Drv;

  if(!CF_SendCommand(hpccard, ATA_SET_MULTIPLE_MODE_CMD))
  {
    return FALSE;
  }

  return TRUE;
}
/**
  * @brief  set features 
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @param  Sector_Count: the sector count 
  * @param  Feature: Feature val
  * @retval TRUE or FALSE.
  */
BOOL PCCARD_Set_Features(PCCARD_HandleType *hpccard, uint32_t Sector_Count, uint8_t Feature)
{
  hpccard->CF.CFAddr.Sector_Count = Sector_Count;

  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_SECTOR_COUNT)  = hpccard->CF.CFAddr.Sector_Count;
  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CARD_HEAD)     = hpccard->CF.CFAddr.Drv;
  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_FEATURE_REG)   = Feature;

  if(!CF_SendCommand(hpccard, ATA_SET_FEATURE_CMD))
  {
    return FALSE;
  }

  return TRUE;
}
/**
  * @brief  read buffer 
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @param  pBuffer: pointer to the data buffer.
  * @retval TRUE or FALSE.
  */
BOOL PCCARD_Read_Buffer(PCCARD_HandleType *hpccard, uint8_t *pBuffer)
{
  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CARD_HEAD)     = hpccard->CF.CFAddr.Drv;

  if(CF_SendCommand(hpccard, ATA_READ_BUFFER_CMD))
  {
    if (TaskFileRegIsValid(hpccard))
    {
      if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT | DRQ_BIT))
      {
        Read_Sector(hpccard, pBuffer, PCCARD_SECTOR_SIZE);
      }
      else if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT | ERR_BIT))
      {
        return FALSE;
      }
    }
  }

  while(1)
  {
    if(TaskFileRegIsValid(hpccard))
    {
      if((PCCARD_Read_STATUS_REG(hpccard) & (RDY_BIT | DSC_BIT | ERR_BIT)) == 0x50)
      {
        break;
      }
      else if((PCCARD_Read_STATUS_REG(hpccard) & (RDY_BIT | DSC_BIT | ERR_BIT)) == 0x51)
      {
//								printf("Error Code=%X\n", PCCARD_Read_STATUS_REG(hpccard));
        return FALSE;
      }
    }
  }

  return TRUE;
}
/**
  * @brief  write buffer 
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @param  pBuffer: pointer to the data buffer.
  * @retval TRUE or FALSE.
  */
BOOL PCCARD_Write_Buffer(PCCARD_HandleType *hpccard, uint8_t *pBuffer)
{
  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CARD_HEAD)     = hpccard->CF.CFAddr.Drv;

  if(CF_SendCommand(hpccard, ATA_WRITE_BUFFER_CMD))
  {
    if (TaskFileRegIsValid(hpccard))
    {
      if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT | DRQ_BIT))
      {
        Write_Sector(hpccard, pBuffer, PCCARD_SECTOR_SIZE);
      }
      else if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT | ERR_BIT))
      {
        return FALSE;
      }
    }
  }

  while(1)
  {
    if(TaskFileRegIsValid(hpccard))
    {
      if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT))
      {
        break;
      }
      else if((PCCARD_Read_STATUS_REG(hpccard) & (RDY_BIT | DSC_BIT | ERR_BIT)) == 0x51)
      {
//								printf("Error Code=%X\n", PCCARD_Read_STATUS_REG(hpccard));
        return FALSE;
      }
    }
  }

  return TRUE;
}
/**
  * @brief  Translate sector 
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @param  Sector_Address: the sector addr 
  * @retval TRUE or FALSE.
  */
BOOL PCCARD_Translate_Sector(PCCARD_HandleType *hpccard, uint32_t Sector_Address)
{
  uint32_t Total_Cylinder;
  uint8_t Reg;

  Total_Cylinder = (uint32_t)(hpccard->CF.CFCardInfo.Default_Head * hpccard->CF.CFCardInfo.Default_Sector); // The total sectors in one cylinder

  hpccard->CF.CFAddr.Cylinder  = (uint16_t)(Sector_Address / Total_Cylinder);
  hpccard->CF.CFAddr.Head      = (uint8_t)((Sector_Address % Total_Cylinder) / hpccard->CF.CFCardInfo.Default_Sector);
  hpccard->CF.CFAddr.Sector    = (uint8_t)((Sector_Address % Total_Cylinder) % hpccard->CF.CFCardInfo.Default_Sector) + 1;

// Set the parameters to write a sector //
  Reg = hpccard->CF.CFAddr.Head | hpccard->CF.CFAddr.Drv | 0xA0;

  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CYLINDER_HIGH) = (uint8_t)(0xFF & hpccard->CF.CFAddr.Cylinder >> 8);
  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CYLINDER_LOW)  = (uint8_t)(0xFF & hpccard->CF.CFAddr.Cylinder);
  *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_SECTOR_NUMBER) = hpccard->CF.CFAddr.Sector;
  *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_CARD_HEAD)     = Reg;

  if(!CF_SendCommand(hpccard, ATA_TRANSLATE_SECTOR_CMD))
  {
    return FALSE;
  }

  return TRUE;
}
/**
  * @brief  write sector WO_ERASE
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @param  pBuffer: pointer to the data buffer.
  * @param  Sector_Address: the sector addr 
  * @param  Sector_Count: the sector count 
  * @retval TRUE or FALSE.
  */
BOOL PCCARD_Write_Sector_WO_ERASE(PCCARD_HandleType *hpccard, uint8_t *pBuffer, uint32_t Sector_Address, uint32_t Sector_Count)
{
  uint16_t Count = 0;
  uint8_t  Reg;

  if(Sector_Count == 0)
  {
    Sector_Count = 256;
  }

  while(Count < Sector_Count)
  {
    // Set the parameters to write a sector //
    Translate_CHSAddr(hpccard, Sector_Address, Count, Sector_Count);
    Reg = hpccard->CF.CFAddr.Head | hpccard->CF.CFAddr.Drv | 0xA0;

    *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CYLINDER_HIGH) = (uint8_t)(0xFF & hpccard->CF.CFAddr.Cylinder >> 8);
    *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CYLINDER_LOW)  = (uint8_t)(0xFF & hpccard->CF.CFAddr.Cylinder);
    *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_SECTOR_NUMBER) = hpccard->CF.CFAddr.Sector;
    *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_SECTOR_COUNT)  = hpccard->CF.CFAddr.Sector_Count;
    *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_CARD_HEAD)     = Reg;

    if(CF_SendCommand(hpccard, ATA_WRITE_SECTOR_WO_ERASE_CMD))
    {
      while(1)
      {
        if(TaskFileRegIsValid(hpccard))
        {
          if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT | DRQ_BIT))
          {
            Write_Sector(hpccard, pBuffer, PCCARD_SECTOR_SIZE);

            Sector_Address += 1;
            Count += 1;

            if (Count == Sector_Count)
            {
              break;
            }

            if (Count <= (Sector_Count / hpccard->CF.CFCardInfo.Default_Sector) * hpccard->CF.CFCardInfo.Default_Sector)
              if (Count % hpccard->CF.CFAddr.Sector_Count == 0)
              {
                break;
              }
          }
          else if (PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT | ERR_BIT))
          {
            return FALSE;
          }
        }
      }
    }

    while(1)
    {
      if(TaskFileRegIsValid(hpccard))
      {
        if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT))
        {
          break;
        }
        else if((PCCARD_Read_STATUS_REG(hpccard) & (RDY_BIT | DSC_BIT | ERR_BIT)) == 0x51)
        {
//										printf("Error Code=%X\n", PCCARD_Read_STATUS_REG(hpccard));
          return FALSE;
        }
      }
    }
  }

  return TRUE;
}
/**
  * @brief  write Multiple
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @param  pBuffer: pointer to the data buffer.
  * @param  Sector_Address: the sector addr 
  * @param  Sector_Count: the sector count 
  * @retval TRUE or FALSE.
  */
BOOL PCCARD_Write_Multiple(PCCARD_HandleType *hpccard, uint8_t *pBuffer, uint32_t Sector_Address, uint32_t Sector_Count)
{
  uint16_t Count = 0;
  uint8_t  Reg;

  if(Sector_Count == 0)
  {
    Sector_Count = 256;
  }

  while(Count < Sector_Count)
  {
    // Set the parameters to write a sector //
    Translate_CHSAddr(hpccard, Sector_Address, Count, Sector_Count);
    Reg = hpccard->CF.CFAddr.Head | hpccard->CF.CFAddr.Drv | 0xA0;

    *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CYLINDER_HIGH) = (uint8_t)(0xFF & hpccard->CF.CFAddr.Cylinder >> 8);
    *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CYLINDER_LOW)  = (uint8_t)(0xFF & hpccard->CF.CFAddr.Cylinder);
    *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_SECTOR_NUMBER) = hpccard->CF.CFAddr.Sector;
    *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_SECTOR_COUNT)  = hpccard->CF.CFAddr.Sector_Count;
    *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_CARD_HEAD)     = Reg;

    if(CF_SendCommand(hpccard, ATA_WRITE_MULTIPLE_CMD))
    {
      while(1)
      {
        if(TaskFileRegIsValid(hpccard))
        {
          if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT | DRQ_BIT))
          {
            Write_Sector(hpccard, pBuffer, PCCARD_SECTOR_SIZE);

            Sector_Address += 1;
            Count += 1;

            if (Count == Sector_Count)
            {
              break;
            }

            if (Count <= (Sector_Count / hpccard->CF.CFCardInfo.Default_Sector) * hpccard->CF.CFCardInfo.Default_Sector)
              if (Count % hpccard->CF.CFAddr.Sector_Count == 0)
              {
                break;
              }
          }
          else if (PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT | ERR_BIT))
          {
            return FALSE;
          }
        }
      }
    }

    while(1)
    {
      if(TaskFileRegIsValid(hpccard))
      {
        if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT))
        {
          break;
        }
        else if((PCCARD_Read_STATUS_REG(hpccard) & (RDY_BIT | DSC_BIT | ERR_BIT)) == 0x51)
        {
//										printf("Error Code=%X\n", PCCARD_Read_STATUS_REG(hpccard));
          return FALSE;
        }
      }
    }
  }

  return TRUE;
}
/**
  * @brief  read Multiple 
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @param  pBuffer: pointer to the data buffer.
  * @param  Sector_Address: the sector addr 
  * @param  Sector_Count: the sector count 
  * @retval TRUE or FALSE.
  */
BOOL PCCARD_Read_Multiple(PCCARD_HandleType *hpccard, uint8_t *pBuffer, uint32_t Sector_Address, uint32_t Sector_Count)
{
  uint16_t Count = 0;
  uint8_t Reg;

  if(Sector_Count == 0)
  {
    Sector_Count = 256;
  }

  while(Count < Sector_Count)
  {

    Translate_CHSAddr(hpccard, Sector_Address, Count, Sector_Count);
    Reg = hpccard->CF.CFAddr.Head | hpccard->CF.CFAddr.Drv | 0xA0;

    *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CYLINDER_HIGH) = (uint8_t)(0xFF & hpccard->CF.CFAddr.Cylinder >> 8);
    *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CYLINDER_LOW)  = (uint8_t)(0xFF & hpccard->CF.CFAddr.Cylinder);
    *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_SECTOR_NUMBER) = hpccard->CF.CFAddr.Sector;
    *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_SECTOR_COUNT)  = hpccard->CF.CFAddr.Sector_Count;
    *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_CARD_HEAD)     = Reg;

    if(CF_SendCommand(hpccard, ATA_READ_MULTIPLE_CMD))
    {
      while(1)
      {
        if(TaskFileRegIsValid(hpccard))
        {
          if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT | DRQ_BIT))
          {
            Read_Sector(hpccard, pBuffer, PCCARD_SECTOR_SIZE);

            Sector_Address += 1;
            Count += 1;

            if (Count == Sector_Count)
            {
              break;
            }

            if (Count <= (Sector_Count / hpccard->CF.CFCardInfo.Default_Sector) * hpccard->CF.CFCardInfo.Default_Sector)
              if (Count % hpccard->CF.CFAddr.Sector_Count == 0)
              {
                break;
              }
          }
          else if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT | ERR_BIT))
          {
            return FALSE;
          }
        }
      }
    }

    while(1)
    {
      if(TaskFileRegIsValid(hpccard))
      {
        if((PCCARD_Read_STATUS_REG(hpccard) & (RDY_BIT | DSC_BIT | ERR_BIT)) == 0x50)
        {
          break;
        }
        else if((PCCARD_Read_STATUS_REG(hpccard) & (RDY_BIT | DSC_BIT | ERR_BIT)) == 0x51)
        {
//										printf("Error Code=%X\n", PCCARD_Read_STATUS_REG(hpccard));
          return FALSE;
        }
      }
    }
  }

  return TRUE;
}
/**
  * @brief  write Multiple WO_ERASE
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @param  pBuffer: pointer to the data buffer.
  * @param  Sector_Address: the sector addr 
  * @param  Sector_Count: the sector count 
  * @retval TRUE or FALSE.
  */
BOOL PCCARD_Write_Multiple_WO_ERASE(PCCARD_HandleType *hpccard, uint8_t *pBuffer, uint32_t Sector_Address, uint32_t Sector_Count)
{
  uint16_t Count = 0;
  uint8_t Reg;

  if(Sector_Count == 0)
  {
    Sector_Count = 256;
  }

  while(Count < Sector_Count)
  {
    // Set the parameters to write a sector //
    Translate_CHSAddr(hpccard, Sector_Address, Count, Sector_Count);
    Reg = hpccard->CF.CFAddr.Head | hpccard->CF.CFAddr.Drv | 0xA0;

    *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CYLINDER_HIGH) = (uint8_t)(0xFF & hpccard->CF.CFAddr.Cylinder >> 8);
    *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CYLINDER_LOW)  = (uint8_t)(0xFF & hpccard->CF.CFAddr.Cylinder);
    *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_SECTOR_NUMBER) = hpccard->CF.CFAddr.Sector;
    *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_SECTOR_COUNT)  = hpccard->CF.CFAddr.Sector_Count;
    *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_CARD_HEAD)     = Reg;

    if(CF_SendCommand(hpccard, ATA_WRITE_MULTIPLE_WO_ERASE_CMD))
    {
      while(1)
      {
        if(TaskFileRegIsValid(hpccard))
        {
          if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT | DRQ_BIT))
          {
            Write_Sector(hpccard, pBuffer, PCCARD_SECTOR_SIZE);

            Sector_Address += 1;
            Count += 1;

            if (Count == Sector_Count)
            {
              break;
            }

            if (Count <= (Sector_Count / hpccard->CF.CFCardInfo.Default_Sector) * hpccard->CF.CFCardInfo.Default_Sector)
              if (Count % hpccard->CF.CFAddr.Sector_Count == 0)
              {
                break;
              }
          }
          else if (PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT | ERR_BIT))
          {
            return FALSE;
          }
        }
      }
    }

    while(1)
    {
      if(TaskFileRegIsValid(hpccard))
      {
        if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT))
        {
          break;
        }
        else if((PCCARD_Read_STATUS_REG(hpccard) & (RDY_BIT | DSC_BIT | ERR_BIT)) == 0x51)
        {
//										printf("Error Code=%X\n", PCCARD_Read_STATUS_REG(hpccard));
          return FALSE;
        }
      }
    }
  }

  return TRUE;
}
/**
  * @brief  read long sector
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @param  Sector_Address: the sector addr 
  * @param  pBuffer: pointer to the data buffer.
  * @retval TRUE or FALSE.
  */
BOOL PCCARD_Read_Long_Sector(PCCARD_HandleType *hpccard, uint32_t Sector_Address, uint8_t *pBuffer)
{
  uint32_t index = 0, Total_Cylinder;
  uint8_t Reg;
  BOOL command_vaild = TRUE;

  Total_Cylinder = (uint32_t)(hpccard->CF.CFCardInfo.Default_Head * hpccard->CF.CFCardInfo.Default_Sector); // The total sectors in one cylinder

  hpccard->CF.CFAddr.Cylinder  = (uint16_t)(Sector_Address / Total_Cylinder);
  hpccard->CF.CFAddr.Head      = (uint8_t)((Sector_Address % Total_Cylinder) / hpccard->CF.CFCardInfo.Default_Sector);
  hpccard->CF.CFAddr.Sector    = (uint8_t)((Sector_Address % Total_Cylinder) % hpccard->CF.CFCardInfo.Default_Sector) + 1;
  hpccard->CF.CFAddr.Sector_Count = 1;	//  some CF storage card implement the read long sector command as read sector commend
  //  set sector count to 1, after read 512 byte data, if the DRQ_BIT still high, means the
  //  read long sector command work well, because it shoult return 516 byte data (ECC).


  Reg = hpccard->CF.CFAddr.Head | hpccard->CF.CFAddr.Drv | 0xA0;

  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CYLINDER_HIGH) = (uint8_t)(0xFF & hpccard->CF.CFAddr.Cylinder >> 8);
  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CYLINDER_LOW)  = (uint8_t)(0xFF & hpccard->CF.CFAddr.Cylinder);
  *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_SECTOR_NUMBER) = hpccard->CF.CFAddr.Sector;
  *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_SECTOR_COUNT)  = hpccard->CF.CFAddr.Sector_Count;
  *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_CARD_HEAD)     = Reg;

  if(CF_SendCommand(hpccard, ATA_READ_LONG_SECTOR_CMD))
  {
    if(TaskFileRegIsValid(hpccard))
    {
      if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT | DRQ_BIT))
      {
        Read_Sector(hpccard, pBuffer, PCCARD_SECTOR_SIZE);

        if(PCCARD_Read_STATUS_REG(hpccard) & DRQ_BIT)
        {
          for(index = 0; index < 4; index++)
          {
            *(uint8_t *)pBuffer++ = *(uint8_t *)(hpccard->CF.IOAddr);
          }
        }
        else
        {
          command_vaild = FALSE;
        }

        if(PCCARD_Read_STATUS_REG(hpccard) & DRQ_BIT)
        {
          command_vaild = FALSE;
        }

        else if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT | ERR_BIT))
        {
          return FALSE;

        }
      }
    }
  }

  while(1)
  {
    if(TaskFileRegIsValid(hpccard))
    {
      if((PCCARD_Read_STATUS_REG(hpccard) & (RDY_BIT | DSC_BIT | ERR_BIT)) == 0x50)
      {
        break;
      }
      else if((PCCARD_Read_STATUS_REG(hpccard) & (RDY_BIT | DSC_BIT | ERR_BIT)) == 0x51)
      {
//								printf("Error Code=%X\n", PCCARD_Read_STATUS_REG(hpccard));
        return FALSE;
      }
    }
  }

  if(command_vaild == TRUE)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
/**
  * @brief  write long sector
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @param  Sector_Address: the sector addr 
  * @param  pBuffer: pointer to the data buffer.
  * @retval TRUE or FALSE.
  */
BOOL PCCARD_Write_Long_Sector(PCCARD_HandleType *hpccard, uint32_t Sector_Address, uint8_t *pBuffer)
{
  uint32_t index = 0, Total_Cylinder;
  uint8_t Reg;
  BOOL command_vaild = TRUE;

  Total_Cylinder = (uint32_t)(hpccard->CF.CFCardInfo.Default_Head * hpccard->CF.CFCardInfo.Default_Sector); // The total sectors in one cylinder

  hpccard->CF.CFAddr.Cylinder  = (uint16_t)(Sector_Address / Total_Cylinder);
  hpccard->CF.CFAddr.Head      = (uint8_t)((Sector_Address % Total_Cylinder) / hpccard->CF.CFCardInfo.Default_Sector);
  hpccard->CF.CFAddr.Sector    = (uint8_t)((Sector_Address % Total_Cylinder) % hpccard->CF.CFCardInfo.Default_Sector) + 1;
  hpccard->CF.CFAddr.Sector_Count = 1;	//  some CF storage card implement the read long sector command as read sector commend
  //  set sector count to 1, after read 512 byte data, if the DRQ_BIT still high, means the
  //  read long sector command is work well, because it shoult return 516 byte data (ECC).


  Reg = hpccard->CF.CFAddr.Head | hpccard->CF.CFAddr.Drv | 0xA0;

  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CYLINDER_HIGH) = (uint8_t)(0xFF & hpccard->CF.CFAddr.Cylinder >> 8);
  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_CYLINDER_LOW)  = (uint8_t)(0xFF & hpccard->CF.CFAddr.Cylinder);
  *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_SECTOR_NUMBER) = hpccard->CF.CFAddr.Sector;
  *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_SECTOR_COUNT)  = hpccard->CF.CFAddr.Sector_Count;
  *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_CARD_HEAD)     = Reg;

  if(CF_SendCommand(hpccard, ATA_WRITE_LONG_SECTOR_CMD))
  {
    if(TaskFileRegIsValid(hpccard))
    {
      if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT | DRQ_BIT))
      {
        Write_Sector(hpccard, pBuffer, PCCARD_SECTOR_SIZE);

        if(PCCARD_Read_STATUS_REG(hpccard) & DRQ_BIT)
        {
          for(index = 0; index < 4; index++) ///////////8 bit problem
          {
            *(uint8_t *)(hpccard->CF.IOAddr) = *(uint8_t *)pBuffer++;
          }
        }
        else
        {
          command_vaild = FALSE;
        }

        if(PCCARD_Read_STATUS_REG(hpccard) & DRQ_BIT)
        {
          command_vaild = FALSE;
        }

        else if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT | ERR_BIT))
        {
          return FALSE;
        }
      }
    }
  }

  while(1)
  {
    if(TaskFileRegIsValid(hpccard))
    {
      if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT))
      {
        break;
      }
      else if((PCCARD_Read_STATUS_REG(hpccard) & (RDY_BIT | DSC_BIT | ERR_BIT)) == 0x51)
      {
//								printf("Error Code=%X\n", PCCARD_Read_STATUS_REG(hpccard));
        return FALSE;
      }
    }
  }

  if(command_vaild == TRUE)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
/**
  * @brief  read sector
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @param  pBuffer: pointer to the data buffer.
  * @param  Transfer_Size: the data length.
  * @retval pointer to the data buffer after read.
  */
uint8_t *Read_Sector(PCCARD_HandleType *hpccard, uint8_t *pBuffer, uint32_t Transfer_Size)
{
  uint32_t index = 0;

  XMC_Enable_16bit_Feature(hpccard->Instance, &(hpccard->Init), hpccard->CF.Enable_8_bit_mode);

  if(hpccard->CF.DMAEnable == TRUE)
  {
    if(hpccard->CF.Enable_8_bit_mode)
    {
      MYDMA_Config(DMA1_Channel4, (uint32_t)hpccard->CF.IOAddr, (uint32_t)pBuffer, Transfer_Size, Read_Transfer, Enable_8_bit_Transfer);
    }
    else
    {
      MYDMA_Config(DMA1_Channel4, (uint32_t)hpccard->CF.IOAddr, (uint32_t)pBuffer, (Transfer_Size / 2), Read_Transfer, Enable_16_bit_Transfer);
    }

    while(1)
    {
      if(DMA_GetFlagStatus(DMA1_FLAG_TC4) != RESET)
      {
        DMA_ClearFlag(DMA1_FLAG_TC4);
        break;
      }
    }

    pBuffer += 512;
  }
  else
  {
    if(hpccard->CF.Enable_8_bit_mode)
    {
      for(; index < (Transfer_Size) ; index++)
      {
        *(uint8_t *)pBuffer++ = *(uint8_t *)(hpccard->CF.IOAddr);
      }
    }
    else
    {
      for(; index < (Transfer_Size / 2); index++)
      {
        *(uint16_t *)pBuffer = *(uint16_t *)(hpccard->CF.IOAddr);
        pBuffer += 2;  // offset to next 16 bit
      }
    }
  }

  XMC_Enable_16bit_Feature(hpccard->Instance, &(hpccard->Init), 1);
  return pBuffer;
}
/**
  * @brief  write sector
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @param  pBuffer: pointer to the data buffer.
  * @param  Transfer_Size: the data length.
  * @retval pointer to the data buffer after write.
  */
uint8_t *Write_Sector(PCCARD_HandleType *hpccard, uint8_t *pBuffer, uint32_t Transfer_Size)
{
  uint32_t index = 0;

  XMC_Enable_16bit_Feature(hpccard->Instance, &(hpccard->Init), hpccard->CF.Enable_8_bit_mode);

  if(hpccard->CF.DMAEnable == TRUE)
  {
    if(hpccard->CF.Enable_8_bit_mode)
    {
      MYDMA_Config(DMA1_Channel4, (uint32_t)pBuffer, (uint32_t)hpccard->CF.IOAddr, Transfer_Size, Write_Transfer, Enable_8_bit_Transfer);
    }
    else
    {
      MYDMA_Config(DMA1_Channel4, (uint32_t)pBuffer, (uint32_t)hpccard->CF.IOAddr, (Transfer_Size / 2), Write_Transfer, Enable_16_bit_Transfer);
    }

    while(1)
    {
      if(DMA_GetFlagStatus(DMA1_FLAG_TC4) != RESET)	
      {
        DMA_ClearFlag(DMA1_FLAG_TC4);
        break;
      }
    }

    pBuffer += 512;
  }
  else
  {
    if(hpccard->CF.Enable_8_bit_mode)
    {
      for(; index < (Transfer_Size); index++)
      {
        *(uint8_t *)(hpccard->CF.IOAddr) = *(uint8_t *)pBuffer++;
      }
    }
    else
    {
      for(; index < (Transfer_Size / 2); index++)
      {
        *(uint16_t *)(hpccard->CF.IOAddr) = *(uint16_t *)pBuffer;
        pBuffer += 2;  // offset to next 16 bit
      }
    }
  }

  XMC_Enable_16bit_Feature(hpccard->Instance, &(hpccard->Init), 1);
  return pBuffer;
}

/**
  * @brief  Reset the PCCARD memory
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @retval HAL status
  */
BOOL PCCARD_Reset(PCCARD_HandleType *hpccard)
{
  /* Provide an SW reset and Read and verify the:
   - CF Configuration Option Register at address 0x98000200 --> 0x80
   - Card Configuration and Status Register at address 0x98000202 --> 0x00
   - Pin Replacement Register  at address 0x98000204 --> 0x0C
   - Socket and Copy Register at address 0x98000206 --> 0x00
  */

  uint8_t config_option_reg;

  // Set up the Reset bit in config option register
  *(__IO uint8_t *)(PCCARD_ATTRIBUTE_SPACE_ADDRESS | CONFIG_OPTION_REG) = 0x80;
  Delay_ms(100);
  // Clear the Reset bit in config option register
  *(__IO uint8_t *)(PCCARD_ATTRIBUTE_SPACE_ADDRESS | CONFIG_OPTION_REG) = 0x00;
  Delay_ms(100);
  // Set the PC card I/O configurations
  *(__IO uint8_t *)(PCCARD_ATTRIBUTE_SPACE_ADDRESS | CONFIG_OPTION_REG) = hpccard->CF.Protocol;
  Delay_ms(100);

  config_option_reg = *(__IO uint8_t *)(PCCARD_ATTRIBUTE_SPACE_ADDRESS | CONFIG_OPTION_REG);

  if(config_option_reg == hpccard->CF.Protocol)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/**
	* Translate_CHSAddr:
	*	translate DISK sector number to C.H.S address.
	*	input	: CFCard
	*			  Start (Start sector number)
	*			  Sector_Count (sector count which read from CF card)
	*			  Sector_Limit (maximum sector number)
	*	output	: None
	*/
void Translate_CHSAddr(PCCARD_HandleType *hpccard, uint32_t Start, uint16_t Sector_Count, uint16_t Sector_Limit)
{
  uint32_t Total_Cylinder;
  uint16_t Prev_Count;

  Prev_Count = (Sector_Limit / hpccard->CF.CFCardInfo.Default_Sector) * hpccard->CF.CFCardInfo.Default_Sector;

  Total_Cylinder = (uint32_t)(hpccard->CF.CFCardInfo.Default_Head * hpccard->CF.CFCardInfo.Default_Sector); // The total sectors in one cylinder

  hpccard->CF.CFAddr.Cylinder  = (uint16_t)(Start / Total_Cylinder);
  hpccard->CF.CFAddr.Head      = (uint8_t)((Start % Total_Cylinder) / hpccard->CF.CFCardInfo.Default_Sector);
  hpccard->CF.CFAddr.Sector    = (uint8_t)((Start % Total_Cylinder) % hpccard->CF.CFCardInfo.Default_Sector) + 1;

  if ((Prev_Count <= Sector_Count) && (Sector_Count <= Sector_Limit))
  {
    hpccard->CF.CFAddr.Sector_Count = (uint8_t)(Sector_Limit % hpccard->CF.CFCardInfo.Default_Sector);
  }
  else
  {
    hpccard->CF.CFAddr.Sector_Count = (uint8_t)hpccard->CF.CFCardInfo.Default_Sector;
  }
}

/**
  * @brief  Read CIS Information.
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @retval TRUE or FALSE.
  */
BOOL ReadCFCardCISInformation(PCCARD_HandleType *hpccard)
{
  uint8_t CIS_1, CIS_2, CIS_3;

  CIS_1 = *(uint8_t *)(PCCARD_ATTRIBUTE_SPACE_ADDRESS | 0x0);
  CIS_2 = *(uint8_t *)(PCCARD_ATTRIBUTE_SPACE_ADDRESS | 0x2);
  CIS_3 = *(uint8_t *)(PCCARD_ATTRIBUTE_SPACE_ADDRESS | 0x4);

  if(CIS_1 == 0x1 && CIS_2 == 0x4 && CIS_3 == 0xdf)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/**
  * @brief  Send command.
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @param  Cmd: the command value
  * @retval TRUE or FALSE.
  */
BOOL CF_SendCommand(PCCARD_HandleType *hpccard, uint8_t Cmd)
{
  uint8_t Reg;

  do
  {
    *(__IO uint8_t  *)(hpccard->CF.IOAddr | ATA_STATUS_CMD) = Cmd;

    while(!TaskFileRegIsValid(hpccard)) ;

    Reg = PCCARD_Read_STATUS_REG(hpccard);

    if ((Reg & ERR_BIT) != 0)
    {
      return FALSE;
    }
  }
  while((Reg & RDY_BIT) == 0);

  return TRUE;
}
/**
  * @brief  Check the PCCARD TaskFileReg valid.
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @retval TRUE or FALSE.
  */
BOOL TaskFileRegIsValid(PCCARD_HandleType *hpccard)
{
  uint8_t status = 0;

  status = PCCARD_Read_STATUS_REG(hpccard);

  if(status & BUSY_BIT)
  {
    return FALSE;
  }

  return TRUE;
}
/**
  * @brief  Check the PCCARD info valid.
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @param  Sector_Count: sector count
  * @param  Head_Count:head count
  * @retval TRUE or FALSE.
  */
BOOL Vaild_Init_Drive_Para(PCCARD_HandleType *hpccard, uint16_t Sector_Count, uint8_t Head_Count)
{
  uint8_t CardInfo[PCCARD_SECTOR_SIZE * sizeof(uint8_t)] = {0};

  memset(CardInfo, 0, PCCARD_SECTOR_SIZE * sizeof(uint8_t));

  if(PCCARD_Identify(hpccard))
  {
    while(1)
    {
      if(TaskFileRegIsValid(hpccard))
      {
        if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT | DRQ_BIT))
        {
          Read_Sector(hpccard, CardInfo, PCCARD_SECTOR_SIZE);
          break;
        }
        else if(PCCARD_Read_STATUS_REG(hpccard) == (RDY_BIT | DSC_BIT | ERR_BIT))
        {
          printf("\r\nCommand Pass, but ERR bit is high");
          printf("\r\nCF Status Reg=%#x", PCCARD_Read_STATUS_REG(hpccard));
          printf("\r\nCF Error  Reg=%#x", PCCARD_Read_ERROR_REG(hpccard));
          return FALSE;
        }
      }
    }
  }

  while(1)
  {
    if(TaskFileRegIsValid(hpccard))
    {
      if((PCCARD_Read_STATUS_REG(hpccard) & (RDY_BIT | DSC_BIT | ERR_BIT)) == 0x50)
      {
        break;
      }
      else if((PCCARD_Read_STATUS_REG(hpccard) & (RDY_BIT | DSC_BIT | ERR_BIT)) == 0x51)
      {
        printf("\r\nSend Command Failed");
        printf("\r\nCF Status Reg=%#x", PCCARD_Read_STATUS_REG(hpccard));
        printf("\r\nCF Error  Reg=%#x", PCCARD_Read_ERROR_REG(hpccard));
        return FALSE;
      }
    }
  }

  if (((uint16_t)CardInfo[0] | (uint16_t)CardInfo[1] << 8) == 0x848A && ((uint16_t)CardInfo[110] | \
      (uint16_t)CardInfo[111] << 8) ==  Head_Count && ((uint16_t)CardInfo[112] | (uint16_t)CardInfo[113] << 8) ==  Sector_Count )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

}
/**
  * @brief  printf the verification result.
  * @param  result: pointer to a verification_result structure that contains
  *                various kinds of verification.
  * @retval None.
  */
void show_verification_result(verification_result_struct *result)
{
  printf("\r\n");
  printf("\r\n----------------------------------------------------------------------");
  printf("\r\n%s    %-6s   ", "ATTRIBUTE_RW_PASS      ", ((result->ATTRIBUTE_RW_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "READ_ATTRIBUTE_CIS     ", ((result->CIS_READ_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "IDENTIFY_DRIVE         ", ((result->ATA_IDENTIFY_DRIVE_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "EXECUTE_DRIVE_DIAG     ", ((result->ATA_EXECUTE_DRIVE_DIAG_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "WRITE_SECTOR           ", ((result->ATA_WRITE_SECTOR_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "READ_SECTOR            ", ((result->ATA_READ_SECTOR_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "ACESS_25_SECTORS       ", ((result->ACESS_25_SECTORS_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n----------------------------------------------------------------------");
#if 0
  printf("\r\n%s    %-6s   ", "CHECK_POWER_MODE       ", ((result->ATA_CHECK_POWER_MODE_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "ERASE_SECTOR           ", ((result->ATA_ERASE_SECTOR_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "FORMAT_TRACK           ", ((result->ATA_FORMAT_TRACK_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "IDLE                   ", ((result->ATA_IDLE_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "IDLE_IMMEDIATE         ", ((result->ATA_IDLE_IMMEDIATE_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "INIT_DRIVE_PARA        ", ((result->ATA_INIT_DRIVE_PARA_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "READ_BUFFER            ", ((result->ATA_READ_BUFFER_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "READ_LONG_SECTOR       ", ((result->ATA_READ_LONG_SECTOR_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "READ_MULTIPLE          ", ((result->ATA_READ_MULTIPLE_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "READ_VERIFY_SECTOR     ", ((result->ATA_READ_VERIFY_SECTOR_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "RECALIBRATE            ", ((result->ATA_RECALIBRATE_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "REQUEST_SENSE          ", ((result->ATA_REQUEST_SENSE_PASS) ? " PASS" : "FAILED *****"));
#if 0  //  security commands
  printf("\r\n%s    %-6s   ", "SECURITY_DIS_PASSWORD  ", ((result->ATA_SECURITY_DISABLE_PASSWORD_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "SECURITY_EREASE_PREPARE", ((result->ATA_SECURITY_EREASE_PREPARE_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "SECURITY_ERASE_UNIT    ", ((result->ATA_SECURITY_ERASE_UNIT_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "SECURITY_FREEZE_LOCKE  ", ((result->ATA_SECURITY_FREEZE_LOCK_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "SECURITY_SET_PASSWORD  ", ((result->ATA_SECURITY_SET_PASSWORD_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "SECURITY_UNLOCK        ", ((result->ATA_SECURITY_UNLOCK_PASS) ? " PASS" : "FAILED *****"));
#endif
  printf("\r\n%s    %-6s   ", "SEEK                   ", ((result->ATA_SEEK_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "SET_FEATURE            ", ((result->ATA_SET_FEATURE_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "SET_MULTIPLE_MODE      ", ((result->ATA_SET_MULTIPLE_MODE_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "SET_SLEEP_MODE         ", ((result->ATA_SET_SLEEP_MODE_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "STANDBY                ", ((result->ATA_STANDBY_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "STANDBY_IMMEDIATE      ", ((result->ATA_STANDBY_IMMEDIATE_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "TRANSLATE_SECTOR       ", ((result->ATA_TRANSLATE_SECTOR_PASS) ? " PASS" : "FAILED *****"));
#if 0  //  security command
  printf("\r\n%s    %-6s   ", "WEAR_LEVEL             ", ((result->ATA_WEAR_LEVEL_PASS) ? " PASS" : "FAILED *****"));
#endif
  printf("\r\n%s    %-6s   ", "WRITE_BUFFER           ", ((result->ATA_WRITE_BUFFER_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "WRITE_LONG_SECTOR      ", ((result->ATA_WRITE_LONG_SECTOR_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "WRITE_MULTIPLE         ", ((result->ATA_WRITE_MULTIPLE_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "WRITE_MULTIPLE_WO_ERASE", ((result->ATA_WRITE_MULTIPLE_WO_ERASE_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "WRITE_SECTOR_WO_ERASE  ", ((result->ATA_WRITE_SECTOR_WO_ERASE_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n%s    %-6s   ", "WRITE_VERIFY           ", ((result->ATA_WRITE_VERIFY_PASS) ? " PASS" : "FAILED *****"));
  printf("\r\n----------------------------------------------------------------------");
#endif
  printf("\r\n\r\n");
}


/**
  * @brief  This function handles PCCARD device interrupt request.
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @retval HAL status
  */
void PCCARD_IRQHandler(PCCARD_HandleType *hpccard)
{
  /* Check PCCARD interrupt Rising edge flag */
  if(__XMC_PCCARD_GET_FLAG(hpccard->Instance, XMC_FLAG_RisingEdge))
  {
    /* PCCARD interrupt callback*/
    PCCARD_ITCallback(hpccard);

    /* Clear PCCARD interrupt Rising edge pending bit */
    __XMC_PCCARD_CLEAR_FLAG(hpccard->Instance, XMC_FLAG_RisingEdge);
  }

  /* Check PCCARD interrupt Level flag */
  if(__XMC_PCCARD_GET_FLAG(hpccard->Instance, XMC_FLAG_Level))
  {
    /* PCCARD interrupt callback*/
    PCCARD_ITCallback(hpccard);

    /* Clear PCCARD interrupt Level pending bit */
    __XMC_PCCARD_CLEAR_FLAG(hpccard->Instance, XMC_FLAG_Level);
  }

  /* Check PCCARD interrupt Falling edge flag */
  if(__XMC_PCCARD_GET_FLAG(hpccard->Instance, XMC_FLAG_FallingEdge))
  {
    /* PCCARD interrupt callback*/
    PCCARD_ITCallback(hpccard);

    /* Clear PCCARD interrupt Falling edge pending bit */
    __XMC_PCCARD_CLEAR_FLAG(hpccard->Instance, XMC_FLAG_FallingEdge);
  }

  /* Check PCCARD interrupt FIFO empty flag */
  if(__XMC_PCCARD_GET_FLAG(hpccard->Instance, XMC_FLAG_FEMPT))
  {
    /* PCCARD interrupt callback*/
    PCCARD_ITCallback(hpccard);

    /* Clear PCCARD interrupt FIFO empty pending bit */
    __XMC_PCCARD_CLEAR_FLAG(hpccard->Instance, XMC_FLAG_FEMPT);
  }
}

/**
  * @brief  PCCARD interrupt feature callback
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @retval None
  */
__weak void PCCARD_ITCallback(PCCARD_HandleType *hpccard)
{
  /* Prevent unused argument(s) compilation warning */
//  UNUSED(hpccard);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the PCCARD_ITCallback could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup PCCARD_Exported_Functions_Group3 Peripheral State functions
 *  @brief   Peripheral State functions
 *
@verbatim
  ==============================================================================
                   ##### PCCARD Peripheral State functions #####
  ==============================================================================
  [..]
    This subsection permits to get in run-time the status of the PCCARD controller
    and the data flow.

@endverbatim
  * @{
  */

/**
  * @brief  return the PCCARD controller state
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @retval HAL state
  */
PCCARD_StateType PCCARD_GetState(PCCARD_HandleType *hpccard)
{
  return hpccard->State;
}
/**
  * @brief  Get the PCCARD error reg value.
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @retval reg: error reg value.
  */
uint8_t PCCARD_Read_ERROR_REG(PCCARD_HandleType *hpccard)
{
  uint8_t Reg = 0;

  /* Read ERROR Reg operation */
  Reg =  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_ERROR_REG);
  CF_print(ERR_REG_INFO, ("\r\nCF card ERROR Register value is %#x", Reg));
  return Reg;
}
/**
  * @brief  Get the PCCARD status reg value.
  * @param  hpccard: pointer to a PCCARD_HandleType structure that contains
  *                the configuration information for PCCARD module.
  * @retval reg: status reg value.
  */
uint8_t PCCARD_Read_STATUS_REG(PCCARD_HandleType *hpccard)
{
  uint8_t Reg = 0;

  /* Read STATUS Reg operation */
  Reg =  *(__IO uint8_t *)(hpccard->CF.IOAddr | ATA_STATUS_CMD);
  CF_print(STS_REG_INFO, ("\r\nCF card STATUS Register value is %#x", Reg));

  return Reg;
}


/**
  * @}
  */

/**
  * @}
  */


/**
  * @}
  */

/**
  * @}
  */