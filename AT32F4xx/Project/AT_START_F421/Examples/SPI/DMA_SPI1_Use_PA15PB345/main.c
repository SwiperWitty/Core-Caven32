/**
  ******************************************************************************
  * File   : SPI/SPI_DMA_SPI1_Use_PA15PB345/main.c 
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

/** @addtogroup SPI_DMA_SPI1_Use_PA15PB345
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define BufferSize       32
#define SPI_MASTER                   SPI1
#define SPI_MASTER_CLK               RCC_APB2PERIPH_SPI1
#define SPI_MASTER_GPIO              GPIOB
#define SPI_MASTER_GPIO_NSS          GPIOA
#define SPI_MASTER_GPIO_CLK          RCC_AHBPERIPH_GPIOB 
#define SPI_MASTER_GPIO_CLK_NSS      RCC_AHBPERIPH_GPIOA 
#define SPI_MASTER_PIN_NSS           GPIO_Pins_15
#define SPI_MASTER_PIN_SCK           GPIO_Pins_3
#define SPI_MASTER_PIN_MISO          GPIO_Pins_4
#define SPI_MASTER_PIN_MOSI          GPIO_Pins_5
#define SPI_SLAVE                    SPI2
#define SPI_SLAVE_CLK                RCC_APB1PERIPH_SPI2
#define SPI_SLAVE_GPIO               GPIOB
#define SPI_SLAVE_GPIO_CLK           RCC_AHBPERIPH_GPIOB
#define SPI_SLAVE_PIN_NSS            GPIO_Pins_12
#define SPI_SLAVE_PIN_SCK            GPIO_Pins_13
#define SPI_SLAVE_PIN_MISO           GPIO_Pins_14
#define SPI_SLAVE_PIN_MOSI           GPIO_Pins_15
#define SPI_SLAVE_DMA                DMA1
#define SPI_SLAVE_DMA_CLK            RCC_AHBPERIPH_DMA1
#define SPI_SLAVE_Rx_DMA_Channel     DMA1_Channel4
#define SPI_SLAVE_Rx_DMA_FLAG        DMA1_FLAG_TC4
#define SPI_SLAVE_Tx_DMA_Channel     DMA1_Channel5
#define SPI_SLAVE_Tx_DMA_FLAG        DMA1_FLAG_TC5  
#define SPI_SLAVE_DR_Base            0x4000380C

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
SPI_InitType  SPI_InitStructure;
DMA_InitType  DMA_InitStructure;
GPIO_InitType GPIO_InitStructure;

uint8_t SPI_MASTER_Buffer_Tx[BufferSize] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                                            0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
                                            0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12,
                                            0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                                            0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E,
                                            0x1F, 0x20};
uint8_t SPI_SLAVE_Buffer_Rx[BufferSize];
__IO uint8_t TxIdx = 0;
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
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_at32f415_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_at32f4xx.c file
     */     
       
  /* System clocks configuration ---------------------------------------------*/
  RCC_Configuration();

  /* GPIO configuration ------------------------------------------------------*/
  GPIO_Configuration();

  /* Config LED */
  AT32_Board_Init();
  
  /* SPI_SLAVE_Rx_DMA_Channel configuration ---------------------------------------------*/
  DMA_Reset(SPI_SLAVE_Rx_DMA_Channel);
  DMA_DefaultInitParaConfig(&DMA_InitStructure);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SPI_SLAVE_DR_Base;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)SPI_SLAVE_Buffer_Rx;
  DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALSRC;
  DMA_InitStructure.DMA_BufferSize = BufferSize;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;
  DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE;
  DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_BYTE;
  DMA_InitStructure.DMA_MemoryDataWidth = DMA_MEMORYDATAWIDTH_BYTE;
  DMA_InitStructure.DMA_Mode = DMA_MODE_NORMAL;
  DMA_InitStructure.DMA_Priority = DMA_PRIORITY_VERYHIGH;
  DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;
  DMA_Init(SPI_SLAVE_Rx_DMA_Channel, &DMA_InitStructure);

  /* SPI_MASTER configuration ------------------------------------------------------*/
  SPI_DefaultInitParaConfig(&SPI_InitStructure);
  SPI_InitStructure.SPI_TransMode = SPI_TRANSMODE_TX_HALFDUPLEX;
  SPI_InitStructure.SPI_Mode = SPI_MODE_MASTER;
  SPI_InitStructure.SPI_FrameSize = SPI_FRAMESIZE_8BIT;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_LOW;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2EDGE;
  SPI_InitStructure.SPI_NSSSEL = SPI_NSSSEL_HARD;
  SPI_InitStructure.SPI_MCLKP = SPI_MCLKP_8;
  SPI_InitStructure.SPI_FirstBit = SPI_FIRSTBIT_MSB;
  SPI_InitStructure.SPI_CPOLY = 7;
  SPI_Init(SPI_MASTER, &SPI_InitStructure);

  /* SPI_SLAVE configuration ------------------------------------------------------*/
  SPI_InitStructure.SPI_TransMode = SPI_TRANSMODE_RX_HALFDUPLEX;
  SPI_InitStructure.SPI_Mode = SPI_MODE_SLAVE;
  SPI_Init(SPI_SLAVE, &SPI_InitStructure);

  /* Enable SPI_MASTER NSS output for master mode */
  SPI_NSSHardwareOutputEnable(SPI_MASTER, ENABLE);

  /* Enable SPI_SLAVE Rx request */
  SPI_I2S_DMAEnable(SPI_SLAVE, SPI_I2S_DMA_RX, ENABLE);

  /* Enable SPI_SLAVE */
  SPI_Enable(SPI_SLAVE, ENABLE);
  /* Enable SPI_MASTER */
  SPI_Enable(SPI_MASTER, ENABLE);

  /* Enable DMA1 Channel4 */
  DMA_ChannelEnable(SPI_SLAVE_Rx_DMA_Channel, ENABLE);

  /* Transfer procedure */
  while (TxIdx < BufferSize)
  {
    /* Wait for SPI_MASTER Tx buffer empty */
    while (SPI_I2S_GetFlagStatus(SPI_MASTER, SPI_I2S_FLAG_TE) == RESET);
    /* Send SPI_MASTER data */
    SPI_I2S_TxData(SPI_MASTER, SPI_MASTER_Buffer_Tx[TxIdx++]);
  }

  /* Wait for DMA1 channel4 transfer complete */
  while (!DMA_GetFlagStatus(SPI_SLAVE_Rx_DMA_FLAG));

  /* Check the correctness of written data */
  TransferStatus = Buffercmp(SPI_SLAVE_Buffer_Rx, SPI_MASTER_Buffer_Tx, BufferSize);
  /* TransferStatus = PASSED, if the transmitted and received data
     are equal */
  /* TransferStatus = FAILED, if the transmitted and received data
     are different */

  /* if passed ,LED2 lights */
  if(TransferStatus==PASSED)
  {
    AT32_LEDn_ON(LED2);
  }    
  else
  {
    AT32_LEDn_OFF(LED2);
  }
  
  while (1)
  {}
}

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{
  /* Enable peripheral clocks --------------------------------------------------*/
  /* Enable SPI_SLAVE DMA clock */
  RCC_AHBPeriphClockCmd(SPI_SLAVE_DMA_CLK, ENABLE);

  /* Enable SPI_MASTER clock and GPIO clock for SPI_MASTER and SPI_SLAVE */
  RCC_APB2PeriphClockCmd(SPI_MASTER_CLK , ENABLE);
  RCC_AHBPeriphClockCmd(SPI_MASTER_GPIO_CLK | SPI_MASTER_GPIO_CLK_NSS | SPI_SLAVE_GPIO_CLK | RCC_AHBPERIPH_GPIOF, ENABLE);
  /* Enable SPI_SLAVE Periph clock */
  RCC_APB1PeriphClockCmd(SPI_SLAVE_CLK, ENABLE);
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure;
  GPIO_StructInit(&GPIO_InitStructure);
  /* Configure SPI_MASTER pins: NSS, SCK and MOSI */
  GPIO_InitStructure.GPIO_Pins = SPI_MASTER_PIN_SCK | SPI_MASTER_PIN_MOSI;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pull = GPIO_Pull_NOPULL;
  GPIO_Init(SPI_MASTER_GPIO, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins = SPI_MASTER_PIN_NSS;
  GPIO_Init(SPI_MASTER_GPIO_NSS, &GPIO_InitStructure);
  GPIO_PinAFConfig(SPI_MASTER_GPIO_NSS, GPIO_PinsSource15, GPIO_AF_0);
  GPIO_PinAFConfig(SPI_MASTER_GPIO, GPIO_PinsSource3, GPIO_AF_0);
  GPIO_PinAFConfig(SPI_MASTER_GPIO, GPIO_PinsSource5, GPIO_AF_0);

  /* Configure SPI_SLAVE pins: NSS, SCK and MISO*/
  GPIO_InitStructure.GPIO_Pins =  SPI_SLAVE_PIN_MISO;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pull = GPIO_Pull_NOPULL;
  GPIO_Init(SPI_SLAVE_GPIO, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins =  SPI_SLAVE_PIN_NSS;
  GPIO_InitStructure.GPIO_Pull = GPIO_Pull_PU;
  GPIO_Init(SPI_SLAVE_GPIO, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins =  SPI_SLAVE_PIN_SCK;
  GPIO_InitStructure.GPIO_Pull = GPIO_Pull_PD;
  GPIO_Init(SPI_SLAVE_GPIO, &GPIO_InitStructure);
  GPIO_PinAFConfig(SPI_SLAVE_GPIO, GPIO_PinsSource12, GPIO_AF_0);
  GPIO_PinAFConfig(SPI_SLAVE_GPIO, GPIO_PinsSource13, GPIO_AF_0);
  GPIO_PinAFConfig(SPI_SLAVE_GPIO, GPIO_PinsSource14, GPIO_AF_0);
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


