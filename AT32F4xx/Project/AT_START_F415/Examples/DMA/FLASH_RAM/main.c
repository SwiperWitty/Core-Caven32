 /**
 **************************************************************************
 * File   : DMA/FLASH_RAM/main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
 * Brief  : Main program body
 **************************************************************************
 */
 
/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "at32_board.h"

/** @addtogroup AT32F415_StdPeriph_Examples
  * @{
  */
  
/** @addtogroup DMA_FLASH_RAM
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define BufferSize  32

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
DMA_InitType  DMA_InitStructure    = {0};
__IO uint32_t CurrDataCounterBegin = 0;
__IO uint32_t CurrDataCounterEnd   = 0x01; /* This variable should not be initialized to 0 */

TestStatus TransferStatus = FAILED;
const uint32_t SRC_Const_Buffer[BufferSize]= {0x01020304,0x05060708,0x090A0B0C,0x0D0E0F10,
                                              0x11121314,0x15161718,0x191A1B1C,0x1D1E1F20,
                                              0x21222324,0x25262728,0x292A2B2C,0x2D2E2F30,
                                              0x31323334,0x35363738,0x393A3B3C,0x3D3E3F40,
                                              0x41424344,0x45464748,0x494A4B4C,0x4D4E4F50,
                                              0x51525354,0x55565758,0x595A5B5C,0x5D5E5F60,
                                              0x61626364,0x65666768,0x696A6B6C,0x6D6E6F70,
                                              0x71727374,0x75767778,0x797A7B7C,0x7D7E7F80};
uint32_t DST_Buffer[BufferSize];

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void NVIC_Configuration(void);
TestStatus Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength);
    
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{ 
  /* System Clocks Configuration */
  RCC_Configuration();
       
  /* NVIC configuration */
  NVIC_Configuration();
  
  /* Board.c init */
  AT32_Board_Init();
  
  /* DMA1 channel6 configuration */
  DMA_Reset(DMA1_Channel6);
  DMA_DefaultInitParaConfig(&DMA_InitStructure);
  DMA_InitStructure.DMA_PeripheralBaseAddr  = (uint32_t)SRC_Const_Buffer;
  DMA_InitStructure.DMA_MemoryBaseAddr      = (uint32_t)DST_Buffer;
  DMA_InitStructure.DMA_Direction           = DMA_DIR_PERIPHERALSRC;
  DMA_InitStructure.DMA_BufferSize          = BufferSize;
  DMA_InitStructure.DMA_PeripheralInc       = DMA_PERIPHERALINC_ENABLE;
  DMA_InitStructure.DMA_MemoryInc           = DMA_MEMORYINC_ENABLE;
  DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_WORD;
  DMA_InitStructure.DMA_MemoryDataWidth     = DMA_MEMORYDATAWIDTH_WORD;
  DMA_InitStructure.DMA_Mode                = DMA_MODE_NORMAL;
  DMA_InitStructure.DMA_Priority            = DMA_PRIORITY_HIGH;
  DMA_InitStructure.DMA_MTOM                = DMA_MEMTOMEM_ENABLE;
  DMA_Init(DMA1_Channel6, &DMA_InitStructure);

  /* Enable DMA1 Channel6 Transfer Complete interrupt */
  DMA_INTConfig(DMA1_Channel6, DMA_INT_TC, ENABLE);

  /* Get Current Data Counter value before transfer begins */
  CurrDataCounterBegin = DMA_GetCurrDataCounter(DMA1_Channel6);

  /* Enable DMA1 Channel6 transfer */
  DMA_ChannelEnable(DMA1_Channel6, ENABLE);

  /* Wait the end of transmission */
  while (CurrDataCounterEnd != 0)
  {
  }
  
  /* Check if the transmitted and received data are equal */
  TransferStatus = Buffercmp(SRC_Const_Buffer, DST_Buffer, BufferSize);
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
  /* Enable peripheral clocks ------------------------------------------------*/
  /* Enable DMA1 clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA1, ENABLE);
}

/**
  * @brief  Configure the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
  NVIC_InitType NVIC_InitStructure = {0};
  
  /* Enable DMA1 channel6 IRQ Channel */
  NVIC_InitStructure.NVIC_IRQChannel                   = DMA1_Channel6_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer, pBuffer1: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer identical to pBuffer1
  *         FAILED: pBuffer differs from pBuffer1
  */
TestStatus Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength)
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

