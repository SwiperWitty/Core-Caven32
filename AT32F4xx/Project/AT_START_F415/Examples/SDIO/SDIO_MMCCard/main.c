/**
  ******************************************************************************
  * File   : SDIO/SDIO_MMCCard/main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : MMC card function test.
  ******************************************************************************
  */

/* Includes -----------------------------------------------------------------*/
#include <stdio.h>
#include <stdbool.h>
#include "string.h"
#include "at32f4xx.h"
#include "at32_sdio.h"
#include "at32_board.h"

/** @addtogroup AT32F415_StdPeriph_Examples
  * @{
  */

/** @addtogroup SDIO_MMCCard
  * @{
  */ 

/* Private macro -------------------------------------------------------------*/
#define BLOCK_SIZE           (512) ///< Block Size in Bytes
#define BLOCKS_NUMBER        (10)  ///< Multiple Blocks Test Numbers
#define MULTI_BUFFER_SIZE    (BLOCK_SIZE * BLOCKS_NUMBER) ///< Multiple Blocks Test Size
#define STREAM_BUFFER_SIZE   (2048) ///< MMC Card Stream Test Buffer Size

/* Private variables ---------------------------------------------------------*/
static uint8_t SBlock_TBuffer[BLOCK_SIZE], SBlock_RBuffer[BLOCK_SIZE]; ///< Single Block TX and RX Buffer
static uint8_t MBlock_TBuffer[MULTI_BUFFER_SIZE], MBlock_RBuffer[MULTI_BUFFER_SIZE]; ///< Multiple Blocks TX and RX Buffer
static uint8_t Stream_TBuffer[STREAM_BUFFER_SIZE], Stream_RBuffer[STREAM_BUFFER_SIZE]; ///< MMC Card Stream TX and RX Buffer

/* Private typedef -----------------------------------------------------------*/
/**
 * Test Result
 */
typedef enum
{
  TEST_FAIL = 0, ///< fail
  TEST_SUCCESS,  ///< success
} TEST_RESULT;

/* Private function prototypes -----------------------------------------------*/
static TEST_RESULT SD_SingleBlockTest(void);
static TEST_RESULT SD_MultipleBlocksTest(void);
static TEST_RESULT MMC_StreamTest(void);
static bool Memory_Compare(uint8_t* pMemory1, uint8_t* pMemory2, uint32_t Size);
static void SD_Test_Error(void);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  SD Card single block read/write test.
  * @param  None
  * @retval TEST_FAIL:    Fail.
  *         TEST_SUCCESS: Success.
  */
static TEST_RESULT SD_SingleBlockTest(void)
{
  SD_Error Status = SD_OK;
  uint8_t bus_width;

  /* Test 1-bit width */
  for(bus_width = 0; bus_width < 1; bus_width++)
  {
    memset(SBlock_TBuffer, bus_width + 0xAB, BLOCK_SIZE);
    memset(SBlock_RBuffer, 0, BLOCK_SIZE);

    /* Set data width */
    Status = SD_EnableWideBusOperation(bus_width);

    if (Status != SD_OK)
    {
      return TEST_FAIL;
    }

    /* Write block of 512 bytes on address 0 */
    Status = SD_WriteBlock(SBlock_TBuffer, 0x00, BLOCK_SIZE);

    if (Status != SD_OK)
    {
      return TEST_FAIL;
    }

    /* Read block of 512 bytes from address 0 */
    Status = SD_ReadBlock(SBlock_RBuffer, 0x00, BLOCK_SIZE);

    if (Status != SD_OK)
    {
      return TEST_FAIL;
    }

    if(!Memory_Compare(SBlock_TBuffer, SBlock_RBuffer, BLOCK_SIZE))
    {
      return TEST_FAIL;
    }
  }

  return TEST_SUCCESS;

}

/**
  * @brief  SD Card multiple blocks read/write test.
  * @param  None
  * @retval TEST_FAIL:    Fail.
  *         TEST_SUCCESS: Success.
  */
static TEST_RESULT SD_MultipleBlocksTest(void)
{
  SD_Error Status = SD_OK;
  uint8_t bus_width;

  /* Test 1-bit width */
  for(bus_width = 0; bus_width < 1; bus_width++)
  {
    memset(MBlock_TBuffer, bus_width + 0x3C, MULTI_BUFFER_SIZE);
    memset(MBlock_RBuffer, 0, MULTI_BUFFER_SIZE);

    /* Set data width */
    Status = SD_EnableWideBusOperation(bus_width);

    if (Status != SD_OK)
    {
      return TEST_FAIL;
    }

    /* Write multiple block of many bytes on address 0 */
    Status = SD_WriteMultiBlocks(MBlock_TBuffer, 0x00, BLOCK_SIZE, BLOCKS_NUMBER);

    if (Status != SD_OK)
    {
      return TEST_FAIL;
    }

    /* Read block of many bytes from address 0 */
    Status = SD_ReadMultiBlocks(MBlock_RBuffer, 0x00, BLOCK_SIZE, BLOCKS_NUMBER);

    if (Status != SD_OK)
    {
      return TEST_FAIL;
    }

    /* Check the correctness of written data */
    if(!Memory_Compare(MBlock_TBuffer, MBlock_RBuffer, MULTI_BUFFER_SIZE))
    {
      return TEST_FAIL;
    }
  }

  return TEST_SUCCESS;

}

/**
  * @brief  MMC Card data stream read/write test.
  * @param  None
  * @retval TEST_FAIL:    Fail.
  *         TEST_SUCCESS: Success.
  */
static TEST_RESULT MMC_StreamTest(void)
{
  SD_Error Status = SD_OK;

  memset(Stream_TBuffer, 0x5A, STREAM_BUFFER_SIZE);
  memset(Stream_RBuffer, 0, STREAM_BUFFER_SIZE);

  /* Set 1-bit data width */
  Status = SD_EnableWideBusOperation(SDIO_BusWide_1b);

  if (Status != SD_OK)
  {
    return TEST_FAIL;
  }

  /* Write stream of many bytes on address 0 */
  Status = MMC_StreamWrite(Stream_TBuffer, 0x00, STREAM_BUFFER_SIZE);

  if (Status != SD_OK)
  {
    return TEST_FAIL;
  }

  /* Read stream of many bytes from address 0 */
  Status = MMC_StreamRead(Stream_RBuffer, 0x00, STREAM_BUFFER_SIZE);

  if (Status != SD_OK)
  {
    return TEST_FAIL;
  }

  /* Check the correctness of written data */
  if(!Memory_Compare(Stream_TBuffer, Stream_RBuffer, STREAM_BUFFER_SIZE))
  {
    return TEST_FAIL;
  }

  return TEST_SUCCESS;
}

/**
  * @brief  Compare if two memory has the same values.
  * @param  pMemory1: The first memory to be compared.
  * @param  pMemory2: The second memory to be compared.
  * @retval true:The two memory has the same values
  *         false: The two memory has the diffent values.
  */
static bool Memory_Compare(uint8_t* pMemory1, uint8_t* pMemory2, uint32_t Size)
{
  while (Size--)
  {
    if (*pMemory1 != *pMemory2)
    {
      return false;
    }

    pMemory1++;
    pMemory2++;
  }

  return true;
}

/**
  * @brief  LED2 ON_OFF every 300ms for SD test failed.
  * @param  None
  * @retval None
  */
static void SD_Test_Error(void)
{
  AT32_LEDn_ON(LED2);
  Delay_ms(300);
  AT32_LEDn_OFF(LED2);
  Delay_ms(300);
}

/**
  * @brief  Configures SDIO1 IRQ channel.
  * @param  None
  * @retval None
  */
static void NVIC_Configuration(void)
{
  NVIC_InitType NVIC_InitStructure = {0};

  /* Configure the NVIC Preemption Priority Bits */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}

/* Gloable functions ---------------------------------------------------------*/
/**
  * @brief  Main Function.
  * @param  None
  * @retval None
  */
int main(void)
{
  /* --------BSP Init --------------------------------------------*/
  UART_Print_Init(115200);
  AT32_Board_Init();
  NVIC_Configuration();

  /* --------SDIO1 Init ------------------------------------------*/
  if(SD_Init() != SD_OK)
  {
    printf("MMC Card Init Failed.\r\n");

    /* 初始化失败,LED2闪烁 */
    while(1)
    {
      SD_Test_Error();
    }
  }

  printf("MMC Card Init OK.\r\n");

  /* --------MMC Card Single Block Test ----------------------------*/
  if(SD_SingleBlockTest() != TEST_SUCCESS)
  {
    printf("MMC Card Single Block Test Failed.\r\n");

    /* Single Block Test Fail, LED2闪烁 */
    while(1)
    {
      SD_Test_Error();
    }
  }

  printf("MMC Card Single Block Test Pass.\r\n");

  /* --------MMC Card Multiple Blocks Test -------------------------*/
  if(SD_MultipleBlocksTest() != TEST_SUCCESS)
  {
    printf("MMC Card Multiple Blocks Test Failed.\r\n");

    /* Multiple Blocks Test Fail, LED2闪烁 */
    while(1)
    {
      SD_Test_Error();
    }
  }

  printf("MMC Card Multiple Blocks Test Pass.\r\n");

  /* --------MMC Card Data Stream Test -----------------------------*/
  if(MMC_StreamTest() != TEST_SUCCESS)
  {
    printf("MMC Card Data Stream Test Failed.\r\n");

    /* Data Stream Test Fail, LED2闪烁 */
    while(1)
    {
      SD_Test_Error();
    }
  }

  printf("MMC Card Data Stream Test Pass.\r\n");

  /* --------All Tests Pass, LED3 and LED4闪烁 ---------------------*/
  while(1)
  {
    AT32_LEDn_ON(LED3);
    AT32_LEDn_ON(LED4);
    Delay_ms(300);
    AT32_LEDn_OFF(LED3);
    AT32_LEDn_OFF(LED4);
    Delay_ms(300);
  }
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


