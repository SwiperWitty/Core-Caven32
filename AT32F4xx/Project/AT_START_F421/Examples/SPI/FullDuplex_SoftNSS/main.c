/**
  ******************************************************************************
  * File   : SPI/FullDuplex_SoftNSS/main.c 
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
  
/** @addtogroup SPI_FullDuplex_SoftNSS
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define BufferSize 32
#define SPIy                   SPI1
#define SPIy_CLK               RCC_APB2PERIPH_SPI1
#define SPIy_GPIO              GPIOA
#define SPIy_GPIO_CLK          RCC_AHBPERIPH_GPIOA  
#define SPIy_PIN_SCK           GPIO_Pins_5
#define SPIy_PIN_MISO          GPIO_Pins_6
#define SPIy_PIN_MOSI          GPIO_Pins_7

#define SPIz                    SPI2
#define SPIz_CLK                RCC_APB1PERIPH_SPI2
#define SPIz_GPIO               GPIOB
#define SPIz_GPIO_CLK           RCC_AHBPERIPH_GPIOB 
#define SPIz_PIN_SCK            GPIO_Pins_13
#define SPIz_PIN_MISO           GPIO_Pins_14
#define SPIz_PIN_MOSI           GPIO_Pins_15 

uint32_t i=0;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
SPI_InitType   SPI_InitStructure;
uint8_t SPIy_Buffer_Tx[BufferSize] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                      0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
                                      0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
                                      0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C,
                                      0x1D, 0x1E, 0x1F, 0x20};
uint8_t SPIz_Buffer_Tx[BufferSize] = {0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
                                      0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E,
                                      0x5F, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 
                                      0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C,
                                      0x6D, 0x6E, 0x6F, 0x70};
uint8_t SPIy_Buffer_Rx[BufferSize], SPIz_Buffer_Rx[BufferSize];
__IO uint8_t TxIdx = 0, RxIdx = 0, k = 0;
volatile TestStatus TransferStatus1 = FAILED, TransferStatus2 = FAILED;
volatile TestStatus TransferStatus3 = FAILED, TransferStatus4 = FAILED;

/* Private functions ---------------------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(uint16_t SPIy_Mode, uint16_t SPIz_Mode);
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);

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

  /* 1st phase: SPIy Master and SPIz Slave */
  /* GPIO configuration ------------------------------------------------------*/
  GPIO_Configuration(SPI_MODE_MASTER, SPI_MODE_SLAVE);
  
  /* Config LED */
  AT32_Board_Init();
  
  /* SPIy Config -------------------------------------------------------------*/
  SPI_DefaultInitParaConfig(&SPI_InitStructure);
  SPI_InitStructure.SPI_TransMode = SPI_TRANSMODE_FULLDUPLEX;
  SPI_InitStructure.SPI_Mode = SPI_MODE_MASTER;
  SPI_InitStructure.SPI_FrameSize = SPI_FRAMESIZE_8BIT;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_LOW;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2EDGE;
  SPI_InitStructure.SPI_NSSSEL = SPI_NSSSEL_SOFT;
  SPI_InitStructure.SPI_MCLKP = SPI_MCLKP_8;
  SPI_InitStructure.SPI_FirstBit = SPI_FIRSTBIT_LSB;
  SPI_InitStructure.SPI_CPOLY = 7;
  SPI_Init(SPIy, &SPI_InitStructure);

  /* SPIz Config -------------------------------------------------------------*/
  SPI_InitStructure.SPI_Mode = SPI_MODE_SLAVE;
  SPI_Init(SPIz, &SPI_InitStructure);

  /* Enable SPIy */
  SPI_Enable(SPIy, ENABLE);
  /* Enable SPIz */
  SPI_Enable(SPIz, ENABLE);

  /* Transfer procedure */
  while (TxIdx < BufferSize)
  {
    /* Wait for SPIy Tx buffer empty */
    while (SPI_I2S_GetFlagStatus(SPIy, SPI_I2S_FLAG_TE) == RESET);
    /* Send SPIz data */
    SPI_I2S_TxData(SPIz, SPIz_Buffer_Tx[TxIdx]);
    /* Send SPIy data */
    SPI_I2S_TxData(SPIy, SPIy_Buffer_Tx[TxIdx++]);
    /* Wait for SPIz data reception */
    while (SPI_I2S_GetFlagStatus(SPIz, SPI_I2S_FLAG_RNE) == RESET);
    /* Read SPIz received data */
    SPIz_Buffer_Rx[RxIdx] = SPI_I2S_RxData(SPIz);
    /* Wait for SPIy data reception */
    while (SPI_I2S_GetFlagStatus(SPIy, SPI_I2S_FLAG_RNE) == RESET);
    /* Read SPIy received data */
    SPIy_Buffer_Rx[RxIdx++] = SPI_I2S_RxData(SPIy);
  }

  /* Check the correctness of written dada */
  TransferStatus1 = Buffercmp(SPIz_Buffer_Rx, SPIy_Buffer_Tx, BufferSize);
  TransferStatus2 = Buffercmp(SPIy_Buffer_Rx, SPIz_Buffer_Tx, BufferSize);
  /* TransferStatus1, TransferStatus2 = PASSED, if the transmitted and received data
     are equal */
  /* TransferStatus1, TransferStatus2 = FAILED, if the transmitted and received data
     are different */
  SPI_Enable(SPIy, DISABLE);
  /* Enable SPIz */
  SPI_Enable(SPIz, DISABLE);
  /* 2nd phase: SPIy Slave and SPIz Master */
  /* GPIO configuration ------------------------------------------------------*/
  GPIO_Configuration(SPI_MODE_SLAVE , SPI_MODE_MASTER);
  
  /* SPIy Re-configuration ---------------------------------------------------*/
  SPI_InitStructure.SPI_Mode = SPI_MODE_SLAVE;
  SPI_Init(SPIy, &SPI_InitStructure);

  /* SPIz Re-configuration ---------------------------------------------------*/
  SPI_InitStructure.SPI_Mode = SPI_MODE_MASTER;
  SPI_Init(SPIz, &SPI_InitStructure);

  /* Reset TxIdx, RxIdx indexes and receive tables values */
  TxIdx = 0;
  RxIdx = 0;
  for (k = 0; k < BufferSize; k++)  SPIz_Buffer_Rx[k] = 0;
  for (k = 0; k < BufferSize; k++)  SPIy_Buffer_Rx[k] = 0;

  /* Enable SPIy */
  SPI_Enable(SPIz, ENABLE);
  /* Enable SPIz */
  SPI_Enable(SPIy, ENABLE);

  /* Transfer procedure */
  while (TxIdx < BufferSize)
  {
    /* Wait for SPIz Tx buffer empty */
    while (SPI_I2S_GetFlagStatus(SPIz, SPI_I2S_FLAG_TE) == RESET);
    /* Send SPIy data */
    SPI_I2S_TxData(SPIy, SPIy_Buffer_Tx[TxIdx]);
    /* Send SPIz data */
    SPI_I2S_TxData(SPIz, SPIz_Buffer_Tx[TxIdx++]);
    /* Wait for SPIy data reception */
    while (SPI_I2S_GetFlagStatus(SPIy, SPI_I2S_FLAG_RNE) == RESET);
    /* Read SPIy received data */
    SPIy_Buffer_Rx[RxIdx] = SPI_I2S_RxData(SPIy);
    /* Wait for SPIz data reception */
    while (SPI_I2S_GetFlagStatus(SPIz, SPI_I2S_FLAG_RNE) == RESET);
    /* Read SPIz received data */
    SPIz_Buffer_Rx[RxIdx++] = SPI_I2S_RxData(SPIz);
  }

  /* Check the correctness of written dada */
  TransferStatus3 = Buffercmp(SPIz_Buffer_Rx, SPIy_Buffer_Tx, BufferSize);
  TransferStatus4 = Buffercmp(SPIy_Buffer_Rx, SPIz_Buffer_Tx, BufferSize);
  /* TransferStatus3, TransferStatus4 = PASSED, if the transmitted and received data
     are equal */
  /* TransferStatus3, TransferStatus4 = FAILED, if the transmitted and received data
     are different */

  /* if passed ,LED2 lights */
  if((TransferStatus1==PASSED)&&(TransferStatus2==PASSED)&&(TransferStatus3==PASSED)&&(TransferStatus4==PASSED))
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

  /* Enable SPIy clock and GPIO clock for SPIy and SPIz */
  RCC_APB2PeriphClockCmd(SPIy_CLK, ENABLE);
  RCC_AHBPeriphClockCmd(SPIy_GPIO_CLK | SPIz_GPIO_CLK | RCC_AHBPERIPH_GPIOF, ENABLE);

  /* Enable SPIz Periph clock */
  RCC_APB1PeriphClockCmd(SPIz_CLK, ENABLE);
}

/**
  * @brief  Configures the different SPIy and SPIz GPIO ports.
  * @param  SPIy_Mode: Specifies the SPIy operating mode. 
  *            This parameter can be:
  *              -  SPIy_Mode_Master
  *              -  SPIy_Mode_Slave                 
  * @param  SPIz_Mode: Specifies the SPIz operating mode. 
  *            This parameter can be:
  *              -  SPIz_Mode_Master
  *              -  SPIz_Mode_Slave 
  * @retval None
  */
void GPIO_Configuration(uint16_t SPIy_Mode, uint16_t SPIz_Mode)
{
  GPIO_InitType GPIO_InitStructure;
  GPIO_StructInit(&GPIO_InitStructure);
  /* Configure SPIy pins: SCK, MISO and MOSI ---------------------------------*/
  GPIO_InitStructure.GPIO_Pins = SPIy_PIN_SCK | SPIy_PIN_MOSI;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  if(SPIy_Mode == SPI_MODE_MASTER)
  {
    /* Configure SCK and MOSI pins as Alternate Function NOPULL */
    GPIO_InitStructure.GPIO_Pull = GPIO_Pull_NOPULL;
  }
  else
  {
    /* Configure SCK and MOSI pins as Alternate Function PD */
    GPIO_InitStructure.GPIO_Pull = GPIO_Pull_PD;
  }
  GPIO_Init(SPIy_GPIO, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pins = SPIy_PIN_MISO;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pull = GPIO_Pull_NOPULL;
  GPIO_Init(SPIy_GPIO, &GPIO_InitStructure);
  GPIO_PinAFConfig(SPIy_GPIO, GPIO_PinsSource5, GPIO_AF_0);
  GPIO_PinAFConfig(SPIy_GPIO, GPIO_PinsSource6, GPIO_AF_0);
  GPIO_PinAFConfig(SPIy_GPIO, GPIO_PinsSource7, GPIO_AF_0);
  /* Configure SPIz pins: SCK, MISO and MOSI ---------------------------------*/
  GPIO_InitStructure.GPIO_Pins = SPIz_PIN_SCK | SPIz_PIN_MOSI;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  if(SPIz_Mode == SPI_MODE_SLAVE)
  {
    /* Configure SCK and MOSI pins as Alternate Function PD */
    GPIO_InitStructure.GPIO_Pull = GPIO_Pull_PD;
  }
  else
  { 
    /* Configure SCK and MOSI pins as Alternate Function NOPULL */
    GPIO_InitStructure.GPIO_Pull = GPIO_Pull_NOPULL;
  }
  GPIO_Init(SPIz_GPIO, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pins = SPIz_PIN_MISO;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pull = GPIO_Pull_NOPULL;
  GPIO_Init(SPIz_GPIO, &GPIO_InitStructure);
  GPIO_PinAFConfig(SPIz_GPIO, GPIO_PinsSource13, GPIO_AF_0);
  GPIO_PinAFConfig(SPIz_GPIO, GPIO_PinsSource14, GPIO_AF_0);
  GPIO_PinAFConfig(SPIz_GPIO, GPIO_PinsSource15, GPIO_AF_0);
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
  
