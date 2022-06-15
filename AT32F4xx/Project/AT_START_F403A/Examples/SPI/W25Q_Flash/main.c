/**
  ******************************************************************************
  * File   : SPI/SPI_W25Q_Flash/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "at32f4xx.h"
#include "at32_board.h"
#include "spi_flash.h"

/** @addtogroup AT32F403A_StdPeriph_Examples
  * @{
  */

/** @addtogroup SPI_W25Q_Flash
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define FLASH_TEST_ADDR   0x1000

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t Buffer_Tx[BUF_SIZE];
uint8_t Buffer_Rx[BUF_SIZE];

volatile TestStatus TransferStatus = FAILED;

/* Private function prototypes -----------------------------------------------*/
void TxDataInit(void);
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);

/* Private functions ---------------------------------------------------------*/
void TxDataInit(void)
{
  uint32_t i;
  for(i=0; i<BUF_SIZE; i++)
  {
    Buffer_Tx[i] = i;
  }
}

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
  uint32_t i, Id;

  TxDataInit();

  /* config at-start-board */
  UART_Print_Init(115200);

  AT32_Board_Init();

  SpiFlash_Init();

  Id = SpiFlash_ReadID();
  if(Id!=W25Q128)
  {
    printf("Spi flash init error!\r\n");
    for(i=0;i<50;i++)
    {
     AT32_LEDn_Toggle(LED2);
     AT32_LEDn_Toggle(LED3);
     Delay_ms(100);
    }
    return 1;
  }
  else
  {
    printf("Check ID success! ID: %x\r\n", Id);
  }

  /* Erase sector */
  SpiFlash_Erase_Sector(FLASH_TEST_ADDR / SPIF_SECTOR_SIZE);
  /* Write Data */
  SpiFlash_Write(Buffer_Tx, FLASH_TEST_ADDR, BUF_SIZE);
  /* Read Data */
  SpiFlash_Read(Buffer_Rx, FLASH_TEST_ADDR, BUF_SIZE);

  /* Printf read data */
  printf("Read Data: ");
  for(i=0; i<BUF_SIZE; i++)
  {
    printf("%x ", Buffer_Rx[i]);
  }
  printf("\r\n");

  /* Check the correctness of written data */
  TransferStatus = Buffercmp(Buffer_Tx, Buffer_Rx, BUF_SIZE);

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
  
