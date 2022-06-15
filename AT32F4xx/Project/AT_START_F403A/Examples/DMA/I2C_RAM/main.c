 /**
 **************************************************************************
 * File   : DMA/I2C_RAM/main.c
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
  
/** @addtogroup DMA_I2C_RAM
  * @{
  */
  
/* Private typedef -----------------------------------------------------------*/
typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define I2C1_DR_Address        0x40005410
#define I2C2_DR_Address        0x40005810
#define I2C1_SLAVE_ADDRESS7    0x30
#define I2C2_SLAVE_ADDRESS7    0x30
#define BufferSize             8
#define ClockSpeed             100000

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
I2C_InitType  I2C_InitStructure = {0};
DMA_InitType  DMA_InitStructure = {0};
uint8_t I2C1_Buffer_Tx[BufferSize] = {1, 2, 3, 4, 5, 6, 7, 8};
uint8_t I2C2_Buffer_Rx[BufferSize];
uint8_t Tx_Idx = 0, Rx_Idx = 0;
volatile TestStatus TransferStatus;
    
/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
TestStatus Buffercmp(uint8_t* pBuffer, uint8_t* pBuffer1, uint16_t BufferLength);
    
/* Private functions ---------------------------------------------------------*/

/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
int main(void)
{

  /* System Clocks Configuration */
  RCC_Configuration();
       
  /* Configure the GPIO ports */
  GPIO_Configuration();
  
  /* Board.c init */
  AT32_Board_Init();
  
  /* DMA1 channel5 configuration ----------------------------------------------*/
  DMA_Reset(DMA1_Channel5);
  DMA_DefaultInitParaConfig(&DMA_InitStructure);
  DMA_InitStructure.DMA_PeripheralBaseAddr  = (uint32_t)I2C2_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr      = (uint32_t)I2C2_Buffer_Rx;
  DMA_InitStructure.DMA_Direction           = DMA_DIR_PERIPHERALSRC;
  DMA_InitStructure.DMA_BufferSize          = BufferSize;
  DMA_InitStructure.DMA_PeripheralInc       = DMA_PERIPHERALINC_DISABLE;
  DMA_InitStructure.DMA_MemoryInc           = DMA_MEMORYINC_ENABLE;
  DMA_InitStructure.DMA_PeripheralDataWidth = DMA_MEMORYDATAWIDTH_BYTE;
  DMA_InitStructure.DMA_MemoryDataWidth     = DMA_MEMORYDATAWIDTH_BYTE;
  DMA_InitStructure.DMA_Mode                = DMA_MODE_NORMAL;
  DMA_InitStructure.DMA_Priority            = DMA_PRIORITY_VERYHIGH;
  DMA_InitStructure.DMA_MTOM                = DMA_MEMTOMEM_DISABLE;
  DMA_Init(DMA1_Channel5, &DMA_InitStructure);

  /* DMA1 channel6 configuration ----------------------------------------------*/
  DMA_Reset(DMA1_Channel6);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)I2C1_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)I2C1_Buffer_Tx;
  DMA_InitStructure.DMA_Direction          = DMA_DIR_PERIPHERALDST;
  DMA_InitStructure.DMA_Priority           = DMA_PRIORITY_HIGH;
  DMA_Init(DMA1_Channel6, &DMA_InitStructure);

  /* Enable I2C1 and I2C2 ----------------------------------------------------*/
  I2C_Cmd(I2C1, ENABLE);
  I2C_Cmd(I2C2, ENABLE);

  /* I2C1 configuration ------------------------------------------------------*/
  I2C_StructInit(&I2C_InitStructure);
  I2C_InitStructure.I2C_Mode        = I2C_Mode_I2CDevice;
  I2C_InitStructure.I2C_FmDutyCycle = I2C_FmDutyCycle_2_1;
  I2C_InitStructure.I2C_OwnAddr1    = I2C1_SLAVE_ADDRESS7;
  I2C_InitStructure.I2C_Ack         = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AddrMode    = I2C_AddrMode_7bit;
  I2C_InitStructure.I2C_BitRate     = ClockSpeed;
  I2C_Init(I2C1, &I2C_InitStructure);
  /* I2C2 configuration ------------------------------------------------------*/
  I2C_InitStructure.I2C_OwnAddr1    = I2C2_SLAVE_ADDRESS7;
  I2C_Init(I2C2, &I2C_InitStructure);
  
  /*----- Transmission Phase -----*/
  /* Send I2C1 START condition */
  I2C_GenerateSTART(I2C1, ENABLE);
  /* Test on I2C1 EV5 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_START_GENERATED));  
  /* Send I2C2 slave Address for write */
  I2C_Send7bitAddress(I2C1, I2C2_SLAVE_ADDRESS7, I2C_Direction_Transmit);
  /* Test on I2C2 EV1 and clear it */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_SLAVE_ADDRESS_RECEIVER_MATCHED));  
  /* Test on I2C1 EV6 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_ADDRESS | I2C_EVENT_MASTER_TRANSMITTER));  

  /* Enable I2C2 DMA */
  I2C_DMACmd(I2C2, ENABLE);
  /* Enable I2C1 DMA */
  I2C_DMACmd(I2C1, ENABLE);

  /* Enable DMA1 Channel5 */
  DMA_ChannelEnable(DMA1_Channel5, ENABLE);
  /* Enable DMA1 Channel6 */
  DMA_ChannelEnable(DMA1_Channel6, ENABLE);

  /* DMA1 Channel5 transfer complete test */
  while(!DMA_GetFlagStatus(DMA1_FLAG_TC5));
  /* DMA1 Channel6 transfer complete test */
  while(!DMA_GetFlagStatus(DMA1_FLAG_TC6));

  /* Send I2C1 STOP Condition */
  I2C_GenerateSTOP(I2C1, ENABLE);
  /* Test on I2C2 EV4 */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_SLAVE_STOP_DETECTED)); 
  /* Clear I2C2 STOPF flag: read operation to I2C_SR1 followed by a 
  write operation to I2C_CR1 */
  (void)(I2C_GetFlagStatus(I2C2, I2C_FLAG_STOPF));
  I2C_Cmd(I2C2, ENABLE);


  /* Check if the transmitted and received data are equal */
  TransferStatus = Buffercmp(I2C1_Buffer_Tx, I2C2_Buffer_Rx, BufferSize);
  /* TransferStatus = PASSED, if the transmitted and received data 
     are the same */
  /* TransferStatus = FAILED, if the transmitted and received data 
     are different */

  if(TransferStatus == PASSED)
  {
    /* LED2.3.4 on */
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
  /* Enable DMA1 clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA1, ENABLE);
  /* Enable GPIOB clock */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);
  /* Enable I2C1 and I2C2 clock */
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_I2C1 | RCC_APB1PERIPH_I2C2, ENABLE);
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure;

  /* Configure I2C1 pins: SCL and SDA */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins     =  GPIO_Pins_6 | GPIO_Pins_7;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode     = GPIO_Mode_AF_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Configure I2C2 pins: SCL and SDA */
  GPIO_InitStructure.GPIO_Pins     = GPIO_Pins_10 | GPIO_Pins_11;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
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

  printf("Wrong parameters value: file %s on line %d\r\n", file, line);

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

