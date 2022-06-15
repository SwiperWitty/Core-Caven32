/**
  ******************************************************************************
  * File   : SPI/HalfDuplex_Switch/main.c 
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

/** @addtogroup SPI_HalfDuplex_Switch
  * @{
  */ 
  
/* Private define ------------------------------------------------------------*/
#define SPI_MASTER                   SPI1
#define SPI_MASTER_CLK               RCC_APB2PERIPH_SPI1
#define SPI_MASTER_GPIO              GPIOA
#define SPI_MASTER_GPIO_CLK          RCC_APB2PERIPH_GPIOA  
#define SPI_MASTER_PIN_SCK           GPIO_Pins_5
#define SPI_MASTER_PIN_MISO          GPIO_Pins_6
#define SPI_MASTER_PIN_MOSI          GPIO_Pins_7
#define SPI_MASTER_IRQn              SPI1_IRQn

#define SPI_SLAVE                    SPI2
#define SPI_SLAVE_CLK                RCC_APB1PERIPH_SPI2
#define SPI_SLAVE_GPIO               GPIOB
#define SPI_SLAVE_GPIO_CLK           RCC_APB2PERIPH_GPIOB 
#define SPI_SLAVE_PIN_SCK            GPIO_Pins_13
#define SPI_SLAVE_PIN_MISO           GPIO_Pins_14
#define SPI_SLAVE_PIN_MOSI           GPIO_Pins_15 
#define SPI_SLAVE_IRQn               SPI2_I2S2EXT_IRQn

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define BufferSize 32

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
SPI_InitType SPI_InitStructure;
uint8_t SPI_MASTER_Buffer_Tx[BufferSize] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                                            0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
                                            0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12,
                                            0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                                            0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E,
                                            0x1F, 0x20};
uint8_t SPI_SLAVE_Buffer_Rx[BufferSize];
uint8_t SPI_SLAVE_Buffer_Tx[BufferSize] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
                                            0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC,
                                            0xDD, 0xEE, 0xFF, 0x12, 0x23, 0x34,
                                            0x45, 0x56, 0x67, 0x78, 0x89, 0x9A,
                                            0xAB, 0xBC, 0xCD, 0xDE, 0xEF, 0xFE,
                                            0xFC, 0xFF};
uint8_t SPI_MASTER_Buffer_Rx[BufferSize];                                            
__IO uint8_t TxIdx = 0, RxIdx = 0 ,i=0;
uint16_t ipd_flag=0;
volatile TestStatus TransferStatus = FAILED , TransferStatus1 = FAILED;

/* Private functions ---------------------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
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
  
  /* SPI_MASTER configuration ------------------------------------------------*/
  SPI_DefaultInitParaConfig(&SPI_InitStructure);
  SPI_InitStructure.SPI_TransMode = SPI_TRANSMODE_TX_HALFDUPLEX;
  SPI_InitStructure.SPI_Mode = SPI_MODE_MASTER;
  SPI_InitStructure.SPI_FrameSize = SPI_FRAMESIZE_8BIT;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_LOW;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2EDGE;
  SPI_InitStructure.SPI_NSSSEL = SPI_NSSSEL_SOFT;
  SPI_InitStructure.SPI_MCLKP = SPI_MCLKP_256;
  SPI_InitStructure.SPI_FirstBit = SPI_FIRSTBIT_MSB;
  SPI_InitStructure.SPI_CPOLY = 7;
  SPI_Init(SPI_MASTER, &SPI_InitStructure);

  /* SPI_SLAVE configuration -------------------------------------------------*/
  SPI_InitStructure.SPI_TransMode = SPI_TRANSMODE_RX_HALFDUPLEX;
  SPI_InitStructure.SPI_Mode = SPI_MODE_SLAVE;
  SPI_Init(SPI_SLAVE, &SPI_InitStructure);

  /* Enable SPI_MASTER TXE interrupt */
  SPI_I2S_INTConfig(SPI_MASTER, SPI_I2S_INT_TE, ENABLE);
  /* Enable SPI_SLAVE RXNE interrupt */
  SPI_I2S_INTConfig(SPI_SLAVE, SPI_I2S_INT_RNE, ENABLE);

  /* Enable SPI_SLAVE */
  SPI_Enable(SPI_SLAVE, ENABLE);
  /* Enable SPI_MASTER */
  SPI_Enable(SPI_MASTER, ENABLE);

  /* Transfer procedure */
  while (RxIdx < BufferSize)
  {}

  /* Check the correctness of written dada */
  TransferStatus = Buffercmp(SPI_SLAVE_Buffer_Rx, SPI_MASTER_Buffer_Tx, BufferSize);
  /* TransferStatus = PASSED, if the transmitted and received data
     are equal */
  /* TransferStatus = FAILED, if the transmitted and received data
     are different */
    
  /* Disable SPI_SLAVE */
  SPI_Enable(SPI_SLAVE, DISABLE);
  /* Disable SPI_MASTER */
  SPI_Enable(SPI_MASTER, DISABLE);    
    
  /* Clear TX/RX conuter */  
  TxIdx=0;
  RxIdx=0;    
    
  /* Disable SPI_MASTER TXE interrupt */
  SPI_I2S_INTConfig(SPI_MASTER, SPI_I2S_INT_TE, DISABLE);
  /* Disable SPI_SLAVE RXNE interrupt */
  SPI_I2S_INTConfig(SPI_SLAVE, SPI_I2S_INT_RNE, DISABLE);    
            
  /* SPI MASTER configuration */          
  SPI_InitStructure.SPI_TransMode = SPI_TRANSMODE_RX_HALFDUPLEX;
  SPI_InitStructure.SPI_Mode = SPI_MODE_MASTER;    
  SPI_Init(SPI_MASTER, &SPI_InitStructure);
    
  /* SPI SLAVE configuration */              
  SPI_InitStructure.SPI_TransMode = SPI_TRANSMODE_TX_HALFDUPLEX;
  SPI_InitStructure.SPI_Mode = SPI_MODE_SLAVE;
  SPI_Init(SPI_SLAVE, &SPI_InitStructure);    
    
  /* Enable SPI_SLAVE TXE interrupt */
  SPI_I2S_INTConfig(SPI_SLAVE, SPI_I2S_INT_TE, ENABLE);     
  /* Enable SPI_MASTER RXNE interrupt */
  SPI_I2S_INTConfig(SPI_MASTER, SPI_I2S_INT_RNE, ENABLE); 

  /* Enable SPI_SLAVE */
  SPI_Enable(SPI_SLAVE, ENABLE);
  /* Enable SPI_MASTER */
  SPI_Enable(SPI_MASTER, ENABLE);  
  
  /* Transfer procedure */
  while (RxIdx < BufferSize)
  {}
    
  /* Check the correctness of written dada */
  TransferStatus1 = Buffercmp(SPI_MASTER_Buffer_Rx, SPI_SLAVE_Buffer_Tx, BufferSize);
  /* TransferStatus1 = PASSED, if the transmitted and received data
     are equal */
  /* TransferStatus1 = FAILED, if the transmitted and received data
     are different */     
    
  /* if passed ,LED2 lights */
  if((TransferStatus==PASSED)&&(TransferStatus1==PASSED))
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
  /* Enable SPI_MASTER clock and GPIO clock for SPI_MASTER and SPI_SLAVE */
  RCC_APB2PeriphClockCmd(SPI_MASTER_GPIO_CLK | SPI_SLAVE_GPIO_CLK | RCC_APB2PERIPH_GPIOD |
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

  /* Configure SPI_MASTER pins: SCK and MOSI ---------------------------------*/
  /* Configure SCK and MOSI pins as Alternate Function Push Pull */
  GPIO_InitStructure.GPIO_Pins = SPI_MASTER_PIN_SCK | SPI_MASTER_PIN_MOSI  ;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(SPI_MASTER_GPIO, &GPIO_InitStructure);    
  
  /* Configure SPI_SLAVE pins: SCK and MISO ---------------------------------*/
  /* Configure SCK and MOSI pins as Input Down */
  GPIO_ResetBits(GPIOB,GPIO_Pins_5);      
  GPIO_InitStructure.GPIO_Pins = SPI_SLAVE_PIN_SCK ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PD;    
  GPIO_Init(SPI_SLAVE_GPIO, &GPIO_InitStructure);    
  
  /* Configure MISO pin as Alternate Function Push Pull */
  GPIO_InitStructure.GPIO_Pins = SPI_SLAVE_PIN_MISO;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(SPI_SLAVE_GPIO, &GPIO_InitStructure);    
     
  /* wait for clock pin pull down */   
  while(GPIO_ReadInputDataBit(SPI_SLAVE_GPIO,SPI_SLAVE_PIN_SCK)!=RESET);
}

/**
  * @brief  Configure the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
  NVIC_InitType NVIC_InitStructure;

  /* 1 bit for pre-emption priority, 3 bits for subpriority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  /* Configure and enable SPI_MASTER interrupt -------------------------------*/
  NVIC_InitStructure.NVIC_IRQChannel = SPI_MASTER_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Configure and enable SPI_SLAVE interrupt --------------------------------*/
  NVIC_InitStructure.NVIC_IRQChannel = SPI_SLAVE_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_Init(&NVIC_InitStructure);
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
  
