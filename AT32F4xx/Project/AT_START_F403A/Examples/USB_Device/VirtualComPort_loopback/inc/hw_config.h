/**
  ******************************************************************************
  * File   : USB_Device/VirtualComPort_loopback/inc/hw_config.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : USB Hardware Configuration & setup header
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include <at32f4xx.h>
#include "usb_type.h"


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/


#define USB_FIFO_MAX     1024
#define USBCLK_FROM_HSE  1
#define USBCLK_FROM_HSI  2
typedef struct _usb_usart_fifo
{
  uint8_t  fifo[USB_FIFO_MAX];
  uint16_t wrpointer;      /*write pointer*/
  uint16_t curpointer;      /*cur pointer*/
  uint16_t remaindpointer; /*remaind pointer*/
  uint16_t total;
}usb_usart_fifo;

extern usb_usart_fifo usb_txfifo;
extern usb_usart_fifo usb_rxfifo;

/* Exported functions ------------------------------------------------------- */
void Set_System(void);
void Set_USBClock(u8 Clk_Source);
void GPIO_AINConfig(void);
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void USB_Interrupts_Config(void);
void USB_Cable_Config (FunctionalState NewState);

void Get_SerialNum(void);
uint16_t CDC_Send_DATA (uint8_t *u8SendBuffer, uint16_t u16Sendlen);
uint16_t CDC_Receive_DATA(uint8_t *u8RcvBuffer, uint16_t u16RecvLen);
#endif  /*__HW_CONFIG_H*/


