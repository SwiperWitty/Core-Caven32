/**
  ******************************************************************************
  * File   : USB_Device/VirtualMassStorage_IAP_Demo/src/usb_scsi.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : All processing related to the SCSI commands
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"
#include "usb_scsi.h"
#include "mass_mal.h"
#include "usb_bot.h"
#include "usb_regs.h"
#include "memory.h"
#include "usb_lib.h"
/** @addtogroup AT32F403A_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_VirtualMassStrorage_IAP_Demo
  * @{
  */ 
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
extern uint8_t Bulk_Data_Buff[BULK_MAX_PACKET_SIZE];  /* data buffer*/
extern uint8_t Bot_State;
extern Bulk_Only_CBW CBW;
extern Bulk_Only_CSW CSW;
extern uint32_t Mass_Memory_Size[2];
extern uint32_t Mass_Block_Size[2];
extern uint32_t Mass_Block_Count[2];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  SCSI Inquiry Command routine.
  * @param  lun: which lun
  * @retval None.
  */
void SCSI_Inquiry_Cmd(uint8_t lun)
{
  uint8_t* Inquiry_Data;
  uint16_t Inquiry_Data_Length;

  if (CBW.CB[1] & 0x01)/*Evpd is set*/
  {
    Inquiry_Data = Page00_Inquiry_Data;
    Inquiry_Data_Length = 5;
  }
  else
  {
    if ( lun == 0)
    {
      Inquiry_Data = Standard_Inquiry_Data;
    }
    else
    {
      Inquiry_Data = Standard_Inquiry_Data2;
    }
    if (CBW.CB[4] <= STANDARD_INQUIRY_DATA_LEN)
      Inquiry_Data_Length = CBW.CB[4];
    else
      Inquiry_Data_Length = STANDARD_INQUIRY_DATA_LEN;
  }
  Transfer_Data_Request(Inquiry_Data, Inquiry_Data_Length);
}

/**
  * @brief  SCSI ReadFormatCapacity Command routine.
  * @param  lun: which lun
  * @retval None.
  */
void SCSI_ReadFormatCapacity_Cmd(uint8_t lun)
{
  if (MAL_GetStatus(lun) != 0 )
  {
    Set_Scsi_Sense_Data(CBW.bLUN, NOT_READY, MEDIUM_NOT_PRESENT);
    Set_CSW (CSW_CMD_FAILED, SEND_CSW_ENABLE);
    Bot_Abort(DIR_IN);
    return;
  }
  ReadFormatCapacity_Data[4] = (uint8_t)(Mass_Block_Count[lun] >> 24);
  ReadFormatCapacity_Data[5] = (uint8_t)(Mass_Block_Count[lun] >> 16);
  ReadFormatCapacity_Data[6] = (uint8_t)(Mass_Block_Count[lun] >>  8);
  ReadFormatCapacity_Data[7] = (uint8_t)(Mass_Block_Count[lun]);

  ReadFormatCapacity_Data[9] = (uint8_t)(Mass_Block_Size[lun] >>  16);
  ReadFormatCapacity_Data[10] = (uint8_t)(Mass_Block_Size[lun] >>  8);
  ReadFormatCapacity_Data[11] = (uint8_t)(Mass_Block_Size[lun]);
  Transfer_Data_Request(ReadFormatCapacity_Data, READ_FORMAT_CAPACITY_DATA_LEN);
}

/**
  * @brief  SCSI ReadCapacity10 Command routine.
  * @param  lun: which lun
  * @retval None.
  */
void SCSI_ReadCapacity10_Cmd(uint8_t lun)
{
  if (MAL_GetStatus(lun))
  {
    Set_Scsi_Sense_Data(CBW.bLUN, NOT_READY, MEDIUM_NOT_PRESENT);
    Set_CSW (CSW_CMD_FAILED, SEND_CSW_ENABLE);
    Bot_Abort(DIR_IN);
    return;
  }

  ReadCapacity10_Data[0] = (uint8_t)((Mass_Block_Count[lun] - 1) >> 24);
  ReadCapacity10_Data[1] = (uint8_t)((Mass_Block_Count[lun] - 1) >> 16);
  ReadCapacity10_Data[2] = (uint8_t)((Mass_Block_Count[lun] - 1) >>  8);
  ReadCapacity10_Data[3] = (uint8_t)(Mass_Block_Count[lun] - 1);

  ReadCapacity10_Data[4] = (uint8_t)(Mass_Block_Size[lun] >>  24);
  ReadCapacity10_Data[5] = (uint8_t)(Mass_Block_Size[lun] >>  16);
  ReadCapacity10_Data[6] = (uint8_t)(Mass_Block_Size[lun] >>  8);
  ReadCapacity10_Data[7] = (uint8_t)(Mass_Block_Size[lun]);
  Transfer_Data_Request(ReadCapacity10_Data, READ_CAPACITY10_DATA_LEN);
}

/**
  * @brief  SCSI ModeSense6 Command routine.
  * @param  lun: which lun
  * @retval None.
  */
void SCSI_ModeSense6_Cmd (uint8_t lun)
{
  Transfer_Data_Request(Mode_Sense6_data, MODE_SENSE6_DATA_LEN);
}

/**
  * @brief  SCSI ModeSense10 Command routine.
  * @param  lun: which lun
  * @retval None.
  */
void SCSI_ModeSense10_Cmd (uint8_t lun)
{
  Transfer_Data_Request(Mode_Sense10_data, MODE_SENSE10_DATA_LEN);
}

/**
  * @brief  SCSI RequestSense Command routine.
  * @param  lun: which lun
  * @retval None.
  */
void SCSI_RequestSense_Cmd (uint8_t lun)
{
  uint8_t Request_Sense_data_Length;

  if (CBW.CB[4] <= REQUEST_SENSE_DATA_LEN)
  {
    Request_Sense_data_Length = CBW.CB[4];
  }
  else
  {
    Request_Sense_data_Length = REQUEST_SENSE_DATA_LEN;
  }
  Transfer_Data_Request(Scsi_Sense_Data, Request_Sense_data_Length);
}

/**
  * @brief  Set Scsi Sense Data routine.
  * @param  lun: which lun
  * @param  Sens_Key: sens key
  * @param  Asc: asc
  * @retval None.
  */
void Set_Scsi_Sense_Data(uint8_t lun, uint8_t Sens_Key, uint8_t Asc)
{
  Scsi_Sense_Data[2] = Sens_Key;
  Scsi_Sense_Data[12] = Asc;
}

/**
  * @brief  SCSI Start_Stop_Unit Command routine.
  * @param  lun: which lun
  * @retval None.
  */
void SCSI_Start_Stop_Unit_Cmd(uint8_t lun)
{
  Set_CSW (CSW_CMD_PASSED, SEND_CSW_ENABLE);
}

/**
  * @brief  SCSI Read10 Command routine.
  * @param  lun: which lun
  * @param  LBA: LBA value
  * @param  BlockNbr: Block Number
  * @retval None.
  */
void SCSI_Read10_Cmd(uint8_t lun , uint32_t LBA , uint32_t BlockNbr)
{
  if (Bot_State == BOT_IDLE)
  {
    if (!(SCSI_Address_Management(CBW.bLUN, SCSI_READ10, LBA, BlockNbr)))/*address out of range*/
    {
      return;
    }

    if ((CBW.bmFlags & 0x80) != 0)
    {
      Bot_State = BOT_DATA_IN;
      Read_Memory(lun, LBA , BlockNbr);
    }
    else
    {
      Bot_Abort(BOTH_DIR);
      Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
      Set_CSW (CSW_CMD_FAILED, SEND_CSW_ENABLE);
    }
    return;
  }
  else if (Bot_State == BOT_DATA_IN)
  {
    Read_Memory(lun , LBA , BlockNbr);
  }
}

/**
  * @brief  SCSI Write10 Command routine.
  * @param  lun: which lun
  * @param  LBA: LBA value
  * @param  BlockNbr: Block Number
  * @retval None.
  */
void SCSI_Write10_Cmd(uint8_t lun , uint32_t LBA , uint32_t BlockNbr)
{
  if (Bot_State == BOT_IDLE)
  {
    if (!(SCSI_Address_Management(CBW.bLUN, SCSI_WRITE10 , LBA, BlockNbr)))/*address out of range*/
    {
      return;
    }

    if ((CBW.bmFlags & 0x80) == 0)
    {
      Bot_State = BOT_DATA_OUT;
      SetEPRxStatus(ENDP2, EP_RX_VALID);
    }
    else
    {
      Bot_Abort(DIR_IN);
      Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
      Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
    }
    return;
  }
  else if (Bot_State == BOT_DATA_OUT)
  {
    Write_Memory(lun , LBA , BlockNbr);
  }
}

/**
  * @brief  SCSI Verify10 Command routine.
  * @param  lun: which lun
  * @retval None.
  */
void SCSI_Verify10_Cmd(uint8_t lun)
{
  if ((CBW.dDataLength == 0) && !(CBW.CB[1] & BLKVFY))/* BLKVFY not set*/
  {
    Set_CSW (CSW_CMD_PASSED, SEND_CSW_ENABLE);
  }
  else
  {
    Bot_Abort(BOTH_DIR);
    Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
    Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
  }
}
/**
  * @brief  Valid Commands routine.
  * @param  lun: which lun
  * @retval None.
  */
void SCSI_Valid_Cmd(uint8_t lun)
{
  if (CBW.dDataLength != 0)
  {
    Bot_Abort(BOTH_DIR);
    Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_COMMAND);
    Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
  }
  else
    Set_CSW (CSW_CMD_PASSED, SEND_CSW_ENABLE);
}

/**
  * @brief  Test Unit Ready Commands routine.
  * @param  lun: which lun
  * @retval None.
  */
void SCSI_TestUnitReady_Cmd(uint8_t lun)
{
  if (MAL_GetStatus(lun))
  {
    Set_Scsi_Sense_Data(CBW.bLUN, NOT_READY, MEDIUM_NOT_PRESENT);
    Set_CSW (CSW_CMD_FAILED, SEND_CSW_ENABLE);
    Bot_Abort(DIR_IN);
    return;
  }
  else
  {
    Set_CSW (CSW_CMD_PASSED, SEND_CSW_ENABLE);
  }
}

/**
  * @brief  Format Commands routine.
  * @param  lun: which lun
  * @retval None.
  */
void SCSI_Format_Cmd(uint8_t lun)
{
  if (MAL_GetStatus(lun))
  {
    Set_Scsi_Sense_Data(CBW.bLUN, NOT_READY, MEDIUM_NOT_PRESENT);
    Set_CSW (CSW_CMD_FAILED, SEND_CSW_ENABLE);
    Bot_Abort(DIR_IN);
    return;
  }
  else
  {
  }
}

/**
  * @brief  Invalid Commands routine
  * @param  lun: which lun
  * @retval None.
  */
void SCSI_Invalid_Cmd(uint8_t lun)
{
  if (CBW.dDataLength == 0)
  {
    Bot_Abort(DIR_IN);
  }
  else
  {
    if ((CBW.bmFlags & 0x80) != 0)
    {
      Bot_Abort(DIR_IN);
    }
    else
    {
      Bot_Abort(BOTH_DIR);
    }
  }
  Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_COMMAND);
  Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
}

/**
  * @brief  Test the received address.
  * @param  lun: which lun
  * @param  Cmd: the command can be SCSI_READ10 or SCSI_WRITE10.
  * @param  LBA: LBA Value
  * @param  BlockNbr: block number
  * @retval None.
  */
bool SCSI_Address_Management(uint8_t lun , uint8_t Cmd , uint32_t LBA , uint32_t BlockNbr)
{
  if ((LBA + BlockNbr) > Mass_Block_Count[lun] )
  {
    if (Cmd == SCSI_WRITE10)
    {
      Bot_Abort(BOTH_DIR);
    }
    Bot_Abort(DIR_IN);
    Set_Scsi_Sense_Data(lun, ILLEGAL_REQUEST, ADDRESS_OUT_OF_RANGE);
    Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
    return (FALSE);
  }

  if (CBW.dDataLength != BlockNbr * Mass_Block_Size[lun])
  {
    if (Cmd == SCSI_WRITE10)
    {
      Bot_Abort(BOTH_DIR);
    }
    else
    {
      Bot_Abort(DIR_IN);
    }
    Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
    Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
    return (FALSE);
  }
  return (TRUE);
}
/**
  * @}
  */ 

/**
  * @}
  */

