/**
  ******************************************************************************
  * File   : USB_Device/Custom_HID/src/usb_endp.c
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
#include "at32_board.h"
/** @addtogroup AT32F413_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_Custom_HID
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

uint8_t Receive_Buffer[64];
extern __IO uint8_t PrevXferComplete;
extern  vu32 bDeviceState; 
u8 usb_fifo[64] = {8,2,3,4,5,6,7,8,9,0};
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  EP1 OUT Callback Routine, rx data from host use endpoint 1
  * @param  None
  * @retval None.
  */
void EP1_OUT_Callback(void)
{
  
  BitState Led_State;
  int i=0;

  /* Read received data (2 bytes) */  
  usb_Receive_length = GetEPRxCount(EP1_OUT & 0x7F);
  PMAToUserBufferCopy(Receive_Buffer, GetEPRxAddr(EP1_OUT & 0x7F), usb_Receive_length);
  if (Receive_Buffer[1] == 0)
  {
    Led_State = Bit_RESET;
  }
  else 
  {
    Led_State = Bit_SET;
  }
 
  switch (Receive_Buffer[0])
  {
    case 2: /* Led 2 */
     if (Led_State != Bit_RESET)
     {
       AT32_LEDn_ON(LED2);
     }
     else
     {
       AT32_LEDn_OFF(LED2);
     }
      break;
    case 3: /* Led 3 */
     if (Led_State != Bit_RESET)
     {
       AT32_LEDn_ON(LED3);
     }
     else
     {
       AT32_LEDn_OFF(LED3);
     }
      break;
    case 4: /* Led 4 */
     if (Led_State != Bit_RESET)
     {
       AT32_LEDn_ON(LED4);
     }
     else
     {
       AT32_LEDn_OFF(LED4);
     }
      break;
     
     case 0xF0: /* Loop Back transmit */
     if ( usb_rxfifo.wrpointer >= USB_FIFO_MAX)
	   usb_rxfifo.wrpointer = 0;
     for ( i = 0; i < usb_Receive_length; i ++)
     {
       usb_rxfifo.fifo[usb_rxfifo.wrpointer++] = Receive_Buffer[i];
       if ( usb_rxfifo.wrpointer >= USB_FIFO_MAX )
         usb_rxfifo.wrpointer = 0;
     }
     if ( (((usb_rxfifo.wrpointer - usb_rxfifo.curpointer) > (USB_FIFO_MAX - 64)) && (usb_rxfifo.wrpointer > usb_rxfifo.curpointer ))
       || ((usb_rxfifo.curpointer - usb_rxfifo.wrpointer < 64) && usb_rxfifo.curpointer > usb_rxfifo.wrpointer))
     {
       usb_rxfifo.remaindpointer = 1; /*no space recevice next packet*/
     }else
     {
       usb_rxfifo.remaindpointer = 0;
       SetEPRxValid(ENDP1); 
     }
      break;
     
    default:
    AT32_LEDn_OFF(LED2);
    AT32_LEDn_OFF(LED3);
    AT32_LEDn_OFF(LED4); 
    break;
  }
 
  SetEPRxStatus(ENDP1, EP_RX_VALID);

}

/**
  * @brief  EP1 IN Callback, Tx data complete
  * @param  None
  * @retval None.
  */
void EP1_IN_Callback(void)
{
  uint16_t dwSendLen = 0;
	uint32_t SendPtr = 0;  
  PrevXferComplete = 1;
  
	/*no data need send*/
	if (usb_txfifo.wrpointer == usb_txfifo.curpointer )
  {
      usb_txfifo.wrpointer =0;
      usb_txfifo.curpointer = 0;
      return;
  }
	
	if ( usb_txfifo.wrpointer > usb_txfifo.curpointer )
	{
	    dwSendLen = usb_txfifo.wrpointer - usb_txfifo.curpointer;
	}		
	else
	{
		dwSendLen = USB_FIFO_MAX - usb_txfifo.curpointer;
	}
	SendPtr = usb_txfifo.curpointer;
	if ( dwSendLen > 64 )
	{
		dwSendLen = 64;
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
  * @brief  EP2 OUT Callback Routine, rx data from host use endpoint 2
  * @param  None
  * @retval None.
  */
void EP2_OUT_Callback(void)
{

}

/**
  * @brief  EP2 IN Callback, Tx data complete
  * @param  None
  * @retval None.
  */
void EP2_IN_Callback(void)
{
}

/**
  * @brief  SOF Callback Routine.
  * @param  None.
  * @retval None.
  */
void SOF_Callback(void)
{

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
 

