/**
  ******************************************************************************
  * File   : USB_Device/VirtualComPort_loopback/src/usb_endp.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Endpoint routines
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_istr.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "hw_config.h"

/** @addtogroup AT32F403A_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_VirtualComPort_loopback
  * @{
  */ 
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern uint8_t usb_packet_sent;
extern uint8_t usb_packet_receive;
extern uint8_t usb_Receive_Buffer[];
extern uint16_t usb_Receive_length;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  EP1 IN Callback Routine.
  * @param  None.
  * @retval None.
  */
void EP1_IN_Callback(void)
{
    uint16_t dwSendLen = 0;
	uint32_t SendPtr = 0;
	/*no data need send*/
	if (usb_txfifo.wrpointer == usb_txfifo.curpointer )
		return;
	
	if ( usb_txfifo.wrpointer > usb_txfifo.curpointer )
	{
	    dwSendLen = usb_txfifo.wrpointer - usb_txfifo.curpointer;
	}		
	else
	{
		dwSendLen = USB_FIFO_MAX - usb_txfifo.curpointer;
	}
	SendPtr = usb_txfifo.curpointer;
	if ( dwSendLen > VIRTUAL_COM_PORT_DATA_SIZE )
	{
		dwSendLen = VIRTUAL_COM_PORT_DATA_SIZE;
	}
	usb_txfifo.curpointer += dwSendLen;
	
	if ( usb_txfifo.curpointer >= USB_FIFO_MAX )
		usb_txfifo.curpointer = 0;
	
    /* send  packet to PMA*/
    UserToPMABufferCopy(&usb_txfifo.fifo[SendPtr], ENDP1_TXADDR, dwSendLen);
    SetEPTxCount(ENDP1, dwSendLen);
    SetEPTxValid(ENDP1);
}

/**
  * @brief  EP3 OUT Callback Routine.
  * @param  None.
  * @retval None.
  */
void EP3_OUT_Callback(void)
{
  uint16_t i = 0;
  usb_packet_receive = 1;
  usb_Receive_length = GetEPRxCount(ENDP3);

  PMAToUserBufferCopy((unsigned char*)usb_Receive_Buffer, ENDP3_RXADDR, usb_Receive_length);
  if ( usb_rxfifo.wrpointer >= USB_FIFO_MAX)
	  usb_rxfifo.wrpointer = 0;
  for ( i = 0; i < usb_Receive_length; i ++)
  {
	  usb_rxfifo.fifo[usb_rxfifo.wrpointer++] = usb_Receive_Buffer[i];
	  if ( usb_rxfifo.wrpointer >= USB_FIFO_MAX )
		  usb_rxfifo.wrpointer = 0;
  }
  if ( (((usb_rxfifo.wrpointer - usb_rxfifo.curpointer) > (USB_FIFO_MAX - VIRTUAL_COM_PORT_DATA_SIZE)) && (usb_rxfifo.wrpointer > usb_rxfifo.curpointer ))
	  || ((usb_rxfifo.curpointer - usb_rxfifo.wrpointer < VIRTUAL_COM_PORT_DATA_SIZE) && usb_rxfifo.curpointer > usb_rxfifo.wrpointer))
  {
	  usb_rxfifo.remaindpointer = 1; /*no space recevice next packet*/
  }else
  {
	  usb_rxfifo.remaindpointer = 0;
	  SetEPRxValid(ENDP3); 
  }
}

/**
  * @brief  SOF Callback Routine.
  * @param  None.
  * @retval None.
  */
void SOF_Callback(void)
{
//	static uint32_t FrameCount = 0;

	if(bDeviceState == CONFIGURED)
	{
		if (usb_packet_sent == 1 && ( GetEPTxStatus(ENDP1) != EP_TX_VALID ))
		{
			/* Check the data to be sent through IN pipe */
			EP1_IN_Callback();
			usb_packet_sent = 0;
		}
	}  
}

/**
  * @}
  */

/**
  * @}
  */

