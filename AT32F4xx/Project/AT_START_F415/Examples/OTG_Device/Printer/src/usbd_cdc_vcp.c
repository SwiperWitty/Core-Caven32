/**
  ******************************************************************************
  * File   : USB_Device/USB_Printer/src/usbd_cdc_vcp.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file includes cdc application
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_vcp.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
usb_usart_fifo usb_txfifo;
usb_usart_fifo usb_rxfifo;
uint8_t usb_packet_sent = 0;
LINE_CODING linecoding =
{
    115200,                 /* baud rate    */
    0x00,     /* stop bits-1  */
    0x00,  /* parity - none*/
    0x08                    /* nb. of bits 8*/
};

extern uint8_t cdc_rx_buffer[];

extern USB_OTG_CORE_HANDLE  USB_OTG_dev;

/* Private function prototypes -----------------------------------------------*/
static uint16_t CDC_DataTx   (void);
static uint16_t CDC_DataRx (uint8_t *usb_Receive_Buffer, uint32_t Len);
static uint16_t CDC_Init(void);
static uint16_t CDC_Reset(void);
static uint16_t CDC_Ctrl (uint32_t Cmd, uint8_t* Buf, uint32_t Len);

CDC_IF_Prop_Type CDC_vcp_fops = 
{
  CDC_Init,
  CDC_Reset,
  CDC_Ctrl,
  CDC_DataTx,
  CDC_DataRx
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  CDC_Init
  *         Initializes the Media on the AT32
  * @param  None
  * @retval Result of the operation (USB_DEVICE_OK in all cases)
  */
static uint16_t CDC_Init(void)
{ 
    /* COM1 default configuration */
    /* COM1 configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - Parity Odd
        - Hardware flow control disabled
        - Receive and transmit enabled
    */
  
    return USB_DEVICE_OK;
}

/**
  * @brief  CDC_Reset
  *         DeInitializes the Media on the AT32
  * @param  None
  * @retval Result of the operation (USBD_OK in all cases)
  */
static uint16_t CDC_Reset(void)
{
    return USB_DEVICE_OK;
}
/**
  * @brief  CDC_Ctrl
  *         Manage the CDC class requests
  * @param  Cmd: Command code            
  * @param  Buf: Buffer containing command data (request parameters)
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the operation (USB_DEVICE_OK in all cases)
  */
static uint16_t CDC_Ctrl (uint32_t Cmd, uint8_t* Buf, uint32_t Len)
{ 
    switch (Cmd)
    {
        case SEND_ENCAPSULATED_COMMAND:/* Issues a command in the format of the supported control protocol */
            /* Not  needed for this driver */
            break;

        case GET_ENCAPSULATED_RESPONSE:/* Requests a response in the format of the supported control protocol */
            /* Not  needed for this driver */
            break;

        case SET_COMM_FEATURE:/* Controls the settings for a particular communications feature */
            /* Not  needed for this driver */
            break;

        case GET_COMM_FEATURE:/* Returns the current settings for the communications feature */
            /* Not  needed for this driver */
            break;

        case CLEAR_COMM_FEATURE:/* Clears the settings for a particular communications feature */
            /* Not  needed for this driver */
            break;

        case SET_LINE_CODING: /* Configures DTE rate, stop-bits, parity, and number-of-character bits */
            linecoding.bitrate      = (uint32_t)(Buf[0] | (Buf[1] << 8) | (Buf[2] << 16) | (Buf[3] << 24));
            linecoding.format       = Buf[4];
            linecoding.paritytype   = Buf[5];
            linecoding.datatype     = Buf[6];
            /* Set the new configuration */
            break;

        case GET_LINE_CODING:/* Requests current DTE rate, stop-bits, parity, and number-of-character bits */
            Buf[0] = (uint8_t)(linecoding.bitrate);
            Buf[1] = (uint8_t)(linecoding.bitrate >> 8);
            Buf[2] = (uint8_t)(linecoding.bitrate >> 16);
            Buf[3] = (uint8_t)(linecoding.bitrate >> 24);
            Buf[4] = linecoding.format;
            Buf[5] = linecoding.paritytype;
            Buf[6] = linecoding.datatype; 
            break;

        case SET_CONTROL_LINE_STATE:/* signal used to tell the DCE device the DTE device is now present */
            /* Not  needed for this driver */
            break;

        case SEND_BREAK:/* Sends special carrier modulation used to specify style break */
            /* Not  needed for this driver */
            break;    
    
        default:
            break;
    }

    return USB_DEVICE_OK;
}
/**
  * @brief  CDC_DataTx
  *         CDC received data to be send over USB IN endpoint are managed in 
  *         this function.
  * @retval Result of the operation: USB_DEVICE_OK if all operations are OK else FAIL
  */
static uint16_t CDC_DataTx (void)
{ 
  uint16_t dwSendLen = 0;
	uint32_t SendPtr = 0;
  if (usb_txfifo.wrpointer == usb_txfifo.curpointer )
	return USB_DEVICE_FAIL;
	
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
  
  USB_DCD_EPT_Tx (&USB_OTG_dev, CDC_IN_EP, &usb_txfifo.fifo[SendPtr] , dwSendLen );
  return USB_DEVICE_OK;
}

/**
  * @brief  CDC_DataRx
  *         Data received over USB OUT endpoint are sent over CDC interface 
  *         through this function.
  *           
  *         @note
  *         This function will block any OUT packet reception on USB endpoint 
  *         until exiting this function. If you exit this function before transfer
  *         is complete on CDC interface (ie. using DMA controller) it will result 
  *         in receiving more data while previous ones are still not sent.
  *
  * @param  usb_Receive_Buffer: data received 
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else VCP_FAIL
  */
static uint16_t CDC_DataRx (uint8_t *usb_Receive_Buffer, uint32_t Len)
{ 
  uint16_t i = 0;
  if ( usb_rxfifo.wrpointer >= USB_FIFO_MAX)
	  usb_rxfifo.wrpointer = 0;
  for ( i = 0; i < Len; i ++)
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
    USB_DCD_EPT_PrepareRx(&USB_OTG_dev, CDC_OUT_EP, cdc_rx_buffer, VIRTUAL_COM_PORT_DATA_SIZE);
  }
  
  return USB_DEVICE_OK;
}

/**
  * @brief  CDC_Send_DATA
  *         send the data received from the AT32 to the PC through USB 
  * @retval send len
  */
uint16_t CDC_Send_DATA (uint8_t *u8SendBuffer, uint16_t u16Sendlen)
{
  uint16_t  i = 0;
  if ( usb_txfifo.wrpointer >= USB_FIFO_MAX )
      usb_txfifo.wrpointer = 0;
    
  for (i = 0 ; usb_txfifo.wrpointer < USB_FIFO_MAX && i <  u16Sendlen; i ++ )
  {
      usb_txfifo.fifo[usb_txfifo.wrpointer] = u8SendBuffer[i];
      usb_txfifo.wrpointer ++;
      if ( usb_txfifo.wrpointer >= USB_FIFO_MAX )
          usb_txfifo.wrpointer = 0;  
  }
  usb_packet_sent = 1;
  return u16Sendlen;
}

/**
  * @brief  CDC_Receive_DATA
  *         Get the receive data
  * @retval data len
  */
uint16_t CDC_Receive_DATA(uint8_t *u8RcvBuffer, uint32_t u16RecvLen)
{ 
  uint16_t dwTmpLen = 0, i = 0, dwTmpPtr = usb_rxfifo.curpointer;
  if ( usb_rxfifo.curpointer == usb_rxfifo.wrpointer )
  {
      return 0;
  }
  if ( usb_rxfifo.wrpointer > usb_rxfifo.curpointer )
  {
      dwTmpLen = usb_rxfifo.wrpointer - usb_rxfifo.curpointer;
  }    
  else
  {
      dwTmpLen = USB_FIFO_MAX - usb_rxfifo.curpointer;
  }
  
  if ( dwTmpLen > u16RecvLen )
      dwTmpLen = u16RecvLen;
  
  usb_rxfifo.curpointer += dwTmpLen;
  if ( usb_rxfifo.curpointer >= USB_FIFO_MAX )
      usb_rxfifo.curpointer = 0;
  for ( i = 0; i < dwTmpLen; i ++ )
  {
      u8RcvBuffer[i] = usb_rxfifo.fifo[dwTmpPtr ++];
  }
  if ( usb_rxfifo.remaindpointer == 1 )
  {
    USB_DCD_EPT_PrepareRx(&USB_OTG_dev, CDC_OUT_EP, cdc_rx_buffer, VIRTUAL_COM_PORT_DATA_SIZE);
  }
  return dwTmpLen;
}

