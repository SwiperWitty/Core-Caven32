/**
  ******************************************************************************
  * File   : USART/Interrupt/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 
  
/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "at32_board.h"

/** @addtogroup AT32F403A_StdPeriph_Examples
  * @{
  */

/** @addtogroup USART_Interrupt
  * @{
  */ 
  
/* Private typedef -----------------------------------------------------------*/
typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define TxBufferSize1   (countof(TxBuffer1) - 1)
#define TxBufferSize2   (countof(TxBuffer2) - 1)
#define RxBufferSize1   TxBufferSize2
#define RxBufferSize2   TxBufferSize1

/* Private macro -------------------------------------------------------------*/
#define countof(a)   (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
USART_InitType USART_InitStructure;
uint8_t TxBuffer1[] = "USART Interrupt Example: USART2 -> USART3 using Interrupt";
uint8_t TxBuffer2[] = "USART Interrupt Example: USART3 -> USART2 using Interrupt";
uint8_t RxBuffer1[RxBufferSize1];
uint8_t RxBuffer2[RxBufferSize2];
__IO uint8_t TxCounter1 = 0x00;
__IO uint8_t TxCounter2 = 0x00;
__IO uint8_t RxCounter1 = 0x00; 
__IO uint8_t RxCounter2 = 0x00;
uint8_t NbrOfDataToTransfer1 = TxBufferSize1;
uint8_t NbrOfDataToTransfer2 = TxBufferSize2;
uint8_t NbrOfDataToRead1 = RxBufferSize1;
uint8_t NbrOfDataToRead2 = RxBufferSize2;
__IO TestStatus TransferStatus1 = FAILED; 
__IO TestStatus TransferStatus2 = FAILED;

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
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
       
  /* NVIC configuration */
  NVIC_Configuration();

  /* Configure the GPIO ports */
  GPIO_Configuration();

  /* Initialize Leds mounted on board */
  AT32_Board_Init();
  
  /* USART2 and USART3 configuration ------------------------------------------------------*/
  /* USART2 and USART3 configured as follow:
        - BaudRate = 9600 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_StructInit(&USART_InitStructure);
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  /* Configure USART2 */
  USART_Init(USART2, &USART_InitStructure);
  /* Configure USART3 */
  USART_Init(USART3, &USART_InitStructure);
  
  /* Enable USART2 Receive and Transmit interrupts */
  USART_INTConfig(USART2, USART_INT_RDNE, ENABLE);
  USART_INTConfig(USART2, USART_INT_TDE, ENABLE);

  /* Enable USART3 Receive and Transmit interrupts */
  USART_INTConfig(USART3, USART_INT_RDNE, ENABLE);
  USART_INTConfig(USART3, USART_INT_TDE, ENABLE);

  /* Enable the USART2 */
  USART_Cmd(USART2, ENABLE);
  /* Enable the USART3 */
  USART_Cmd(USART3, ENABLE);

  /* Wait until end of transmission from USART2 to USART3 */
  while(RxCounter2 < RxBufferSize2)
  {
  }

  /* Wait until end of transmission from USART3 to USART2 */
  while(RxCounter1 < RxBufferSize1)
  {
  }
  
  /* Check the received data with the send ones */
  TransferStatus1 = Buffercmp(TxBuffer2, RxBuffer1, RxBufferSize1);
  /* TransferStatus1 = PASSED, if the data transmitted from USART3 and  
     received by USART2 are the same */
  /* TransferStatus1 = FAILED, if the data transmitted from USART3 and 
     received by USART2 are different */
  TransferStatus2 = Buffercmp(TxBuffer1, RxBuffer2, RxBufferSize2);
  /* TransferStatus2 = PASSED, if the data transmitted from USART2 and  
     received by USART3 are the same */
  /* TransferStatus2 = FAILED, if the data transmitted from USART2 and 
     received by USART3 are different */

  /* Judge whether the result is correct */
  if((TransferStatus1 == PASSED)&&(TransferStatus2 == PASSED))
  {
    /*Trun on the LED*/
    AT32_LEDn_ON(LED2);
    AT32_LEDn_ON(LED3);
    AT32_LEDn_ON(LED4);
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
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOD | RCC_APB2PERIPH_GPIOC | RCC_APB2PERIPH_AFIO, ENABLE);

  /* Enable USART2 Clock */
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_USART2, ENABLE); 

  /* Enable USART3 Clock */
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_USART3, ENABLE);  
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure;

  /* Enable the USART3 Pins Software Remapping */
  GPIO_PinsRemapConfig(GPIO_PartialRemap_USART3, ENABLE);
  
  /* Enable the USART2 Pins Software Remapping */
  GPIO_PinsRemapConfig(GPIO_Remap_USART2, ENABLE);  


  /* Configure USART2 Rx as input floating */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /* Configure USART3 Rx as input floating */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_11;
  GPIO_Init(GPIOC, &GPIO_InitStructure);  
  
  /* Configure USART2 Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_5;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* Configure USART3 Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_10;
  GPIO_Init(GPIOC, &GPIO_InitStructure);  
}
/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
  NVIC_InitType NVIC_InitStructure;

  /* Configure the NVIC Preemption Priority Bits */  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  
  /* Enable the USART2 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the USART3 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
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

