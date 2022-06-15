/**
  ******************************************************************************
  * File   : SPI/SPI_CRC/main.c 
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

/** @addtogroup SPI_CRC
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define BufferSize  32

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
SPI_InitType  SPI_InitStructure;
uint16_t SPI1_Buffer_Tx[BufferSize] = {0x0102, 0x0304, 0x0506, 0x0708, 0x090A, 0x0B0C,
                                  0x0D0E, 0x0F10, 0x1112, 0x1314, 0x1516, 0x1718,
                                  0x191A, 0x1B1C, 0x1D1E, 0x1F20, 0x2122, 0x2324,
                                  0x2526, 0x2728, 0x292A, 0x2B2C, 0x2D2E, 0x2F30,
                                  0x3132, 0x3334, 0x3536, 0x3738, 0x393A, 0x3B3C,
                                  0x3D3E, 0x3F40};
uint16_t SPI2_Buffer_Tx[BufferSize] = {0x5152, 0x5354, 0x5556, 0x5758, 0x595A, 0x5B5C,
                                  0x5D5E, 0x5F60, 0x6162, 0x6364, 0x6566, 0x6768,
                                  0x696A, 0x6B6C, 0x6D6E, 0x6F70, 0x7172, 0x7374,
                                  0x7576, 0x7778, 0x797A, 0x7B7C, 0x7D7E, 0x7F80,
                                  0x8182, 0x8384, 0x8586, 0x8788, 0x898A, 0x8B8C,
                                  0x8D8E, 0x8F90};
uint16_t SPI1_Buffer_Rx[BufferSize], SPI2_Buffer_Rx[BufferSize];
uint32_t TxIdx = 0, RxIdx = 0;
__IO uint16_t CRC1Value = 0, CRC2Value = 0;
volatile TestStatus TransferStatus1 = FAILED, TransferStatus2 = FAILED;

/* Private functions ---------------------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
TestStatus Buffercmp(uint16_t* pBuffer1, uint16_t* pBuffer2, uint16_t BufferLength);

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
  
  /* SPI1 configuration ------------------------------------------------------*/
  SPI_DefaultInitParaConfig(&SPI_InitStructure);  
  SPI_InitStructure.SPI_TransMode = SPI_TRANSMODE_FULLDUPLEX;
  SPI_InitStructure.SPI_Mode = SPI_MODE_MASTER;
  SPI_InitStructure.SPI_FrameSize = SPI_FRAMESIZE_16BIT;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_LOW;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2EDGE;
  SPI_InitStructure.SPI_NSSSEL = SPI_NSSSEL_SOFT;
  SPI_InitStructure.SPI_MCLKP = SPI_MCLKP_8;
  SPI_InitStructure.SPI_FirstBit = SPI_FIRSTBIT_MSB;
  SPI_InitStructure.SPI_CPOLY = 7;
  SPI_Init(SPI1, &SPI_InitStructure);

  /* SPI2 configuration ------------------------------------------------------*/
  SPI_InitStructure.SPI_Mode = SPI_MODE_SLAVE;
  SPI_Init(SPI2, &SPI_InitStructure);

  /* Enable SPI1 CRC calculation */
  SPI_CRCEN(SPI1, ENABLE);
  /* Enable SPI2 CRC calculation */
  SPI_CRCEN(SPI2, ENABLE);

  /* Enable SPI1 */
  SPI_Enable(SPI1, ENABLE);
  /* Enable SPI2 */
  SPI_Enable(SPI2, ENABLE);

  /* Transfer procedure */
  while (TxIdx < BufferSize - 1)
  {
    /* Wait for SPI1 Tx buffer empty */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TE) == RESET);
    /* Send SPI2 data */
    SPI_I2S_TxData(SPI2, SPI2_Buffer_Tx[TxIdx]);
    /* Send SPI1 data */
    SPI_I2S_TxData(SPI1, SPI1_Buffer_Tx[TxIdx++]);
    /* Wait for SPI2 data reception */
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RNE) == RESET);
    /* Read SPI2 received data */
    SPI2_Buffer_Rx[RxIdx] = SPI_I2S_RxData(SPI2);
    /* Wait for SPI1 data reception */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RNE) == RESET);
    /* Read SPI1 received data */
    SPI1_Buffer_Rx[RxIdx++] = SPI_I2S_RxData(SPI1);
  }

  /* Wait for SPI1 Tx buffer empty */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TE) == RESET);
  /* Wait for SPI2 Tx buffer empty */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TE) == RESET);

  /* Send last SPI2_Buffer_Tx data */
  SPI_I2S_TxData(SPI2, SPI2_Buffer_Tx[TxIdx]);
  /* Enable SPI2 CRC transmission */
  SPI_TxCRC(SPI2);
  /* Send last SPI1_Buffer_Tx data */
  SPI_I2S_TxData(SPI1, SPI1_Buffer_Tx[TxIdx]);
  /* Enable SPI1 CRC transmission */
  SPI_TxCRC(SPI1);

  /* Wait for SPI1 last data reception */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RNE) == RESET);
  /* Read SPI1 last received data */
  SPI1_Buffer_Rx[RxIdx] = SPI_I2S_RxData(SPI1);

  /* Wait for SPI2 last data reception */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RNE) == RESET);
  /* Read SPI2 last received data */
  SPI2_Buffer_Rx[RxIdx] = SPI_I2S_RxData(SPI2);

  /* Wait for SPI1 data reception: CRC transmitted by SPI2 */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RNE) == RESET);
  /* Wait for SPI2 data reception: CRC transmitted by SPI1 */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RNE) == RESET);

  /* Check the received data with the send ones */
  TransferStatus1 = Buffercmp(SPI2_Buffer_Rx, SPI1_Buffer_Tx, BufferSize);
  TransferStatus2 = Buffercmp(SPI1_Buffer_Rx, SPI2_Buffer_Tx, BufferSize);
  /* TransferStatus1, TransferStatus2 = PASSED, if the data transmitted and received
     are correct */
  /* TransferStatus1, TransferStatus2 = FAILED, if the data transmitted and received
     are different */

  /* Test on the SPI1 CRC Error flag */
  if ((SPI_I2S_GetFlagStatus(SPI1, SPI_FLAG_CERR)) == SET)
  {
    TransferStatus2 = FAILED;
  }

  /* Test on the SPI2 CRC Error flag */
  if ((SPI_I2S_GetFlagStatus(SPI2, SPI_FLAG_CERR)) == SET)
  {
    TransferStatus1 = FAILED;
  }

  /* Read SPI1 received CRC value */
  CRC1Value = SPI_I2S_RxData(SPI1);
  /* Read SPI2 received CRC value */
  CRC2Value = SPI_I2S_RxData(SPI2);

  /* if passed ,LED2 lights */
  if((TransferStatus1==PASSED)&&(TransferStatus2==PASSED)&&(CRC1Value==0x8026)&&(CRC2Value==0xCBF9))
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
  /* GPIOA, GPIOB and SPI1 clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA | RCC_AHBPERIPH_GPIOB | RCC_AHBPERIPH_GPIOF, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_SPI1, ENABLE);

  /* SPI2 Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_SPI2, ENABLE);
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
  /* Configure SPI1 pins: SCK, MISO and MOSI ---------------------------------*/
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_5 | GPIO_Pins_6 |GPIO_Pins_7;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pull = GPIO_Pull_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource5, GPIO_AF_0);
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource6, GPIO_AF_0);
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource7, GPIO_AF_0);
  
  /* Configure SPI2 pins: SCK, MISO and MOSI ---------------------------------*/
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_14 | GPIO_Pins_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pull = GPIO_Pull_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pull = GPIO_Pull_PD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOB, GPIO_PinsSource13, GPIO_AF_0);
  GPIO_PinAFConfig(GPIOB, GPIO_PinsSource14, GPIO_AF_0);
  GPIO_PinAFConfig(GPIOB, GPIO_PinsSource15, GPIO_AF_0);
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
  
