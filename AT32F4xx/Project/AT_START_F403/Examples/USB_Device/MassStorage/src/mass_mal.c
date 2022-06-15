/**
  ******************************************************************************
  * File   : USB_Device/MassStorage/src/mass_mal.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Medium Access Layer interface
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "mass_mal.h"
#include "at32_sdio_sd.h"
/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_MassStorage
  * @{
  */ 
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t Mass_Memory_Size[2];
uint32_t Mass_Block_Size[2];
uint32_t Mass_Block_Count[2];
__IO uint32_t Status = 0;


SD_CardInfo mSDCardInfo;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes the Media on the AT32
  * @param  lun: 0 is SD Card
  * @retval status of media init(MAL_OK/MAL_FAIL).
  */
uint16_t MAL_Init(uint8_t lun)
{
  uint16_t status = MAL_OK;

  switch (lun)
  {
    case 0:
      status = SD_Init();  //FLASH
      break;
    case 1:
      break;
    default:
      return MAL_FAIL;
  }
  return status;
}

/**
  * @brief  Write data to Storage
  * @param  lun: 0 is SD Card, others not support
  * @param  Memory_Offset: Write address offset
  * @param  Writebuff: Write data buffer pointer
  * @param  Transfer_Length: Write data length
  * @retval status of Write data(MAL_OK/MAL_FAIL).
  */
uint16_t MAL_Write(uint8_t lun, uint32_t Memory_Offset, uint32_t *Writebuff, uint16_t Transfer_Length)
{

  switch (lun)
  {
    case 0:
    Status = SD_WriteMultiBlocks((uint8_t*)Writebuff, Memory_Offset, Transfer_Length,Transfer_Length >> 9);
    Status = SD_WaitWriteOperation();  
    while(SD_GetStatus() != SD_TRANSFER_OK)
    {    
    }
      if ( Status != SD_OK )
      {
        return MAL_FAIL;
      }           
      break;
    case 1:
      break;  
    default:
      return MAL_FAIL;
  }
  return MAL_OK;
}

/**
  * @brief  Read data from Storage
  * @param  lun: 0 is SD Card, others not support
  * @param  Memory_Offset: Write address offset
  * @param  Readbuff: read data buffer pointer
  * @param  Transfer_Length: read data length
  * @retval status of read data(MAL_OK/MAL_FAIL).
  */
uint16_t MAL_Read(uint8_t lun, uint32_t Memory_Offset, uint32_t *Readbuff, uint16_t Transfer_Length)
{

  switch (lun)
  {
    case 0:

      SD_ReadMultiBlocks((uint8_t*)Readbuff, Memory_Offset, Transfer_Length, Transfer_Length >> 9);
      Status = SD_WaitReadOperation();
      while(SD_GetStatus() != SD_TRANSFER_OK)
      {
      }
      
      if ( Status != SD_OK )
      {
        return MAL_FAIL;
      }     
      break;
    case 1:
      ;
      break;
    default:
      return MAL_FAIL;
  }
  return MAL_OK;
}

/**
  * @brief  get mal status
  * @param  lun: 0 is SD Card, others not support
  * @retval status (MAL_OK/MAL_FAIL).
  */
uint16_t MAL_GetStatus (uint8_t lun)
{

  uint32_t DeviceSizeMul = 0, NumberOfBlocks = 0;

  if (lun == 0)
  {
    if (SD_Init() == SD_OK)
    {
      SD_GetCardInfo(&mSDCardInfo);
      SD_SelectDeselect((uint32_t) (mSDCardInfo.RCA << 16));
      DeviceSizeMul = (mSDCardInfo.SD_csd.DeviceSizeMul + 2);

      if(mSDCardInfo.CardType == SDIO_HIGH_CAPACITY_SD_CARD)
      {
        Mass_Block_Count[0] = (mSDCardInfo.SD_csd.DeviceSize + 1) * 1024;
      }
      else
      {
        NumberOfBlocks  = ((1 << (mSDCardInfo.SD_csd.RdBlockLen)) / 512);
        Mass_Block_Count[0] = ((mSDCardInfo.SD_csd.DeviceSize + 1) * (1 << DeviceSizeMul) << (NumberOfBlocks/2));
      }
      Mass_Block_Size[0]  = 512;

      Status = SD_SelectDeselect((uint32_t) (mSDCardInfo.RCA << 16)); 
      Status = SD_EnableWideBusOperation(SDIO_BusWidth_4b); 
      if ( Status != SD_OK )
      {
        return MAL_FAIL;
      }
     
      Mass_Memory_Size[0] = Mass_Block_Count[0] * Mass_Block_Size[0];
      return MAL_OK;

    }

  }
  else
  {

  }

  return MAL_FAIL;
}


/**
  * @}
  */

/**
  * @}
  */
  
