/**
  ******************************************************************************
  * File   : FreeRTOS/bsp/bsp_uart_fifo.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Bsp Service Routines.
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/  
#include "bsp.h"
#include "at32_board.h"
/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup FreeRTOS_CountSemaphore_SynTaskWithTask
  * @{
  */ 
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/  
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#if UART1_FIFO_EN == 1
	static UART_T g_tUart1;
	static uint8_t g_TxBuf1[UART1_TX_BUF_SIZE];		/* transmit buffer */
	static uint8_t g_RxBuf1[UART1_RX_BUF_SIZE];		/* receive buffer */
#endif

/* Private function prototypes -----------------------------------------------*/
static void UartVarInit(void);

static void InitHardUart(void);
static void UartSend(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen);
static uint8_t UartGetChar(UART_T *_pUart, uint8_t *_pByte);
static void UartIRQ(UART_T *_pUart);
static void ConfigUartNVIC(void);

/* Private functions ---------------------------------------------------------*/

/**
 * @brief   Initialize all hardware included on the AT-STRAT-F403 board
 * @param   None
 * @retval  None
 */
void bsp_InitUart(void)
{
	UartVarInit();		/* initialize all global parameters */

	InitHardUart();		/* initialize USART */

	ConfigUartNVIC();	/* configurate USART interrupts */
}

/**
 * @brief   change serial number to USART device
 * @param   _ucPort: serial number
            COM1
            COM2
            COM3
            COM4
            COM5
            COM6
 * @retval  USART device
 */

UART_T *ComToUart(COM_PORT_E _ucPort)
{
	if (_ucPort == COM1)
	{
		#if UART1_FIFO_EN == 1
			return &g_tUart1;
		#else
			return 0;
		#endif
	}
	else
	{
		return 0;
	}
}

/**
 * @brief   send data to TX FIFO,and in fact send out data by ISR
 * @param   _ucPort: serial number that send the data
                COM1
                COM2
                COM3
                COM4
                COM5
                COM6
            _ucaBuf: buffer contain data that will be sent
            _usLen : length of sent data
 * @retval  None
 */
void comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen)
{
	UART_T *pUart;

	pUart = ComToUart(_ucPort);
	if (pUart == 0)
	{
		return;
	}

	if (pUart->SendBefor != 0)
	{
		pUart->SendBefor(); /* changed to TX mode when used as RS485 communication */
	}

	UartSend(pUart, _ucaBuf, _usLen);
}

/**
 * @brief   send one byte data to TX FIFO,and in fact send out data by ISR
 * @param   _ucPort: serial number that send the data
                COM1
                COM2
                COM3
                COM4
                COM5
                COM6
            _ucByte: the sent data
 * @retval  None
 */

void comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte)
{
	comSendBuf(_ucPort, &_ucByte, 1);
}

/**
 * @brief   obtain one byte data from receive FIFO, none blocked mode
 * @param   _ucPort: serial number that send the data
                COM1
                COM2
                COM3
                COM4
                COM5
                COM6
            _ucByte: the received data
 * @retval  0: no data 1:obtain the data
 */

uint8_t comGetChar(COM_PORT_E _ucPort, uint8_t *_pByte)
{
	UART_T *pUart;

	pUart = ComToUart(_ucPort);
	if (pUart == 0)
	{
		return 0;
	}

	return UartGetChar(pUart, _pByte);
}

/**
  * @brief  Clear TX fifo data
  * @param  _ucPort: serial number that send the data
                COM1
                COM2
                COM3
                COM4
                COM5
                COM6
  * @retval None
  */
void comClearTxFifo(COM_PORT_E _ucPort)
{
	UART_T *pUart;

	pUart = ComToUart(_ucPort);
	if (pUart == 0)
	{
		return;
	}

	pUart->usTxWrite = 0;
	pUart->usTxRead = 0;
	pUart->usTxCount = 0;
}

/**
  * @brief  Clear RX fifo data
  * @param  _ucPort: serial number that send the data
                COM1
                COM2
                COM3
                COM4
                COM5
                COM6
  * @retval None
  */
void comClearRxFifo(COM_PORT_E _ucPort)
{
	UART_T *pUart;

	pUart = ComToUart(_ucPort);
	if (pUart == 0)
	{
		return;
	}

	pUart->usRxWrite = 0;
	pUart->usRxRead = 0;
	pUart->usRxCount = 0;
}

/**
 * @brief   reconfigurate the baudrate
 * @param   None
 * @retval  None
 */

void bsp_SetUart1Baud(uint32_t _baud)
{
	USART_InitType USART_InitStructure;

	USART_InitStructure.USART_BaudRate      = _baud;	
	USART_InitStructure.USART_WordLength    = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits      = USART_StopBits_1;
	USART_InitStructure.USART_Parity        = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                  = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(AT32_PRINT_UART, &USART_InitStructure);
}

/**
  * @brief  Uart parameter initialization
  * @param  None
  * @retval None
  */
static void UartVarInit(void)
{
#if UART1_FIFO_EN == 1
	g_tUart1.uart           = AT32_PRINT_UART;						
	g_tUart1.pTxBuf         = g_TxBuf1;					
	g_tUart1.pRxBuf         = g_RxBuf1;					
	g_tUart1.usTxBufSize    = UART1_TX_BUF_SIZE;	
	g_tUart1.usRxBufSize    = UART1_RX_BUF_SIZE;	
	g_tUart1.usTxWrite      = 0;						
	g_tUart1.usTxRead       = 0;						
	g_tUart1.usRxWrite      = 0;						
	g_tUart1.usRxRead       = 0;						
	g_tUart1.usRxCount      = 0;						
	g_tUart1.usTxCount      = 0;						
	g_tUart1.SendBefor      = 0;						
	g_tUart1.SendOver       = 0;						
	g_tUart1.ReciveNew      = 0;						
#endif
}

/**
  * @brief  Hard uart initialization
  * @param  None
  * @retval None
  */
static void InitHardUart(void)
{
	GPIO_InitType GPIO_InitStructure;
	USART_InitType USART_InitStructure;

#if UART1_FIFO_EN == 1		
	RCC_APB2PeriphClockCmd(AT32_PRINT_UARTTX_GPIO_RCC | RCC_APB2PERIPH_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(AT32_PRINT_UARTRX_GPIO_RCC,ENABLE);  
	RCC_APB2PeriphClockCmd(AT32_PRINT_UART_RCC, ENABLE);

	GPIO_InitStructure.GPIO_Pins        = AT32_PRINT_UARTTX_PIN;
	GPIO_InitStructure.GPIO_Mode        = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_MaxSpeed    = GPIO_MaxSpeed_50MHz;
	GPIO_Init(AT32_PRINT_UARTTX_GPIO, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pins        = AT32_PRINT_UARTRX_PIN;
	GPIO_InitStructure.GPIO_Mode        = GPIO_Mode_IN_FLOATING;
	GPIO_Init(AT32_PRINT_UARTRX_GPIO, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate      = UART1_BAUD;	
	USART_InitStructure.USART_WordLength    = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits      = USART_StopBits_1;
	USART_InitStructure.USART_Parity        = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode          = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(AT32_PRINT_UART, &USART_InitStructure);

	USART_INTConfig(AT32_PRINT_UART, USART_INT_RDNE, ENABLE);	/* enable receive data not empty interrupt */
	USART_Cmd(AT32_PRINT_UART, ENABLE);		/* enable */

	USART_ClearFlag(AT32_PRINT_UART, USART_FLAG_TRAC);     /* clear Transmission Complete flag */
#endif
}

/**
  * @brief  Uart interrupt config
  * @param  None
  * @retval None
  */
static void ConfigUartNVIC(void)
{
	NVIC_InitType NVIC_InitStructure;

#if UART1_FIFO_EN == 1
    NVIC_InitStructure.NVIC_IRQChannel                      = USARTx_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority           = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif
}

/**
  * @brief  Uart interrupt config
  * @param  _pUart:Uart to use
  * @param  _ucaBuf:buffer contain data that will be sent
  * @param  _usLen:length of sent data
  * @retval None
  */
static void UartSend(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen)
{
	uint16_t i;

	for (i = 0; i < _usLen; i++)
	{
		while (1)//wait forever until tx buffer has sapce
		{
			__IO uint16_t usCount;

			DISABLE_INT();
			usCount = _pUart->usTxCount;
			ENABLE_INT();

			if (usCount < _pUart->usTxBufSize)
			{
				break;
			}
		}

		/* copy data to tx buffer*/
		_pUart->pTxBuf[_pUart->usTxWrite] = _ucaBuf[i];

		DISABLE_INT();
		if (++_pUart->usTxWrite >= _pUart->usTxBufSize)//the tx write point return to the start while reach to the bottom
		{
			_pUart->usTxWrite = 0;
		}
		_pUart->usTxCount++;
		ENABLE_INT();
	}

	USART_INTConfig(_pUart->uart, USART_INT_TDE, ENABLE);//enable transmit empty interrupt
}

/**
  * @brief  Uart interrupt config
  * @param  _pUart:Uart to use
  * @param  _pByte:buffer to receive data
  * @retval 0:No data received
            1:Data read complete
  */
static uint8_t UartGetChar(UART_T *_pUart, uint8_t *_pByte)
{
	uint16_t usCount;

	DISABLE_INT();
	usCount = _pUart->usRxCount;
	ENABLE_INT();

	
	if (usCount == 0)	
	{
		return 0;
	}
	else
	{
		*_pByte = _pUart->pRxBuf[_pUart->usRxRead];		
		
		DISABLE_INT();
		if (++_pUart->usRxRead >= _pUart->usRxBufSize)
		{
			_pUart->usRxRead = 0;
		}
		_pUart->usRxCount--;
		ENABLE_INT();
		return 1;
	}
}

/**
  * @brief  Uart interrupt config
  * @param  _pUart:Uart to use
  * @retval None
  */
static void UartIRQ(UART_T *_pUart)
{
	//receive data process
	if (USART_GetITStatus(_pUart->uart, USART_INT_RDNE) != RESET)
	{
		uint8_t ch;

		ch = USART_ReceiveData(_pUart->uart);
		_pUart->pRxBuf[_pUart->usRxWrite] = ch;
		if (++_pUart->usRxWrite >= _pUart->usRxBufSize)
		{
			_pUart->usRxWrite = 0;
		}
		if (_pUart->usRxCount < _pUart->usRxBufSize)
		{
			_pUart->usRxCount++;
		}

		//if (_pUart->usRxWrite == _pUart->usRxRead)
		//if (_pUart->usRxCount == 1)
		{
			if (_pUart->ReciveNew)
			{
				_pUart->ReciveNew(ch);
			}
		}
	}

    //transmit data process
	if (USART_GetITStatus(_pUart->uart, USART_INT_TDE) != RESET)
	{
		//if (_pUart->usTxRead == _pUart->usTxWrite)
		if (_pUart->usTxCount == 0)
		{
			USART_INTConfig(_pUart->uart, USART_INT_TDE, DISABLE);
			USART_INTConfig(_pUart->uart, USART_INT_TRAC, ENABLE);
		}
		else
		{
			USART_SendData(_pUart->uart, _pUart->pTxBuf[_pUart->usTxRead]);
			if (++_pUart->usTxRead >= _pUart->usTxBufSize)
			{
				_pUart->usTxRead = 0;
			}
			_pUart->usTxCount--;
		}

	}
	else if (USART_GetITStatus(_pUart->uart, USART_INT_TRAC) != RESET)
	{
		//if (_pUart->usTxRead == _pUart->usTxWrite)
		if (_pUart->usTxCount == 0)
		{
		
			USART_INTConfig(_pUart->uart, USART_INT_TRAC, DISABLE);

			if (_pUart->SendOver)
			{
				_pUart->SendOver();
			}
		}
		else
		{
			USART_SendData(_pUart->uart, _pUart->pTxBuf[_pUart->usTxRead]);
			if (++_pUart->usTxRead >= _pUart->usTxBufSize)
			{
				_pUart->usTxRead = 0;
			}
			_pUart->usTxCount--;
		}
	}
}

/**
 * @brief   This function handles USART1 exception
 * @param   None
 * @retval  None
 */

#if UART1_FIFO_EN == 1
void USARTx_IRQ_Handler(void)
{
	UartIRQ(&g_tUart1);
}
#endif

int fputc(int ch, FILE *f)
{
#if 0	
	comSendChar(COM1, ch);

	return ch;
#else	
	USART_SendData(AT32_PRINT_UART, (uint8_t) ch);

	while (USART_GetFlagStatus(AT32_PRINT_UART, USART_FLAG_TRAC) == RESET)
	{}

	return ch;
#endif
}

int fgetc(FILE *f)
{
#if 1	
	uint8_t ucData;

	while(comGetChar(COM1, &ucData) == 0);

	return ucData;
#else
	while (USART_GetFlagStatus(AT32_PRINT_UART, USART_FLAG_RXNE) == RESET);

	return (int)USART_ReceiveData(AT32_PRINT_UART);
#endif
}

/**
  * @}
  */ 

/**
  * @}
  */ 
  
