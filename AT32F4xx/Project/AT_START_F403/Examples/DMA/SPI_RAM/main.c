 /**
 **************************************************************************
 * File   : DMA/SPI_RAM/main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
 * Brief  : Main program body
 **************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "at32_board.h"
#include "platform_config.h"

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */
  
/** @addtogroup DMA_SPI_RAM
  * @{
  */   

/* Private typedef -----------------------------------------------------------*/
typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define BufferSize         32
#define CRCPolynomial      7

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
SPI_InitType    SPI_InitStructure;
DMA_InitType    DMA_InitStructure;
uint8_t SPI_MASTER_Buffer_Rx[BufferSize], SPI_SLAVE_Buffer_Rx[BufferSize];
volatile uint8_t SPI_MASTERCRCValue = 0, SPI_SLAVECRCValue = 0;
volatile TestStatus TransferStatus1 = FAILED, TransferStatus2 = FAILED;

uint8_t SPI_MASTER_Buffer_Tx[BufferSize] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
                                            0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,
                                            0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,
                                            0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20};

uint8_t SPI_SLAVE_Buffer_Tx[BufferSize] = {0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,
                                           0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,0x60,
                                           0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,
                                           0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,0x70};
/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
TestStatus Buffercmp(uint8_t* pBuffer, uint8_t* pBuffer1, uint16_t BufferLength);

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* Board.c init */
  AT32_Board_Init();
  

  /* Turn Off LED2 and LED3 */
  AT32_LEDn_OFF(LED2);
  AT32_LEDn_OFF(LED3);
  
  /* System Clocks Configuration */
  RCC_Configuration();
       
  /* Configure the GPIO ports */
  GPIO_Configuration();

  /* SPI_MASTER configuration ------------------------------------------------*/
  SPI_DefaultInitParaConfig(&SPI_InitStructure);
  SPI_InitStructure.SPI_TransMode = SPI_TRANSMODE_FULLDUPLEX;
  SPI_InitStructure.SPI_Mode      = SPI_MODE_MASTER;
  SPI_InitStructure.SPI_FrameSize = SPI_FRAMESIZE_8BIT;
  SPI_InitStructure.SPI_CPOL      = SPI_CPOL_LOW;
  SPI_InitStructure.SPI_CPHA      = SPI_CPHA_2EDGE;
  SPI_InitStructure.SPI_NSSSEL    = SPI_NSSSEL_SOFT;
  SPI_InitStructure.SPI_MCLKP     = SPI_MCLKP_512;
  SPI_InitStructure.SPI_FirstBit  = SPI_FIRSTBIT_MSB;
  SPI_InitStructure.SPI_CPOLY     = CRCPolynomial;
  SPI_Init(SPI_MASTER, &SPI_InitStructure);
  
  /* SPI_SLAVE configuration -------------------------------------------------*/
  SPI_InitStructure.SPI_Mode = SPI_MODE_SLAVE;
  SPI_Init(SPI_SLAVE, &SPI_InitStructure);

  /* SPI_MASTER_Rx_DMA_Channel configuration ---------------------------------*/
  DMA_Reset(SPI_MASTER_Rx_DMA_Channel);
  DMA_DefaultInitParaConfig(&DMA_InitStructure);
  DMA_InitStructure.DMA_PeripheralBaseAddr  = (uint32_t)SPI_MASTER_DR_Base;
  DMA_InitStructure.DMA_MemoryBaseAddr      = (uint32_t)SPI_MASTER_Buffer_Rx;
  DMA_InitStructure.DMA_Direction           = DMA_DIR_PERIPHERALSRC;
  DMA_InitStructure.DMA_BufferSize          = BufferSize;
  DMA_InitStructure.DMA_PeripheralInc       = DMA_PERIPHERALINC_DISABLE;
  DMA_InitStructure.DMA_MemoryInc           = DMA_MEMORYINC_ENABLE;
  DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_BYTE;
  DMA_InitStructure.DMA_MemoryDataWidth     = DMA_MEMORYDATAWIDTH_BYTE;
  DMA_InitStructure.DMA_Mode                = DMA_MODE_NORMAL;
  DMA_InitStructure.DMA_Priority            = DMA_PRIORITY_HIGH;
  DMA_InitStructure.DMA_MTOM                = DMA_MEMTOMEM_DISABLE;
  DMA_Init(SPI_MASTER_Rx_DMA_Channel, &DMA_InitStructure);

  /* SPI_MASTER_Tx_DMA_Channel configuration ---------------------------------*/
  DMA_Reset(SPI_MASTER_Tx_DMA_Channel);  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SPI_MASTER_DR_Base;
  DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)SPI_MASTER_Buffer_Tx;
  DMA_InitStructure.DMA_Direction          = DMA_DIR_PERIPHERALDST;
  DMA_InitStructure.DMA_Priority           = DMA_PRIORITY_LOW;
  DMA_Init(SPI_MASTER_Tx_DMA_Channel, &DMA_InitStructure);

  /* SPI_SLAVE_Rx_DMA_Channel configuration ----------------------------------*/
  DMA_Reset(SPI_SLAVE_Rx_DMA_Channel);  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SPI_SLAVE_DR_Base;
  DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)SPI_SLAVE_Buffer_Rx;
  DMA_InitStructure.DMA_Direction          = DMA_DIR_PERIPHERALSRC;
  DMA_InitStructure.DMA_Priority           = DMA_PRIORITY_VERYHIGH;
  DMA_Init(SPI_SLAVE_Rx_DMA_Channel, &DMA_InitStructure);

  /* SPI_SLAVE_Tx_DMA_Channel configuration ----------------------------------*/
  DMA_Reset(SPI_SLAVE_Tx_DMA_Channel);  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SPI_SLAVE_DR_Base;
  DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)SPI_SLAVE_Buffer_Tx;
  DMA_InitStructure.DMA_Direction          = DMA_DIR_PERIPHERALDST;
  DMA_InitStructure.DMA_Priority           = DMA_PRIORITY_MEDIUM;
  DMA_Init(SPI_SLAVE_Tx_DMA_Channel, &DMA_InitStructure);

  /* Enable SPI_MASTER DMA Tx request */
  SPI_I2S_DMAEnable(SPI_MASTER, SPI_I2S_DMA_TX, ENABLE);
  /* Enable SPI_MASTER DMA Rx request */
  SPI_I2S_DMAEnable(SPI_MASTER, SPI_I2S_DMA_RX, ENABLE);
  /* Enable SPI_SLAVE DMA Tx request */
  SPI_I2S_DMAEnable(SPI_SLAVE, SPI_I2S_DMA_TX, ENABLE);
  /* Enable SPI_SLAVE DMA Rx request */
  SPI_I2S_DMAEnable(SPI_SLAVE, SPI_I2S_DMA_RX, ENABLE);
 
  /* Enable SPI_MASTER CRC calculation */
  SPI_CRCEN(SPI_MASTER, ENABLE);
  /* Enable SPI_SLAVE CRC calculation */
  SPI_CRCEN(SPI_SLAVE, ENABLE);

  /* Enable SPI_SLAVE */
  SPI_Enable(SPI_SLAVE, ENABLE);
  
  /* Enable SPI_MASTER */
  SPI_Enable(SPI_MASTER, ENABLE);

  /* Enable DMA channels */
  DMA_ChannelEnable(SPI_MASTER_Rx_DMA_Channel, ENABLE);
  DMA_ChannelEnable(SPI_SLAVE_Rx_DMA_Channel, ENABLE);
  DMA_ChannelEnable(SPI_SLAVE_Tx_DMA_Channel, ENABLE);
  DMA_ChannelEnable(SPI_MASTER_Tx_DMA_Channel, ENABLE);
   
  /* Transfer complete */
  while(!DMA_GetFlagStatus(SPI_MASTER_Rx_DMA_FLAG));
  while(!DMA_GetFlagStatus(SPI_SLAVE_Rx_DMA_FLAG));
  while(!DMA_GetFlagStatus(SPI_SLAVE_Tx_DMA_FLAG));
  while(!DMA_GetFlagStatus(SPI_MASTER_Tx_DMA_FLAG));

  /* Wait for SPI_MASTER data reception: CRC transmitted by SPI_SLAVE */
  while(SPI_I2S_GetFlagStatus(SPI_MASTER, SPI_I2S_FLAG_RNE) == RESET);
  /* Wait for SPI_SLAVE data reception: CRC transmitted by SPI_MASTER */
  while(SPI_I2S_GetFlagStatus(SPI_SLAVE, SPI_I2S_FLAG_RNE) == RESET);

  /* Check the correctness of written dada */
  TransferStatus1 = Buffercmp(SPI_SLAVE_Buffer_Rx, SPI_MASTER_Buffer_Tx, BufferSize);
  TransferStatus2 = Buffercmp(SPI_MASTER_Buffer_Rx, SPI_SLAVE_Buffer_Tx, BufferSize);
  /* TransferStatus1, TransferStatus2 = PASSED, if the data transmitted and received 
     are correct */
  /* TransferStatus1, TransferStatus2 = FAILED, if the data transmitted and received  
     are different */

  /* Test on the SPI_MASTER CRCR ERROR flag */
  if ((SPI_I2S_GetFlagStatus(SPI_MASTER, SPI_FLAG_CERR)) != RESET)
  {
    TransferStatus1 = FAILED;
  }
  /* Test on the SPI_SLAVE CRCR ERROR flag */
  if ((SPI_I2S_GetFlagStatus(SPI_SLAVE, SPI_FLAG_CERR)) != RESET)
  {
    TransferStatus2 = FAILED;
  } 

  /* Read SPI_MASTER received CRC value */
  SPI_MASTERCRCValue = SPI_I2S_RxData(SPI_MASTER);
  /* Read SPI_SLAVE received CRC value */
  SPI_SLAVECRCValue = SPI_I2S_RxData(SPI_SLAVE);

  if (TransferStatus1 != FAILED)
  {
    /* OK */
    /* Turn on LD2 */
    AT32_LEDn_ON(LED2);
  }
  else
  { 
    /* KO */
    /* Turn Off LD2 */
    AT32_LEDn_OFF(LED2);
  }
    
  if (TransferStatus2 != FAILED)
  {	
    /* OK */
    /* Turn on LD3 */
    AT32_LEDn_ON(LED3);
  }
  else
  { 
    /* KO */
    /* Turn Off LD3 */
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
  /* Enable DMA1 or/and DMA2 clock */
  RCC_AHBPeriphClockCmd(SPI_MASTER_DMA_CLK | SPI_SLAVE_DMA_CLK, ENABLE);

  /* Enable SPI_MASTER clock and GPIO clock for SPI_MASTER and SPI_SLAVE */
  RCC_APB2PeriphClockCmd(SPI_MASTER_GPIO_CLK | SPI_SLAVE_GPIO_CLK |
                         SPI_MASTER_CLK, ENABLE);
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
  /* Configure SPI_MASTER pins: SCK and MOSI */
  GPIO_InitStructure.GPIO_Pins     = SPI_MASTER_PIN_SCK | SPI_MASTER_PIN_MOSI;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode     = GPIO_Mode_AF_PP;
  GPIO_Init(SPI_MASTER_GPIO, &GPIO_InitStructure);
  /* Configure SPI_MASTER pins:  MISO */
  GPIO_InitStructure.GPIO_Pins     = SPI_MASTER_PIN_MISO;
  GPIO_InitStructure.GPIO_Mode     = GPIO_Mode_IN_FLOATING;
  GPIO_Init(SPI_MASTER_GPIO, &GPIO_InitStructure);

  /* Configure SPI_SLAVE pins: SCK and MOSI */
  GPIO_InitStructure.GPIO_Pins     = SPI_SLAVE_PIN_SCK | SPI_SLAVE_PIN_MOSI;
  GPIO_InitStructure.GPIO_Mode     = GPIO_Mode_IN_PD;
  GPIO_Init(SPI_SLAVE_GPIO, &GPIO_InitStructure);
  while(GPIO_ReadInputDataBit(SPI_SLAVE_GPIO,SPI_SLAVE_PIN_SCK)!=RESET);
  
  /* Configure SPI_SLAVE pins: MISO  */
  GPIO_InitStructure.GPIO_Pins     = SPI_SLAVE_PIN_MISO ;
  GPIO_InitStructure.GPIO_Mode     = GPIO_Mode_AF_PP;
  GPIO_Init(SPI_SLAVE_GPIO, &GPIO_InitStructure);
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer, pBuffer1: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer identical to pBuffer1
  *         FAILED: pBuffer differs from pBuffer1
  */
TestStatus Buffercmp(uint8_t* pBuffer, uint8_t* pBuffer1, uint16_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer != *pBuffer1)
    {
      return FAILED;
    }
    
    pBuffer++;
    pBuffer1++;
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
  

