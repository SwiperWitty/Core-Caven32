/**
  ******************************************************************************
  * File   : USART/HyperTerminal_HwFlowControl/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 
  
/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "at32_board.h"

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup USART_HyperTerminal_HwFlowControl
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TxBufferSize   (countof(TxBuffer) - 1)
#define RxBufferSize   0xFF

/* Private macro -------------------------------------------------------------*/
#define countof(a)   (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
USART_InitType USART_InitStructure;
uint8_t TxBuffer[] = "\n\rUSART Hyperterminal Hardware Flow Control Example: USART - \
Hyperterminal communication using hardware flow control\n\r";
uint8_t RxBuffer[RxBufferSize];
uint8_t NbrOfDataToTransfer = TxBufferSize;
uint8_t TxCounter = 0;
uint8_t RxCounter = 0;

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
  
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
  
  /* USART2 configuration ------------------------------------------------------*/
  /* USART2 configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control enabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_StructInit(&USART_InitStructure);
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART2, &USART_InitStructure);
  /* Enable the USART2 */
  USART_Cmd(USART2, ENABLE);

/* Communication hyperterminal-USART2 using hardware flow control -------------*/
  /* Send a buffer from USART to hyperterminal */  
  while(NbrOfDataToTransfer--)
  {
    USART_SendData(USART2, TxBuffer[TxCounter++]);
    while(USART_GetFlagStatus(USART2, USART_FLAG_TDE) == RESET);          
  }
  
  /* Receive a string (Max RxBufferSize bytes) from the Hyperterminal ended by '\r' (Enter key) */
  do
  { 
    if((USART_GetFlagStatus(USART2, USART_FLAG_RDNE) != RESET)&&(RxCounter < RxBufferSize))
    {
       RxBuffer[RxCounter] = USART_ReceiveData(USART2);
       USART_SendData(USART2, RxBuffer[RxCounter++]);
    }   
 
  }while((RxBuffer[RxCounter - 1] != '\r')&&(RxCounter != RxBufferSize));

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
  /* Enable GPIOx and AFIO clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA | RCC_APB2PERIPH_AFIO, ENABLE);

  /* Enable USART2 clocks */
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

  /* Configure USART2 RTS and USART2 Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_1 | GPIO_Pins_2;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure USART2 CTS and USART2 Rx as input floating */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_0 | GPIO_Pins_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
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


