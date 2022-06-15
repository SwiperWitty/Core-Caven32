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


/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup I2S_Interrupt
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
I2S_InitType I2S_InitStructure;
const uint16_t I2S3_Buffer_Tx[32] = {0x0102, 0x0304, 0x0506, 0x0708, 0x090A, 0x0B0C,
                                     0x0D0E, 0x0F10, 0x1112, 0x1314, 0x1516, 0x1718,
                                     0x191A, 0x1B1C, 0x1D1E, 0x1F20, 0x2122, 0x2324,
                                     0x2526, 0x2728, 0x292A, 0x2B2C, 0x2D2E, 0x2F30,
                                     0x3132, 0x3334, 0x3536, 0x3738, 0x393A, 0x3B3C,
                                     0x3D3E, 0x3F40
                                    };

uint16_t I2S2_Buffer_Rx[32];
__IO uint32_t TxIdx = 0, RxIdx = 0;
TestStatus TransferStatus1 = FAILED, TransferStatus2 = FAILED;
ErrorStatus HSEStartUpStatus;

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
TestStatus Buffercmp(uint16_t* pBuffer1, uint16_t* pBuffer2, uint16_t BufferLength);
TestStatus Buffercmp24bits(uint16_t* pBuffer1, uint16_t* pBuffer2, uint16_t BufferLength);

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
       file (startup_at32f403_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_at32f4xx.c file
     */

  /* System clocks configuration ---------------------------------------------*/
  RCC_Configuration();

  /* NVIC configuration ------------------------------------------------------*/
  NVIC_Configuration();

  /* GPIO configuration ------------------------------------------------------*/
  GPIO_Configuration();

  /* Config LED */ 
  AT32_Board_Init();
  
  SPI_I2S_Reset(SPI3);
  SPI_I2S_Reset(SPI2);

  /* I2S peripheral configuration */
  I2S_DefaultInit(&I2S_InitStructure);
  I2S_InitStructure.I2s_AudioProtocol = I2S_AUDIOPROTOCOL_PHILLIPS;
  I2S_InitStructure.I2S_FrameFormat = I2S_FRAMEFORMAT_DL16BIT_CHL32BIT;
  I2S_InitStructure.I2S_MCLKOE = I2S_MCLKOE_DISABLE;
  I2S_InitStructure.I2S_AudioFreq = I2S_AUDIOFREQ_48K;
  I2S_InitStructure.I2S_CPOL = I2S_CPOL_LOW;

  /* I2S3 Master Transmitter to I2S2 Slave Receiver communication -----------*/
  /* I2S3 configuration */
  I2S_InitStructure.I2S_Mode = I2S_MODE_MASTERTX;
  I2S_Init(SPI3, &I2S_InitStructure);

  /* I2S2 configuration */
  I2S_InitStructure.I2S_Mode = I2S_MODE_SLAVERX;
  I2S_Init(SPI2, &I2S_InitStructure);

  /* Enable the I2S3 TxE interrupt */
  SPI_I2S_INTConfig(SPI3, SPI_I2S_INT_TE, ENABLE);

  /* Enable the I2S2 RxNE interrupt */
  SPI_I2S_INTConfig(SPI2, SPI_I2S_INT_RNE, ENABLE);

  /* Enable the I2S2 */
  I2S_Enable(SPI2, ENABLE);

  /* Enable the I2S3 */
  I2S_Enable(SPI3, ENABLE);

  /* Wait the end of communication */
  while (RxIdx < 32)
  {}

  TransferStatus1 = Buffercmp(I2S2_Buffer_Rx, (uint16_t*)I2S3_Buffer_Tx, 32);
  /* TransferStatus1 = PASSED, if the data transmitted from I2S3 and received by
                               I2S2 are the same
     TransferStatus1 = FAILED, if the data transmitted from I2S3 and received by
                               I2S2 are different */

  /* Reinitialize the buffers */
  for (RxIdx = 0; RxIdx < 32; RxIdx++)
  {
    I2S2_Buffer_Rx[RxIdx] = 0;
  }

  TxIdx = 0;
  RxIdx = 0;

  SPI_I2S_Reset(SPI3);
  SPI_I2S_Reset(SPI2);

  /* I2S peripheral configuration */
  I2S_InitStructure.I2s_AudioProtocol = I2S_AUDIOPROTOCOL_PHILLIPS;
  I2S_InitStructure.I2S_FrameFormat = I2S_FRAMEFORMAT_DL24BIT_CHL32BIT;
  I2S_InitStructure.I2S_MCLKOE = I2S_MCLKOE_DISABLE;
  I2S_InitStructure.I2S_AudioFreq = I2S_AUDIOFREQ_16K;
  I2S_InitStructure.I2S_CPOL = I2S_CPOL_LOW;

  /* I2S3 Master Transmitter to I2S2 Slave Receiver communication -----------*/
  /* I2S3 configuration */
  I2S_InitStructure.I2S_Mode = I2S_MODE_MASTERTX;
  I2S_Init(SPI3, &I2S_InitStructure);

  /* I2S2 configuration */
  I2S_InitStructure.I2S_Mode = I2S_MODE_SLAVERX;
  I2S_Init(SPI2, &I2S_InitStructure);

  /* Enable the I2S3 TxE interrupt */
  SPI_I2S_INTConfig(SPI3, SPI_I2S_INT_TE, ENABLE);

  /* Enable the I2S2 RxNE interrupt */
  SPI_I2S_INTConfig(SPI2, SPI_I2S_INT_RNE, ENABLE);

  /* Enable the I2S2 */
  I2S_Enable(SPI2, ENABLE);

  /* Enable the I2S3 */
  I2S_Enable(SPI3, ENABLE);

  /* Wait the end of communication */
  while (RxIdx < 32)
  {
  }

  TransferStatus2 = Buffercmp24bits(I2S2_Buffer_Rx, (uint16_t*)I2S3_Buffer_Tx, 32);
  /* TransferStatus2 = PASSED, if the data transmitted from I2S3 and received by
                               I2S2 are the same
     TransferStatus2 = FAILED, if the data transmitted from I2S3 and received by
                               I2S2 are different */

  /* if passed ,LED2 lights */
  if((TransferStatus1==PASSED)&&(TransferStatus2==PASSED))
  {
    AT32_LEDn_ON(LED2);
  }    
  else
  {
    AT32_LEDn_OFF(LED2);
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
  /* GPIOA, GPIOB and AFIO clocks enable */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA | RCC_APB2PERIPH_GPIOB | RCC_APB2PERIPH_GPIOD |
                         RCC_APB2PERIPH_AFIO, ENABLE);

  /* SPI3 and SPI2 clocks enable */
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_SPI3 | RCC_APB1PERIPH_SPI2, ENABLE);
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure;

  GPIO_PinsRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

  /* Configure SPI2 pins: CK, WS and SD ---------------------------------*/
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_12 | GPIO_Pins_13 | GPIO_Pins_15;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Configure SPI3 pins: CK and SD ------------------------------------*/
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_3 | GPIO_Pins_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Configure SPI3 pins: WS -------------------------------------------*/
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_15;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
}

/**
  * @brief  Configure the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
  NVIC_InitType NVIC_InitStructure;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

  /* SPI3 IRQ Channel configuration */
  NVIC_InitStructure.NVIC_IRQChannel = SPI3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* SPI2 IRQ channel configuration */
  NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_Init(&NVIC_InitStructure);
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

/**
  * @brief  Compares two buffers in 24 bits data format.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer1 identical to pBuffer2
  *         FAILED: pBuffer1 differs from pBuffer2
  */
TestStatus Buffercmp24bits(uint16_t* pBuffer1, uint16_t* pBuffer2, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      if (*pBuffer1 != (*pBuffer2 & 0xFF00))
      {
        return FAILED;
      }
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
  
