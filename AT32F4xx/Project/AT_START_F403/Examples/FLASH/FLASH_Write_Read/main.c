/**
  ******************************************************************************
  * File   : FLASH/FLASH_Write_Read/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 
  
/* Includes ------------------------------------------------------------------*/
#include "flash.h"
#include "at32f4xx.h"
#include "at32_board.h"
#include <stdio.h>

/** @addtogroup AT32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup FLASH_Write_Read
  * @{
  */


#define TEST_BUFEER_SIZE          2050
#define TEST_FLASH_ADDRESS_START  (0x8000000+1024*522)
/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

u16 BufferWrite[TEST_BUFEER_SIZE];   
u16 BufferRead[TEST_BUFEER_SIZE];  
u32 Index=0;
TestStatus Buffercmp(uint16_t* pBuffer1, uint16_t* pBuffer2, uint16_t BufferLength);

int main(void)
{

  Delay_init();
  UART_Print_Init(115200);	
  AT32_LEDn_Init(LED2);				
  AT32_LEDn_Init(LED3);					

  //Fill BufferWrite data to test
  for(Index=0;Index<TEST_BUFEER_SIZE;Index++)
  {
    BufferWrite[Index]=Index;
  }
    FLASH_Write(TEST_FLASH_ADDRESS_START,BufferWrite,TEST_BUFEER_SIZE);		//Write data to flash
    FLASH_Read(TEST_FLASH_ADDRESS_START,BufferRead,TEST_BUFEER_SIZE);   	//read data from flash

  if(Buffercmp(BufferWrite,BufferRead,TEST_BUFEER_SIZE)==PASSED)        //Compare the buffer
  {
    AT32_LEDn_ON(LED2);
    AT32_LEDn_OFF(LED3);    
  }
  else
  {
    AT32_LEDn_OFF(LED2);
    AT32_LEDn_ON(LED3);  
  }

  while(1)
  {

  }
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


