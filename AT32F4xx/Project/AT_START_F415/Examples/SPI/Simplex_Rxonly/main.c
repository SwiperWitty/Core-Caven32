/**
  ******************************************************************************
  * File   : SPI/Simplex_Rxonly/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */
 

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "at32_board.h"


/** @addtogroup AT32F415_StdPeriph_Examples
  * @{
  */

/** @addtogroup SPI_Simplex_Rxonly
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define BufferSize 32
#define SPI_MASTER                   SPI1
#define SPI_MASTER_CLK               RCC_APB2PERIPH_SPI1
#define SPI_MASTER_GPIO              GPIOA
#define SPI_MASTER_GPIO_CLK          RCC_APB2PERIPH_GPIOA  
#define SPI_MASTER_PIN_SCK           GPIO_Pins_5
#define SPI_MASTER_PIN_MOSI          GPIO_Pins_7

#define SPI_SLAVE                    SPI2
#define SPI_SLAVE_CLK                RCC_APB1PERIPH_SPI2
#define SPI_SLAVE_GPIO               GPIOB
#define SPI_SLAVE_GPIO_CLK           RCC_APB2PERIPH_GPIOB 
#define SPI_SLAVE_PIN_SCK            GPIO_Pins_13
#define SPI_SLAVE_PIN_MOSI           GPIO_Pins_15 

uint32_t i=0;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
SPI_InitType   SPI_InitStructure;
uint8_t SPI_MASTER_Buffer_Tx[BufferSize] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                      0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
                                      0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
                                      0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C,
                                      0x1D, 0x1E, 0x1F, 0x20};

uint8_t SPI_SLAVE_Buffer_Rx[BufferSize];
__IO uint8_t TxIdx = 0, RxIdx = 0 ,k;
volatile TestStatus TransferStatus = FAILED;

/* Private functions ---------------------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
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

  /* GPIO configuration ------------------------------------------------------*/
  GPIO_Configuration();
  
  /* Config LED */
  AT32_Board_Init();  
  
  /* SPI_MASTER Config -------------------------------------------------------------*/
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
  SPI_Init(SPI_MASTER, &SPI_InitStructure);

  /* SPI_SLAVE Config -------------------------------------------------------------*/
  SPI_InitStructure.SPI_TransMode = SPI_TRANSMODE_RXONLY;  
  SPI_InitStructure.SPI_Mode = SPI_MODE_SLAVE;
  SPI_Init(SPI_SLAVE, &SPI_InitStructure);
  
  /* Enable SPI_SLAVE */
  SPI_Enable(SPI_SLAVE, ENABLE);
  /* Enable SPI_MASTER */
  SPI_Enable(SPI_MASTER, ENABLE);


  /* Transfer procedure */
  while (TxIdx < BufferSize)
  {
    /* Wait for SPI_MASTER Tx buffer empty */
    while (SPI_I2S_GetFlagStatus(SPI_MASTER, SPI_I2S_FLAG_TE) == RESET);
    /* Send SPI_MASTER data */
    SPI_I2S_TxData(SPI_MASTER, SPI_MASTER_Buffer_Tx[TxIdx++]);
    /* Wait for SPI_SLAVE data reception */
    while (SPI_I2S_GetFlagStatus(SPI_SLAVE, SPI_I2S_FLAG_RNE) == RESET);
    /* Read SPI_SLAVE received data */
    SPI_SLAVE_Buffer_Rx[RxIdx++] = SPI_I2S_RxData(SPI_SLAVE);
  }

  /* Check the correctness of written dada */
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

  /* Enable SPI_MASTER clock and GPIO clock for SPI_MASTER and SPI_SLAVE */
  RCC_APB2PeriphClockCmd(SPI_MASTER_GPIO_CLK | SPI_SLAVE_GPIO_CLK | SPI_MASTER_CLK | RCC_APB2PERIPH_GPIOF, ENABLE);

  /* Enable SPI_SLAVE Periph clock */
  RCC_APB1PeriphClockCmd(SPI_SLAVE_CLK, ENABLE);
}

/**
  * @brief  Configures the different SPI_MASTER and SPI_SLAVE GPIO ports.
  * @param  SPI_MASTER_Mode: Specifies the SPI_MASTER operating mode. 
  *            This parameter can be:
  *              -  SPI_MASTER_Mode_Master
  *              -  SPI_MASTER_Mode_Slave                 
  * @param  SPI_SLAVE_Mode: Specifies the SPI_SLAVE operating mode. 
  *            This parameter can be:
  *              -  SPI_SLAVE_Mode_Master
  *              -  SPI_SLAVE_Mode_Slave 
  * @retval None
  */
void GPIO_Configuration()
{
  GPIO_InitType GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pins = SPI_MASTER_PIN_SCK |SPI_MASTER_PIN_MOSI;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_10MHz;
  GPIO_Init(SPI_MASTER_GPIO,&GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pins = SPI_SLAVE_PIN_SCK |SPI_SLAVE_PIN_MOSI;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PD;
  GPIO_Init(SPI_SLAVE_GPIO,&GPIO_InitStructure);  
  
  /* wait for slave clock pull down */
  while(GPIO_ReadInputDataBit(SPI_SLAVE_GPIO,SPI_SLAVE_PIN_SCK)!=RESET);
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


