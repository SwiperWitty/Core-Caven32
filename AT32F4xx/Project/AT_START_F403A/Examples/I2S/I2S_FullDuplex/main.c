/**
  **************************************************************************
  * File   : main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  **************************************************************************
  */
 

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "at32_board.h"

/** @addtogroup AT32F403A_StdPeriph_Examples
  * @{
  */

/** @addtogroup I2S_FullDuplex
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define TXBUF_SIZE   32
#define RXBUF_SIZE   TXBUF_SIZE

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
I2S_InitType I2S_InitStructure;
DMA_InitType DMA_InitStructure;

uint16_t I2S2_Buffer_Tx[TXBUF_SIZE];
uint16_t I2S2_Buffer_Rx[RXBUF_SIZE];

uint16_t I2S3_Buffer_Tx[TXBUF_SIZE];
uint16_t I2S3_Buffer_Rx[RXBUF_SIZE];

TestStatus TransferStatus2 = FAILED ;
TestStatus TransferStatus3 = FAILED ;

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
TestStatus Buffercmp(uint16_t* pBuffer1, uint16_t* pBuffer2, uint16_t BufferLength);

/* Private functions ---------------------------------------------------------*/
void TxDataInit(void)
{
  uint32_t i;
  for(i=0; i<TXBUF_SIZE; i++)
  {
    I2S2_Buffer_Tx[i] = i;
    I2S3_Buffer_Tx[i] = TXBUF_SIZE - i;
  }
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured,
       this is done through SystemInit() function which is called from startup
       file (startup_at32f4xxxx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_at32f4xx.c file
     */

  TxDataInit();

  /* System clocks configuration ---------------------------------------------*/
  RCC_Configuration();

  /* GPIO configuration ------------------------------------------------------*/
  GPIO_Configuration();

  /* Config LED */
  AT32_Board_Init();

  /* Reset I2S */  
  SPI_I2S_Reset(SPI2);
  SPI_I2S_Reset(SPI3);

  /* I2S peripheral configuration */
  I2S_DefaultInit(&I2S_InitStructure);
  I2S_InitStructure.I2s_AudioProtocol = I2S_AUDIOPROTOCOL_PHILLIPS;
  I2S_InitStructure.I2S_FrameFormat = I2S_FRAMEFORMAT_DL16BIT_CHL32BIT;
  I2S_InitStructure.I2S_MCLKOE = I2S_MCLKOE_DISABLE;
  I2S_InitStructure.I2S_AudioFreq = I2S_AUDIOFREQ_48K;
  I2S_InitStructure.I2S_CPOL = I2S_CPOL_LOW;

  /* I2S2 Master, I2S3 Slave */
  /* I2S2 configuration */
  I2S_InitStructure.I2S_Mode = I2S_MODE_MASTERTX;
  I2S_Init(SPI2, &I2S_InitStructure);
  I2S_InitStructure.I2S_Mode = I2S_MODE_SLAVERX;
  I2S_Init(I2S2EXT, &I2S_InitStructure);

  /* I2S3 configuration */
  I2S_InitStructure.I2S_Mode = I2S_MODE_SLAVERX;
  I2S_Init(SPI3, &I2S_InitStructure);
  I2S_InitStructure.I2S_Mode = I2S_MODE_SLAVETX;
  I2S_Init(I2S3EXT, &I2S_InitStructure);

  /*DMA1_channel1 i2s2 tx */
  /*DMA1_channel2 i2s2ext rx */
  /*DMA1_channel3 i2s3 rx */
  /*DMA1_channel4 i2s3ext tx */

  /* Reset DMA */
  DMA_Reset(DMA1_Channel1);
  DMA_Reset(DMA1_Channel2);
  DMA_Reset(DMA1_Channel3);
  DMA_Reset(DMA1_Channel4);

  /* Config DMA flexible for i2s2 and i2s3 with full duplex */
  DMA_Flexible_Config(DMA1, Flex_Channel1, DMA_FLEXIBLE_SPI2_TX);
  DMA_Flexible_Config(DMA1, Flex_Channel2, DMA_FLEXIBLE_I2S2EXT_RX);
  DMA_Flexible_Config(DMA1, Flex_Channel3, DMA_FLEXIBLE_SPI3_RX);
  DMA_Flexible_Config(DMA1, Flex_Channel4, DMA_FLEXIBLE_I2S3EXT_TX);

  /* DMA configuration */
  DMA_DefaultInitParaConfig(&DMA_InitStructure);
  DMA_InitStructure.DMA_BufferSize = TXBUF_SIZE;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;
  DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE;
  DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_HALFWORD;
  DMA_InitStructure.DMA_MemoryDataWidth = DMA_MEMORYDATAWIDTH_HALFWORD;
  DMA_InitStructure.DMA_Mode = DMA_MODE_NORMAL;
  DMA_InitStructure.DMA_Priority = DMA_PRIORITY_HIGH;
  DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;

  /* DMA1 channel1 configuration */
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(SPI2->DT);
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&I2S2_Buffer_Tx;
  DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALDST;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);

  /* DMA1 channel2 configuration */ 
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(I2S2EXT->DT);
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&I2S2_Buffer_Rx;
  DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALSRC;
  DMA_Init(DMA1_Channel2, &DMA_InitStructure);

  /* DMA1 channel3 configuration */
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(SPI3->DT);
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&I2S3_Buffer_Rx;
  DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALSRC;
  DMA_Init(DMA1_Channel3, &DMA_InitStructure);
  
  /* DMA1 channel4 configuration */ 
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(I2S3EXT->DT);
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&I2S3_Buffer_Tx;
  DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALDST;  
  DMA_Init(DMA1_Channel4, &DMA_InitStructure); 

  /* Enable DMA1 Channel1 */
  DMA_ChannelEnable(DMA1_Channel1, ENABLE);
  /* Enable DMA1 Channel2 */
  DMA_ChannelEnable(DMA1_Channel2, ENABLE);
  /* Enable DMA1 Channel3 */
  DMA_ChannelEnable(DMA1_Channel3, ENABLE);
  /* Enable DMA1 Channel4 */
  DMA_ChannelEnable(DMA1_Channel4, ENABLE);

  /* Enable I2S2 DMA request source */
  SPI_I2S_DMAEnable(SPI2, SPI_I2S_DMA_TX, ENABLE);
  /* Enable I2S2Ext DMA request source */
  SPI_I2S_DMAEnable(I2S2EXT, SPI_I2S_DMA_RX, ENABLE);
  /* Enable I2S3 DMA request source */
  SPI_I2S_DMAEnable(SPI3, SPI_I2S_DMA_RX, ENABLE);
  /* Enable I2S3Ext DMA request source */
  SPI_I2S_DMAEnable(I2S3EXT, SPI_I2S_DMA_TX, ENABLE);

  /* Enable the I2S3 */
  I2S_Enable(SPI3, ENABLE);
  /* Enable the I2S3EXT */
  I2S_Enable(I2S3EXT, ENABLE);
  /* Enable the I2S2EXT */
  I2S_Enable(I2S2EXT, ENABLE);
  /* Enable the I2S2 */
  I2S_Enable(SPI2, ENABLE);

  /* Wait for DMA transmiting complete */
  while(DMA_GetFlagStatus(DMA1_FLAG_TC2)==RESET);

  /* TransferStatus = PASSED, if the data transmitted and received are the same
     TransferStatus = FAILED, if the data transmitted and received are different */
  TransferStatus2 = Buffercmp((uint16_t*)I2S2_Buffer_Tx, (uint16_t*)I2S3_Buffer_Rx, TXBUF_SIZE);

  TransferStatus3 = Buffercmp((uint16_t*)I2S3_Buffer_Tx, (uint16_t*)I2S2_Buffer_Rx, TXBUF_SIZE);

  /* if passed ,LED3 lights */
  if(TransferStatus2==PASSED && TransferStatus3==PASSED)
  {
    AT32_LEDn_ON(LED3);
  }    
  else
  {
    AT32_LEDn_OFF(LED3);
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
  /* Enable peripheral clocks ------------------------------------------------*/
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA1, ENABLE);  
  
  /* GPIOA, GPIOB GPIOC and AFIO clocks enable */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA | RCC_APB2PERIPH_GPIOB | RCC_APB2PERIPH_GPIOC |
                         RCC_APB2PERIPH_AFIO, ENABLE);

  /* SPI2 and SPI3 clocks enable */
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_SPI2 | RCC_APB1PERIPH_SPI3, ENABLE);
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure;

  GPIO_PinsRemapConfig(AFIO_MAP5_SPI3_0001, ENABLE);

  /* Configure I2S2 pins */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_12 | GPIO_Pins_13 | GPIO_Pins_14 | GPIO_Pins_15;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Configure I2S3 pins */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure I2S3 pins */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_10 | GPIO_Pins_11 | GPIO_Pins_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer1 identical to pBuffer2
  *         FAILED: pBuffer1 differs from pBuffer2
  */
TestStatus Buffercmp(uint16_t* pBuffer1, uint16_t* pBuffer2, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
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
  
