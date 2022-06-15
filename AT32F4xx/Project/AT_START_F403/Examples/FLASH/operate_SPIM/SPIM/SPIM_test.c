/**
  ******************************************************************************
  * File   : FLASH/operate_SPIM/SPIM/SPIM_test.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file contains the function SPIM_test used to test ext.flash
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "SPIM_test.h"
#include "at32f4xx.h"  

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup FLASH_Operate_SPIM
  * @{
  */

/* Private variables ---------------------------------------------------------*/
u8 WriteBuffer[4096];  ///The content writed to ext.flash 
u8 ReadBuffer[4096];   ///The content read from ext.flash  		

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Configures the ext.flash
  * @param  None
  * @retval None
  */
void FLASH_InitExtFlash(void)
{
  GPIO_InitType GPIO_InitStructure;
  
  /* Enable ext.flash GPIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO|RCC_APB2PERIPH_GPIOA|RCC_APB2PERIPH_GPIOB, ENABLE);
  
  /* Configure ext.flash pin */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_8|GPIO_Pins_11|GPIO_Pins_12;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_1|GPIO_Pins_6|GPIO_Pins_7;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_PinsRemapConfig(GPIO_Remap_EXT_FLASH, ENABLE);
  
  /* In this example, use on-board EN25QH128A as ext.flash */
  FLASH->B3SEL = FLASH_SPIM_TYPE2;
  
  /* Unlock the ext.flash program erase controller */  
  while (BIT_READ(FLASH->STS3, FLASH_STS_BSY));
  FLASH->FCKEY3 = FLASH_KEY1;
  FLASH->FCKEY3 = FLASH_KEY2;
  while (BIT_READ(FLASH->CTRL3, FLASH_CTRL_LCK));

  /* If the data written to ext.flash need to be scrambled, please specify the scrambled range */
  FLASH->DA = 0;
  
  return;
}

/**
  * @brief  Read one page from a specified address in ext.flash
  * @param  adr: specifies the page starting address
  * @param  sz: page size in bytes
  * @param  buf: the content read from ext.flash
  * @retval None
  */
void ReadPage (unsigned long adr, unsigned long sz, unsigned char *buf)
{
  while (sz)
  {
    *(UINT32 *)buf = *(UINT32 *)adr;    
    sz -= sizeof(UINT32);
    adr += sizeof(UINT32);
    buf += sizeof(UINT32);
  }   
}

/**
  * @brief  To test ext.flash
  * @param  None
  * @retval None
  */
void SPIM_test(void)
{
  u16 i=0;
  
  /* Configures the ext.flash */
  printf("init SPIM.\r\n");   
  FLASH_InitExtFlash();
  
  /* Fill the content to be writed to ext.flash */
  for(i=0;i<SPIM_PAGE_SIZE;i++)
  {
    WriteBuffer[i]=i%256;
  }
  
  /* Erases an ext.flash page */
  printf("erase one page.\r\n");
  FLASH_ErasePage(SPIM_TEST_ADDR);
  
  /* Read an ext.flash page */
  memset(ReadBuffer,0,SPIM_PAGE_SIZE);
  ReadPage(SPIM_TEST_ADDR, SPIM_PAGE_SIZE, ReadBuffer);
  
  /* Check if the desired page are erased */
  for(i=0;i<SPIM_PAGE_SIZE;i++)
  {
    if(ReadBuffer[i]!=0xff)
    {
      printf("operate SPIM fail.\r\n");
      return;
    }    
  }
  
  /* Program an ext.flash page */
  printf("write one page.\r\n");
  i=0;
  while(i<SPIM_PAGE_SIZE)
  {
    FLASH_ProgramWord (SPIM_TEST_ADDR+i,*(u32 *)(WriteBuffer+i)); 
    i=i+4;  
  }
  
  /* Read an ext.flash page */
  printf("read one page.\r\n");
  memset(ReadBuffer,0,SPIM_PAGE_SIZE);
  ReadPage(SPIM_TEST_ADDR, SPIM_PAGE_SIZE, ReadBuffer);
  
  /* Check if reading result and writing content are the same */
  printf("compare the WriteBuffer/ReadBuffer.\r\n");
  for(i=0;i<SPIM_PAGE_SIZE;i++)
  {
    if(ReadBuffer[i]!=WriteBuffer[i])
    {
      /* Print the ext.flash testing result */
      printf("test SPIM fail.\r\n");
      return;
    }    
  }

  /* Print the ext.flash testing result */
  printf("test SPIM success!\r\n");
}


/**
  * @}
  */ 

/**
  * @}
  */ 
  
