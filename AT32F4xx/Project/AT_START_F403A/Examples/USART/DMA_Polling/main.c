/**
  ******************************************************************************
  * File   : USART/DMA_Polling/main.c 
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

/** @addtogroup USART_DMA_Polling
  * @{
  */ 
  
/* Private typedef -----------------------------------------------------------*/
typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define TxBufferSize1   (countof(TxBuffer1) - 1)
#define TxBufferSize2   (countof(TxBuffer2) - 1)

/* Private macro -------------------------------------------------------------*/
#define countof(a)   (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
USART_InitType USART_InitStructure;
uint8_t TxBuffer1[] = "USART DMA Polling: USART2 -> USART3 using DMA";
uint8_t TxBuffer2[] = "USART DMA Polling: USART3 -> USART2 using DMA";
uint8_t RxBuffer1[TxBufferSize2];
uint8_t RxBuffer2[TxBufferSize1];
volatile TestStatus TransferStatus1 = FAILED;
volatile TestStatus TransferStatus2 = FAILED; 

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void DMA_Configuration(void);
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
  
  /* Configure the DMA */
  DMA_Configuration();

/* USART2 and USART3 configuration ------------------------------------------------------*/
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
  
  /* Configure USART2 */
  USART_Init(USART2, &USART_InitStructure);
  /* Configure USART3 */
  USART_Init(USART3, &USART_InitStructure);

  /* Enable USART2 DMA Rx and TX request */
  USART_DMACmd(USART2, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE);
  /* Enable USART3 DMA Rx and TX request */
  USART_DMACmd(USART3, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE);

  /* Enable USART2 TX DMA1 Channel */
  DMA_ChannelEnable(DMA1_Channel7, ENABLE);
  /* Enable USART2 RX DMA1 Channel */
  DMA_ChannelEnable(DMA1_Channel6, ENABLE);

  /* Enable USART3 TX DMA1 Channel */
  DMA_ChannelEnable(DMA1_Channel2, ENABLE);
  /* Enable USART3 RX DMA1 Channel */
  DMA_ChannelEnable(DMA1_Channel3, ENABLE);

  /* Enable the USART3 */
  USART_Cmd(USART2, ENABLE);
  /* Enable the USART3 */
  USART_Cmd(USART3, ENABLE);

  /* Wait until USART2 TX DMA1 Channel Transfer Complete */
  while (DMA_GetFlagStatus(DMA1_FLAG_TC7) == RESET)
  {
  }
  /* Wait until USART2 RX DMA1 Channel Transfer Complete */
  while (DMA_GetFlagStatus(DMA1_FLAG_TC6) == RESET)
  {
  }

  /* Wait until USART3 TX DMA1 Channel Transfer Complete */
  while (DMA_GetFlagStatus(DMA1_FLAG_TC2) == RESET)
  {
  }
  /* Wait until USART3 RX DMA1 Channel Transfer Complete */
  while (DMA_GetFlagStatus(DMA1_FLAG_TC3) == RESET)
  {
  }

  /* Check the received data with the send ones */
  TransferStatus1 = Buffercmp(TxBuffer2, RxBuffer1, TxBufferSize2);
  /* TransferStatus1 = PASSED, if the data transmitted from USART3 and  
     received by USART2 are the same */
  /* TransferStatus1 = FAILED, if the data transmitted from USART3 and 
     received by USART2 are different */
  TransferStatus2 = Buffercmp(TxBuffer1, RxBuffer2, TxBufferSize1);
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
  /* DMA clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA1, ENABLE);

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
  * @brief  Configures the DMA.
  * @param  None
  * @retval None
  */
void DMA_Configuration(void)
{
  DMA_InitType DMA_InitStructure;

  /* USART2 TX DMA1 Channel (triggered by USART2 Tx event) Config */
  DMA_Reset(DMA1_Channel7);  
  DMA_DefaultInitParaConfig(&DMA_InitStructure);
  DMA_InitStructure.DMA_PeripheralBaseAddr = 0x40004404;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)TxBuffer1;
  DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALDST;
  DMA_InitStructure.DMA_BufferSize = TxBufferSize1;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;
  DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE;
  DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_BYTE;
  DMA_InitStructure.DMA_MemoryDataWidth = DMA_MEMORYDATAWIDTH_BYTE;
  DMA_InitStructure.DMA_Mode = DMA_MODE_NORMAL;
  DMA_InitStructure.DMA_Priority = DMA_PRIORITY_VERYHIGH;
  DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;
  DMA_Init(DMA1_Channel7, &DMA_InitStructure);

  /* USART2 RX DMA1 Channel (triggered by USART2 Rx event) Config */
  DMA_Reset(DMA1_Channel6);  
  DMA_InitStructure.DMA_PeripheralBaseAddr = 0x40004404;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RxBuffer1;
  DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALSRC;
  DMA_InitStructure.DMA_BufferSize = TxBufferSize2;
  DMA_Init(DMA1_Channel6, &DMA_InitStructure);
  
  /* USART3 TX DMA1 Channel (triggered by USART3 Tx event) Config */
  DMA_Reset(DMA1_Channel2);  
  DMA_InitStructure.DMA_PeripheralBaseAddr = 0x40004804;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)TxBuffer2;
  DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALDST;
  DMA_InitStructure.DMA_BufferSize = TxBufferSize2;  
  DMA_Init(DMA1_Channel2, &DMA_InitStructure);
  
  /* USART3 RX DMA1 Channel (triggered by USART3 Rx event) Config */
  DMA_Reset(DMA1_Channel3);  
  DMA_InitStructure.DMA_PeripheralBaseAddr = 0x40004804;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RxBuffer2;
  DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALSRC;
  DMA_InitStructure.DMA_BufferSize = TxBufferSize1;
  DMA_Init(DMA1_Channel3, &DMA_InitStructure);  
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


