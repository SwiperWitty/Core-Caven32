/**
  ******************************************************************************
  * File   : USB_Device/VirtualMassStorage_IAP_Demo/src/mass_mal.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Medium Access Layer interface
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "mass_mal.h"
#include "flash_if.h"
#include "FAT16.h"
/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_VirtualMassStrorage_IAP_Demo
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


//SD_CardInfo mSDCardInfo;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes the Media on the AT32
  * @param  lun: 0 is FLASH
  * @retval status of media init(MAL_OK/MAL_FAIL).
  */
uint16_t MAL_Init(uint8_t lun)
{
  uint16_t status = MAL_OK;

  switch (lun)
  {
    case 0:
      AT_FLASH_Init();
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
  * @param  lun: 0 is FLASH, others not support
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
    {
        FATWriteLBA(Memory_Offset,(uint8_t*) Writebuff, (uint32_t) Transfer_Length);
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
  * @param  lun: 0 is FLASH, others not support
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
    {
        if(0 == FATReadLBA(Memory_Offset,(uint8_t*)Readbuff, (uint32_t)Transfer_Length))
        {
            AT_FLASH_ReadBuffer(FLASH_START_ADDR+Memory_Offset, (uint8_t *)Readbuff, Transfer_Length);
        }
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
  * @param  lun: 0 is FLASH, others not support
  * @retval status (MAL_OK/MAL_FAIL).
  */
uint16_t MAL_GetStatus (uint8_t lun)
{
  if ( lun == 0 )
  {
    Mass_Block_Count[0] = FLASH_SIZE / FATBytesPerSec;
    Mass_Block_Size[0] = FATBytesPerSec;
    Mass_Memory_Size[0] = FLASH_SIZE;
    return MAL_OK;
  }

  return MAL_FAIL;
}
/**
  * @}
  */ 

/**
  * @}
  */



