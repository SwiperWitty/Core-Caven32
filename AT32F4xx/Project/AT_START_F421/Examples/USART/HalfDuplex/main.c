/**
  ******************************************************************************
  * File   : USART/HalfDuplex/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "at32_board.h"

/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */

/** @addtogroup USART_HalfDuplex
  * @{
  */ 
  
/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define TxBufferSize1   (countof(TxBuffer1) - 1)
#define TxBufferSize2   (countof(TxBuffer2) - 1)

/* Private macro -------------------------------------------------------------*/
#define countof(a)   (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
USART_InitType USART_InitStructure;
uint8_t TxBuffer1[] = "USART Half Duplex: USART1 -> USART2 using HalfDuplex mode";
uint8_t TxBuffer2[] = "USART Half Duplex: USART2 -> USART1 using HalfDuplex mode";
uint8_t RxBuffer1[TxBufferSize2];
uint8_t RxBuffer2[TxBufferSize1];
uint32_t NbrOfDataToRead1 = TxBufferSize2;
uint32_t NbrOfDataToRead2 = TxBufferSize1;
uint8_t TxCounter1 = 0, RxCounter1 = 0;
uint8_t TxCounter2 = 0, RxCounter2 = 0;
volatile TestStatus TransferStatus1 = FAILED, TransferStatus2 = FAILED;  

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);
  
/* Private functions ---------------------------------------------------------*/

/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
int main(void)
{       
  /* System Clocks Configuration */
  RCC_Configuration();

  /* Configure the GPIO ports */
  GPIO_Configuration();

  /* Initialize Leds mounted on board */
  AT32_Board_Init();
  
/* USART2 and USART3 configuration -------------------------------------------*/
  /* USART2 and USART3 configured as follow:
        - BaudRate = 230400 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled

  */
  USART_StructInit(&USART_InitStructure);
  USART_InitStructure.USART_BaudRate = 230400;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  /* Configure USART1 */
  USART_Init(USART1, &USART_InitStructure);
  /* Configure USART2 */
  USART_Init(USART2, &USART_InitStructure);
  
  /* Enable the USART1 */
  USART_Cmd(USART1, ENABLE);
  /* Enable the USART2 */
  USART_Cmd(USART2, ENABLE);

  /* Enable USART1 Half Duplex Mode*/
  USART_HalfDuplexCmd(USART1, ENABLE);
  /* Enable USART2 Half Duplex Mode*/
  USART_HalfDuplexCmd(USART2, ENABLE);  

  while(NbrOfDataToRead2--)
  {
    /* Wait until end of transmit */
    while(USART_GetFlagStatus(USART1, USART_FLAG_TDE) == RESET)
    {
    }
    /* Write one byte in the USART2 Transmit Data Register */
    USART_SendData(USART1, TxBuffer1[TxCounter1++]);

    /* Wait the byte is entirely received by USART3 */  
    while(USART_GetFlagStatus(USART2, USART_FLAG_RDNE) == RESET)
    {
    }
    /* Store the received byte in the RxBuffer2 */
    RxBuffer2[RxCounter2++] = USART_ReceiveData(USART2);
  }

  /* Clear the USART1 Data Register */
  USART_ReceiveData(USART1);

  while(NbrOfDataToRead1--)
  { 
    /* Wait until end of transmit */
    while(USART_GetFlagStatus(USART2, USART_FLAG_TDE)== RESET)
    {
    }
    /* Write one byte in the USART2 Transmit Data Register */
    USART_SendData(USART2, TxBuffer2[TxCounter2++]);

    /* Wait the byte is entirely received by USART1 */
    while(USART_GetFlagStatus(USART1,USART_FLAG_RDNE) == RESET)
    {
    }
    /* Store the received byte in the RxBuffer1 */
    RxBuffer1[RxCounter1++] = USART_ReceiveData(USART1);
  }
  
  /* Check the received data with the send ones */
  TransferStatus1 = Buffercmp(TxBuffer1, RxBuffer2, TxBufferSize1);
  /* TransferStatus = PASSED, if the data transmitted from USARTy and  
     received by USARTz are the same */
  /* TransferStatus = FAILED, if the data transmitted from USARTy and 
     received by USARTz are different */
  TransferStatus2 = Buffercmp(TxBuffer2, RxBuffer1, TxBufferSize2);
  /* TransferStatus = PASSED, if the data transmitted from USARTz and  
     received by USARTy are the same */
  /* TransferStatus = FAILED, if the data transmitted from USARTz and 
     received by USARTy are different */

  /* Judge whether the result is correct */
  if((TransferStatus1 == PASSED)&&(TransferStatus2 == PASSED))
  {
    /*Trun on the LED*/
    AT32_LEDn_ON(LED2);
    AT32_LEDn_ON(LED3);
    AT32_LEDn_ON(LED4);

  }
  while (1)
  {
  }
}

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{
  /* Enable GPIO clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA, ENABLE);

  /* Enable USART1 Clock */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_USART1, ENABLE); 

  /* Enable USART2 Clock */
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_USART2, ENABLE);  
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure;
  
  /* Connect PXx to USART1_Tx */
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource9, GPIO_AF_1);
  /* Connect PXx to USART2_Tx */
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource2, GPIO_AF_1);

  /* Configure USART1 Tx as alternate function open-drain */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_9;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OutType = GPIO_OutType_PP;
  GPIO_InitStructure.GPIO_Pull = GPIO_Pull_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure USART2 Tx as alternate function open-drain */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_2;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer1 identical to pBuffer2
  *         FAILED: pBuffer1 differs from pBuffer2
  */
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }
    
    pBuffer1++;
    pBuffer2++;
  }

  return PASSED;  
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}

#endif

/**
  * @}
  */ 

/**
  * @}
  */ 




