/**
  ******************************************************************************
  * File   : FreeRTOS/bsp/bsp_uart_fifo.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Bsp Service Routines.
  ******************************************************************************
  */
  

#ifndef _BSP_USART_FIFO_H_
#define _BSP_USART_FIFO_H_

/*
 * function configuration:
 *
 * (1)USART1 -----> RS232
	  PA9/USART1_TX	 
	  PA10/USART1_RX
	  
 * (2)USART2 -----> GPRS
	  PA2/USART2_TX
	  PA3/USART2_RX 

 * (3)USART3 -----> RS485
	  PB10/USART3_TX
	  PB11/USART3_RX
	  PB2/BOOT1/RS485_TX_EN
 */
 
#define	UART1_FIFO_EN	1

typedef enum
{
	COM1 = 0,	/* USART1  PA9, PA10 */
	COM2 = 1,	/* USART2, PA2, PA3 */
	COM3 = 2,	/* USART3, PB10, PB11 */
	COM4 = 3,	/* UART4, PC10, PC11 */
	COM5 = 4,	/* UART5, PC12, PD2 */
}COM_PORT_E;

/* define baudrate, the size of FIFO buffer, support full-duplex transmission */
#if UART1_FIFO_EN == 1
	#define UART1_BAUD			115200
	#define UART1_TX_BUF_SIZE	2*1024
	#define UART1_RX_BUF_SIZE	2*1024
#endif

typedef struct
{
	USART_Type *uart;		            /* Device */
	uint8_t *pTxBuf;			        /* point to transmit buffer */
	uint8_t *pRxBuf;			        /* point to receive buffer */
	uint16_t usTxBufSize;		        /* size of transmit buffer */
	uint16_t usRxBufSize;		        /* size of receive buffer */
	__IO uint16_t usTxWrite;		    /* index for write */
	__IO uint16_t usTxRead;			    /* index for read */
	__IO uint16_t usTxCount;		    /* the remain number of waiting */

	__IO uint16_t usRxWrite;		    /* index for write */
	__IO uint16_t usRxRead;			    /* index for read */
	__IO uint16_t usRxCount;		    /* the remain number for reading */

	void (*SendBefor)(void); 	        /* callback function for before starting transmit£¨used to change to transmit mode for RS485£© */
	void (*SendOver)(void); 	        /* callback function for after transmit complete£¨used to change to receive mode for RS485£© */
	void (*ReciveNew)(uint8_t _byte);	/* callback function while some data arrived */
}UART_T;

void bsp_InitUart(void);
void comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen);
void comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte);
uint8_t comGetChar(COM_PORT_E _ucPort, uint8_t *_pByte);

void comClearTxFifo(COM_PORT_E _ucPort);
void comClearRxFifo(COM_PORT_E _ucPort);

void bsp_SetUart1Baud(uint32_t _baud);

#endif



