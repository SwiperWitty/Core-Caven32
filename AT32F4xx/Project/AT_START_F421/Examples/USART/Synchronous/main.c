/**
  ******************************************************************************
  * File   : USART/Synchronous/main.c 
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

/** @addtogroup USART_Synchronous
  * @{
  */ 
/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define TxBufferSize1   (countof(TxBuffer1) - 1)
#define TxBufferSize2   (countof(TxBuffer2) - 1)
#define DYMMY_BYTE      0x00

/* Private macro -------------------------------------------------------------*/
#define countof(a)   (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
USART_InitType USART_InitStructure;
USART_ClockInitType USART_ClockInitStructure; 

uint8_t TxBuffer1[] = "USART Synchronous Example: USART1 -> SPI1 using TXE and RXNE Flags";
uint8_t TxBuffer2[] = "USART Synchronous Example: SPI1 -> USART1 using TXE and RXNE Flags";
uint8_t RxBuffer1[TxBufferSize2];
uint8_t RxBuffer2[TxBufferSize1];
__IO uint8_t NbrOfDataToRead1 = TxBufferSize2;
__IO uint8_t NbrOfDataToRead2 = TxBufferSize1;
__IO uint8_t TxCounter1 = 0, RxCounter1 = 0;
__IO uint8_t TxCounter2 = 0, RxCounter2 = 0;
volatile TestStatus TransferStatus1 = FAILED, TransferStatus2 = FAILED; 

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void SPI_Configuration(void);
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
  
  /* Initialize Leds mounted on AT_START-EVAL board */
  AT32_Board_Init();

 /* USART1 configuration ------------------------------------------------------*/
  /* USART1 configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - USART Clock Enabled
        - USART CPOL: Clock is active High
        - USART CPHA: Data is captured on the second edge 
        - USART LastBit: The clock pulse of the last data bit is output to 
                         the SCLK pin
  */
  USART_ClockStructInit(&USART_ClockInitStructure);
  USART_ClockInitStructure.USART_Clock = USART_Clock_Enable;
  USART_ClockInitStructure.USART_CPOL = USART_CPOL_High;
  USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
  USART_ClockInitStructure.USART_LastBit = USART_LastBit_Enable;
  USART_ClockInit(USART1, &USART_ClockInitStructure);

  USART_StructInit(&USART_InitStructure);
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1, &USART_InitStructure);
  
  /* Configure the USART1 */
  USART_Init(USART1, &USART_InitStructure);

  /* Enable the USART1 */
  USART_Cmd(USART1, ENABLE);

  /* Configure the SPI */
  SPI_Configuration();
  
  while(NbrOfDataToRead2--)
  {
    /* Write one byte in the USART1 Transmit Data Register */
    USART_SendData(USART1, TxBuffer1[TxCounter1++]);
    /* Wait until end of transmit */
    while(USART_GetFlagStatus(USART1, USART_FLAG_TRAC) == RESET)
    {
    }
    /* Wait the byte is entirely received by SPI1 */  
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RNE) == RESET)
    {
    }
    /* Store the received byte in the RxBuffer2 */
    RxBuffer2[RxCounter2++] = SPI_I2S_RxData(SPI1);
  }

  /* Clear the USART1 Data Register */
  USART_ReceiveData(USART1);

  while(NbrOfDataToRead1--)
  {
    /* Wait until end of transmit */
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TE)== RESET)
    {
    }
    /* Write one byte in the SPI1 Transmit Data Register */
    SPI_I2S_TxData(SPI1, TxBuffer2[TxCounter2++]);

    /* Send a Dummy byte to generate clock to slave */ 
    USART_SendData(USART1, DYMMY_BYTE);
    /* Wait until end of transmit */
    while(USART_GetFlagStatus(USART1, USART_FLAG_TRAC) == RESET)
    {
    }
    /* Wait the byte is entirely received by USART1 */
    while(USART_GetFlagStatus(USART1, USART_FLAG_RDNE) == RESET)
    {
    }
    /* Store the received byte in the RxBuffer1 */
    RxBuffer1[RxCounter1++] = USART_ReceiveData(USART1);
  }
  
  /* Check the received data with the send ones */
  TransferStatus1 = Buffercmp(TxBuffer1, RxBuffer2, TxBufferSize1);
  /* TransferStatus = PASSED, if the data transmitted from USART1 and  
     received by SPI1 are the same */
  /* TransferStatus = FAILED, if the data transmitted from USART1 and 
     received by SPI1 are different */
  TransferStatus2 = Buffercmp(TxBuffer2, RxBuffer1, TxBufferSize2);
  /* TransferStatus = PASSED, if the data transmitted from SPI1 and  
     received by USART1 are the same */
  /* TransferStatus = FAILED, if the data transmitted from SPI1 and 
     received by USART1 are different */
  
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
  /* Enable SPI1 Clock */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_SPI1, ENABLE);    
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure;

  /* Connect PXx to USART1_Clk */
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource8, GPIO_AF_1);
  /* Connect PXx to USART1_Tx */
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource9, GPIO_AF_1);
  /* Connect PXx to USART1_Rx */
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource10, GPIO_AF_1);
  
  /* Configure USART1 TX and USART1 CK pins as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_8 | GPIO_Pins_9 | GPIO_Pins_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_OutType = GPIO_OutType_PP;
  GPIO_InitStructure.GPIO_Pull = GPIO_Pull_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Connect PXx to SPI1_SCK */
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource5, GPIO_AF_0);
  /* Connect PXx to SPI1_MISO */
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource6, GPIO_AF_0);
  /* Connect PXx to SPI1_MOSI */
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource7, GPIO_AF_0);
  
  /* Configure SPI1 pins: MISO and MOSI */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_5 | GPIO_Pins_6 | GPIO_Pins_7;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
}

/**
  * @brief  Configures the SPI.
  * @param  None
  * @retval None
  */
void SPI_Configuration(void)
{
  SPI_InitType SPI_InitStructure;

  SPI_DefaultInitParaConfig(&SPI_InitStructure);

  SPI_I2S_Reset(SPI1);

  SPI_DefaultInitParaConfig(&SPI_InitStructure);
  /* SPI1 Config */
  SPI_InitStructure.SPI_TransMode = SPI_TRANSMODE_FULLDUPLEX;
  SPI_InitStructure.SPI_Mode = SPI_MODE_SLAVE;
  SPI_InitStructure.SPI_FrameSize = SPI_FRAMESIZE_8BIT;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_HIGH;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2EDGE;
  SPI_InitStructure.SPI_NSSSEL = SPI_NSSSEL_SOFT;
  SPI_InitStructure.SPI_FirstBit = SPI_FIRSTBIT_LSB;
  
  /* Configure SPI1 */
  SPI_Init(SPI1, &SPI_InitStructure);

  /* SPI1 enable */
  SPI_Enable(SPI1, ENABLE);
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

