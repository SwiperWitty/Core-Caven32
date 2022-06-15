/**
  ******************************************************************************
  * File   : USB_Device/USB_Printer/src/usb_endp.c
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

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_USB_Printer
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
  SetEPTxValid(ENDP1);
}

/**
  * @brief  EP3 OUT Callback Routine.
  * @param  None.
  * @retval None.
  */
void EP3_OUT_Callback(void)
{
  usb_packet_receive = 1;
  usb_Receive_length = GetEPRxCount(ENDP3);
  /* user add processing code */
  PMAToUserBufferCopy((unsigned char*)usb_Receive_Buffer, ENDP3_RXADDR, usb_Receive_length);

	SetEPRxValid(ENDP3); 
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

