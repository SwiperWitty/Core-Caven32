/**
  ******************************************************************************
  * File   : usb_sil.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Simplified Interface Layer for Global Initialization and Endpoint
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initialize the USB Device IP and the Endpoint 0.
  * @param  None
  * @retval 0.
  */
uint32_t USB_SIL_Init(void)
{
  /* USB interrupts initialization */
  /* clear pending interrupts */
  _SetINTSTS(0);
  wInterrupt_Mask = IMR_MSK;
  /* set interrupts mask */
  _SetCTRL(wInterrupt_Mask);
  return 0;
}

/**
  * @brief  Write a buffer of data to a selected endpoint.
  * @param  bEpAddr: The address of the non control endpoint.
  * @param  pBufferPointer: The pointer to the buffer of data to be written to the endpoint.
  * @param  wBufferSize: Number of data to be written (in bytes).
  * @retval 0.
  */
uint32_t USB_SIL_Write(uint8_t bEpAddr, uint8_t* pBufferPointer, uint32_t wBufferSize)
{
  /* Use the memory interface function to write to the selected endpoint */
  UserToPMABufferCopy(pBufferPointer, GetEPTxAddr(bEpAddr & 0x7F), wBufferSize);

  /* Update the data length in the control register */
  SetEPTxCount((bEpAddr & 0x7F), wBufferSize);
  
  return 0;
}

/**
  * @brief  read a buffer of data from the selected endpoint.
  * @param  bEpAddr: The address of the non control endpoint.
  * @param  pBufferPointer: The pointer to the buffer of data to be written to the endpoint.
  * @retval Number of data read.
  */
uint32_t USB_SIL_Read(uint8_t bEpAddr, uint8_t* pBufferPointer)
{
  uint32_t DataLength = 0;

  /* Get the number of received data on the selected Endpoint */
  DataLength = GetEPRxCount(bEpAddr & 0x7F);
  
  /* Use the memory interface function to write to the selected endpoint */
  PMAToUserBufferCopy(pBufferPointer, GetEPRxAddr(bEpAddr & 0x7F), DataLength);

  /* Return the number of received data */
  return DataLength;
}


