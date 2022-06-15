 /**
 **************************************************************************
 * File   : DMA/XMC/main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
 * Brief  : Main program body
 **************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "XMC_sram.h"
#include "at32f4xx.h"
#include "at32_board.h"

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */
  
/** @addtogroup DMA_XMC
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define BufferSize  32
#define Bank1_SRAM3_ADDR    ((uint32_t)0x68000000)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
DMA_InitType    DMA_InitStructure = {0};
volatile TestStatus TransferStatus;
const uint32_t SRC_Const_Buffer[BufferSize]= {
                            0x01020304,0x05060708,0x090A0B0C,0x0D0E0F10,
                            0x11121314,0x15161718,0x191A1B1C,0x1D1E1F20,
                            0x21222324,0x25262728,0x292A2B2C,0x2D2E2F30,
                            0x31323334,0x35363738,0x393A3B3C,0x3D3E3F40,
                            0x41424344,0x45464748,0x494A4B4C,0x4D4E4F50,
                            0x51525354,0x55565758,0x595A5B5C,0x5D5E5F60,
                            0x61626364,0x65666768,0x696A6B6C,0x6D6E6F70,
                            0x71727374,0x75767778,0x797A7B7C,0x7D7E7F80};
uint8_t DST_Buffer[4*BufferSize];
uint32_t Idx = 0;

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
TestStatus Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
       
  /* System Clocks Configuration */
  RCC_Configuration();

  /* XMC for SRAM and SRAM pins configuration */
  SRAM_Init();
  
  /* Board.c init */
  AT32_Board_Init();
  
  /* Write to XMC -----------------------------------------------------------*/
  /* DMA2 channel5 configuration */
  DMA_Reset(DMA2_Channel5);
  DMA_DefaultInitParaConfig(&DMA_InitStructure);
  DMA_InitStructure.DMA_PeripheralBaseAddr  = (uint32_t)SRC_Const_Buffer;
  DMA_InitStructure.DMA_MemoryBaseAddr      = (uint32_t)Bank1_SRAM3_ADDR;    
  DMA_InitStructure.DMA_Direction           = DMA_DIR_PERIPHERALSRC;
  DMA_InitStructure.DMA_BufferSize          = 32;
  DMA_InitStructure.DMA_PeripheralInc       = DMA_PERIPHERALINC_ENABLE;
  DMA_InitStructure.DMA_MemoryInc           = DMA_MEMORYINC_ENABLE;
  DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_WORD;
  DMA_InitStructure.DMA_MemoryDataWidth     = DMA_MEMORYDATAWIDTH_WORD;
  DMA_InitStructure.DMA_Mode                = DMA_MODE_NORMAL;
  DMA_InitStructure.DMA_Priority            = DMA_PRIORITY_HIGH;
  DMA_InitStructure.DMA_MTOM                = DMA_MEMTOMEM_ENABLE;
  DMA_Init(DMA2_Channel5, &DMA_InitStructure);

  /* Enable DMA2 channel5 */
  DMA_ChannelEnable(DMA2_Channel5, ENABLE);

  /* Check if DMA2 channel5 transfer is finished */
  while(!DMA_GetFlagStatus(DMA2_FLAG_TC5));

  /* Clear DMA2 channel5 transfer complete flag bit */
  DMA_ClearFlag(DMA2_FLAG_TC5);

  /* Read from XMC ----------------------------------------------------------*/
  /* Destination buffer initialization */ 
  for(Idx=0; Idx<128; Idx++) DST_Buffer[Idx]=0;

  /* DMA1 channel3 configuration */
  DMA_Reset(DMA1_Channel3);
  DMA_InitStructure.DMA_PeripheralBaseAddr  = (uint32_t)Bank1_SRAM3_ADDR;  
  DMA_InitStructure.DMA_MemoryBaseAddr      = (uint32_t)DST_Buffer;
  DMA_InitStructure.DMA_Direction           = DMA_DIR_PERIPHERALSRC;
  DMA_InitStructure.DMA_BufferSize          = 128;
  DMA_InitStructure.DMA_PeripheralInc       = DMA_PERIPHERALINC_ENABLE;
  DMA_InitStructure.DMA_MemoryInc           = DMA_MEMORYINC_ENABLE;
  DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_BYTE;
  DMA_InitStructure.DMA_MemoryDataWidth     = DMA_MEMORYDATAWIDTH_BYTE;
  DMA_InitStructure.DMA_Mode                = DMA_MODE_NORMAL;
  DMA_InitStructure.DMA_Priority            = DMA_PRIORITY_HIGH;
  DMA_InitStructure.DMA_MTOM                = DMA_MEMTOMEM_ENABLE;
  DMA_Init(DMA1_Channel3, &DMA_InitStructure);

  /* Enable DMA1 channel3 */
  DMA_ChannelEnable(DMA1_Channel3, ENABLE);

  /* Check if DMA1 channel3 transfer is finished */
  while(!DMA_GetFlagStatus(DMA1_FLAG_TC3));

  /* Clear DMA1 channel3 transfer complete flag bit */
  DMA_ClearFlag(DMA1_FLAG_TC3);

  /* Check if the transmitted and received data are equal */
  TransferStatus = Buffercmp(SRC_Const_Buffer, (uint32_t*)DST_Buffer, BufferSize);
  /* TransferStatus = PASSED, if the transmitted and received data 
     are the same */
  /* TransferStatus = FAILED, if the transmitted and received data 
     are different */

  while (1)
  {
    /*Delay 1000ms */
    Delay_ms(1000);
    
    /* Toggle LED4 */
    AT32_LEDn_Toggle(LED4);
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
  /* DMA1 and DMA2 clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA1 | RCC_AHBPERIPH_DMA2, ENABLE);
  /* XMC clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_XMC, ENABLE);
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


