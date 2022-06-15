/**
  ******************************************************************************
  * File   : USART/Polling/main.c 
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

/** @addtogroup USART_Polling
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define TxBufferSize   (countof(TxBuffer))

/* Private macro -------------------------------------------------------------*/
#define countof(a)   (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
USART_InitType USART_InitStructure;
uint8_t TxBuffer[] = "Buffer Send from USART1 to USART2 using Flags";
uint8_t RxBuffer[TxBufferSize];
__IO uint8_t TxCounter = 0, RxCounter = 0;  
volatile TestStatus TransferStatus = FAILED;  

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
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
  
  /* USART1 and USART2 configuration ------------------------------------------------------*/
  /* USART1 and USART2 configured as follow:
        - BaudRate = 230400 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - Even parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_StructInit(&USART_InitStructure);
  USART_InitStructure.USART_BaudRate = 230400;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_Even;
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

  while(TxCounter < TxBufferSize)
  {   
    /* Send one byte from USART1 to USART2 */
    USART_SendData(USART1, TxBuffer[TxCounter++]);
    
    /* Loop until USART1 DR register is empty */ 
    while(USART_GetFlagStatus(USART1, USART_FLAG_TDE) == RESET)
    {
    }
    
    /* Loop until the USART2 Receive Data Register is not empty */
    while(USART_GetFlagStatus(USART2, USART_FLAG_RDNE) == RESET)
    {
    }

    /* Store the received byte in RxBuffer */
    RxBuffer[RxCounter++] = (USART_ReceiveData(USART2) & 0x7F);  
    
  } 
  /* Check the received data with the send ones */
  TransferStatus = Buffercmp(TxBuffer, RxBuffer, TxBufferSize);
  /* TransferStatus = PASSED, if the data transmitted from USART1 and  
     received by USART2 are the same */
  /* TransferStatus = FAILED, if the data transmitted from USART1 and 
     received by USART2 are different */
  
  /* Judge whether the result is correct */
  if(TransferStatus == PASSED)
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
  /* Connect PXx to USART1_Rx */
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource10, GPIO_AF_1);
  /* Connect PXx to USART2_Tx */
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource2, GPIO_AF_1);
  /* Connect PXx to USART2_Rx */
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource3, GPIO_AF_1);
  
  /* Configure USART1 Rx as input floating */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_9 | GPIO_Pins_10 | GPIO_Pins_2 | GPIO_Pins_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_OutType = GPIO_OutType_PP;
  GPIO_InitStructure.GPIO_Pull = GPIO_Pull_NOPULL;
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



