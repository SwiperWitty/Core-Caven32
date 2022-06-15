/**
  ******************************************************************************
  * File   : usbd_cdc_vcp.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Header for usbd_cdc_vcp.c file
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_CDC_VCP_H
#define __USBD_CDC_VCP_H

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_core_loopback.h"
#include "string.h"
/* Exported typef ------------------------------------------------------------*/
#define USB_FIFO_MAX     2048
#define VIRTUAL_COM_PORT_DATA_SIZE  64
typedef struct _usb_usart_fifo
{
  uint8_t  fifo[USB_FIFO_MAX];
  uint16_t wrpointer;      /*write pointer*/
  uint16_t curpointer;      /*cur pointer*/
  uint16_t remaindpointer; /*remaind pointer*/
  uint16_t total;
}usb_usart_fifo;

typedef struct
{
  uint32_t bitrate;
  uint8_t  format;
  uint8_t  paritytype;
  uint8_t  datatype;
}LINE_CODING;

extern usb_usart_fifo usb_txfifo;
extern usb_usart_fifo usb_rxfifo;
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* send data function */

uint16_t CDC_Receive_DATA(uint8_t *u8RcvBuffer, uint32_t u16RecvLen);
uint16_t CDC_Send_DATA (uint8_t *u8SendBuffer, uint16_t u16Sendlen);


#endif /* __USBD_CDC_VCP_H */

