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

/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */

/** @addtogroup I2S_SPI_I2S_Switch
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define BufferSize   32

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
I2S_InitType I2S_InitStructure;
SPI_InitType SPI_InitStructure;
GPIO_InitType GPIO_InitStructure;
uint16_t I2S1_Buffer_Tx[BufferSize] = {0x0102, 0x0304, 0x0506, 0x0708, 0x090A, 0x0B0C,
                                  0x0D0E, 0x0F10, 0x1112, 0x1314, 0x1516, 0x1718,
                                  0x191A, 0x1B1C, 0x1D1E, 0x1F20, 0x2122, 0x2324,
                                  0x2526, 0x2728, 0x292A, 0x2B2C, 0x2D2E, 0x2F30,
                                  0x3132, 0x3334, 0x3536, 0x3738, 0x393A, 0x3B3C,
                                  0x3D3E, 0x3F40};

uint16_t SPI1_Buffer_Tx[BufferSize] = {0x5152, 0x5354, 0x5556, 0x5758, 0x595A, 0x5B5C,
                                  0x5D5E, 0x5F60, 0x6162, 0x6364, 0x6566, 0x6768,
                                  0x696A, 0x6B6C, 0x6D6E, 0x6F70, 0x7172, 0x7374,
                                  0x7576, 0x7778, 0x797A, 0x7B7C, 0x7D7E, 0x7F80,
                                  0x8182, 0x8384, 0x8586, 0x8788, 0x898A, 0x8B8C,
                                  0x8D8E, 0x8F90};

__IO uint16_t I2S2_Buffer_Rx[BufferSize];
__IO uint16_t SPI2_Buffer_Rx[BufferSize];
__IO uint8_t TxIdx = 0, RxIdx = 0;
volatile TestStatus TransferStatus1 = FAILED, TransferStatus2 = FAILED;
volatile TestStatus TransferStatus3 = FAILED;
ErrorStatus HSEStartUpStatus;

/* Private functions ---------------------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
TestStatus Buffercmp(uint16_t* pBuffer1, uint16_t* pBuffer2, uint16_t BufferLength);

/**
  * @brief   Main program
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
 
  /* Deinitializes the SPI2 and SPI1 peripheral registers --------------------*/
  SPI_I2S_Reset(SPI2);
  SPI_I2S_Reset(SPI1);
  I2S_DefaultInit(&I2S_InitStructure);
  
  /* I2S peripheral configuration */
  I2S_InitStructure.I2s_AudioProtocol = I2S_AUDIOPROTOCOL_PHILLIPS;
  I2S_InitStructure.I2S_FrameFormat = I2S_FRAMEFORMAT_DL16BIT_CHL32BIT;
  I2S_InitStructure.I2S_MCLKOE = I2S_MCLKOE_DISABLE;
  I2S_InitStructure.I2S_AudioFreq = I2S_AUDIOFREQ_48K;
  I2S_InitStructure.I2S_CPOL = I2S_CPOL_LOW;

  /* I2S1 Master Transmitter to I2S2 Slave Receiver communication ------------*/
  /* I2S1 configuration */
  I2S_InitStructure.I2S_Mode = I2S_MODE_MASTERTX;
  I2S_Init(SPI1, &I2S_InitStructure);

  /* I2S2 configuration */
  I2S_InitStructure.I2S_Mode = I2S_MODE_SLAVERX;
  I2S_Init(SPI2, &I2S_InitStructure);

  /* Enable the I2S2 */
  I2S_Enable(SPI2, ENABLE);

  /* Enable the I2S1 */
  I2S_Enable(SPI1, ENABLE);

  /* Begin the communication in I2S mode */
  while (RxIdx < BufferSize)
  {
    /* Wait the Tx buffer to be empty */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TE) == RESET)
    {}
    /* Send a data from I2S1 */
    SPI_I2S_TxData(SPI1, I2S1_Buffer_Tx[TxIdx++]);

    /* Wait the Rx buffer to be full */
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RNE) == RESET)
    {}
    /* Store the I2S2 received data in the relative data table */
    I2S2_Buffer_Rx[RxIdx++] = SPI_I2S_RxData(SPI2);
  }

  TransferStatus1 = Buffercmp((uint16_t *)I2S2_Buffer_Rx, I2S1_Buffer_Tx, BufferSize);
  /* TransferStatus1 = PASSED, if the data transmitted from I2S1 and received by
                               I2S2 are the same 
     TransferStatus1 = FAILED, if the data transmitted from I2S1 and received by
                               I2S2 are different */
  /* Disable the I2S1 */
  I2S_Enable(SPI1, DISABLE);

  /* Disable the I2S2 */
  I2S_Enable(SPI2, DISABLE);
  
  /* Reset TxIdx, RxIdx indexes */
  TxIdx = 0;
  RxIdx = 0;

  /* Deinitializes the SPI2 and SPI1 peripheral registers --------------------*/
  SPI_I2S_Reset(SPI2);
  SPI_I2S_Reset(SPI1);
  SPI_DefaultInitParaConfig(&SPI_InitStructure);
  
  /* Switch to SPI mode communication ----------------------------------------*/
  /* SPI1 configuration */
  SPI_InitStructure.SPI_TransMode = SPI_TRANSMODE_TX_HALFDUPLEX;
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
  SPI_InitStructure.SPI_TransMode = SPI_TRANSMODE_RXONLY;
  SPI_InitStructure.SPI_Mode = SPI_MODE_SLAVE;
  SPI_Init(SPI2, &SPI_InitStructure);
  
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Pull = GPIO_Pull_PD;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Pull = GPIO_Pull_PD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_5;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Enable SPI2 */
  SPI_Enable(SPI2, ENABLE);
  /* Enable SPI1 */
  SPI_Enable(SPI1, ENABLE);

  /* Begin the communication in SPI mode */
  while (RxIdx < BufferSize)
  {
    /* Wait the Tx buffer to be empty */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TE) == RESET)
    {}
    /* Send a data from SPI1 */
    SPI_I2S_TxData(SPI1, SPI1_Buffer_Tx[TxIdx++]);

    /* Wait the Rx buffer to be full */
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RNE) == RESET)
    {}
    /* Store the SPI2 received data in the relative data table */
    SPI2_Buffer_Rx[RxIdx++] = SPI_I2S_RxData(SPI2);
  }

  TransferStatus2 = Buffercmp((uint16_t *)SPI2_Buffer_Rx, SPI1_Buffer_Tx, BufferSize);
  /* TransferStatus2 = PASSED, if the data transmitted from SPI1 and received by
                               SPI2 are the same
     TransferStatus2 = FAILED, if the data transmitted from SPI1 and received by
                               SPI2 are different */
  /* Disable SPI2 */
  SPI_Enable(SPI2, DISABLE);
  /* Disable SPI1 */
  SPI_Enable(SPI1, DISABLE);  

  /* Reset TxIdx, RxIdx indexes and receive table values */
  for (TxIdx = 0; TxIdx < BufferSize; TxIdx++)
  {
    I2S2_Buffer_Rx[TxIdx] = 0;
  }
  
  TxIdx = 0;
  RxIdx = 0;

  /* Deinitializes the SPI2 and SPI3 peripheral registers --------------------*/
  SPI_I2S_Reset(SPI2);
  SPI_I2S_Reset(SPI1);
  
  /* I2S1 Slave Transmitter to I2S2 Master Receiver communication ------------*/
  /* I2S1 configuration */
  I2S_InitStructure.I2S_Mode = I2S_MODE_SLAVETX;
  I2S_Init(SPI1, &I2S_InitStructure);

  /* I2S2 configuration */
  I2S_InitStructure.I2S_Mode = I2S_MODE_MASTERRX;
  I2S_Init(SPI2, &I2S_InitStructure);

  /* Wait the Tx buffer to be empty */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TE) == RESET)
  {}
  /* Prepare the first data to be sent from the slave */
  SPI_I2S_TxData(SPI1, I2S1_Buffer_Tx[TxIdx++]);

  /* Enable the I2S1 */
  I2S_Enable(SPI1, ENABLE);

  /* Enable the I2S2 */
  I2S_Enable(SPI2, ENABLE);

  /* Begin the communication in I2S mode */
  while (RxIdx < BufferSize)
  {
    /* Wait the Rx buffer to be full */
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RNE) == RESET)
    {}
    /* Store the I2S2 received data in the relative data table */
    I2S2_Buffer_Rx[RxIdx++] = SPI_I2S_RxData(SPI2);

    /* Wait the Tx buffer to be empty */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TE) == RESET)
    {}
    /* Send a data from I2S1 */
    SPI_I2S_TxData(SPI1, I2S1_Buffer_Tx[TxIdx++]);
  }

  TransferStatus3 = Buffercmp((uint16_t *)I2S2_Buffer_Rx, I2S1_Buffer_Tx, BufferSize);
  /* TransferStatus3 = PASSED, if the data transmitted from I2S1 and received by
                               I2S2 are the same
     TransferStatus3 = FAILED, if the data transmitted from I2S1 and received by
                               I2S2 are different */
 
  /* if passed ,LED2 lights */
  if((TransferStatus1==PASSED)&&(TransferStatus2==PASSED)&&(TransferStatus3==PASSED))
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
  /* GPIOA, GPIOB and AFIO clocks enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA | RCC_AHBPERIPH_GPIOB | RCC_AHBPERIPH_GPIOF, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_SPI1, ENABLE);
  
  /* SPI2 and SPI1 clocks enable */
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
  /* Configure SPI2 pins: CK, WS and SD ---------------------------------*/
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pull = GPIO_Pull_PU;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_13 | GPIO_Pins_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pull = GPIO_Pull_PD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOB, GPIO_PinsSource12, GPIO_AF_0);
  GPIO_PinAFConfig(GPIOB, GPIO_PinsSource13, GPIO_AF_0);
  GPIO_PinAFConfig(GPIOB, GPIO_PinsSource15, GPIO_AF_0);

  /* Configure SPI1 pins: CK and SD ------------------------------------*/
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_4 | GPIO_Pins_5 |GPIO_Pins_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pull = GPIO_Pull_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource4, GPIO_AF_0);
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource5, GPIO_AF_0);
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource7, GPIO_AF_0);  
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
  
