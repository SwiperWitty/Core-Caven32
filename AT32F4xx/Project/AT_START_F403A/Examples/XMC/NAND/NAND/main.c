/**
  **************************************************************************
  * File   : main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  **************************************************************************
  */
 

/* Includes ------------------------------------------------------------------*/
#include "at32f403_nand.h"
#include "at32f4xx.h"
#include "at32_board.h"

/** @addtogroup AT32F403A_StdPeriph_Examples
  * @{
  */
  
/** @addtogroup XMC_NAND
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BUFFER_SIZE         0x400
#define NAND_AT_MakerID     0xAD
#define NAND_AT_DeviceID    0xF1

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
NAND_IDTypeDef NAND_ID;
NAND_ADDRESS WriteReadAddr;
uint8_t TxBuffer[BUFFER_SIZE], RxBuffer[BUFFER_SIZE];
uint32_t PageNumber = 2, WriteReadStatus = 0, status= 0;
uint32_t j = 0;

/* Private function prototypes -----------------------------------------------*/
void Fill_Buffer(uint8_t *pBuffer, uint16_t BufferLenght, uint32_t Offset);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief   Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  /* Enable the XMC Clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_XMC, ENABLE);
    
  /* LED Initalization */
  AT32_Board_Init();    
  
  /* XMC Initialization */
  NAND_Init();

  /* NAND read ID command */
  NAND_ReadID(&NAND_ID);

  /* Verify the NAND ID */
  /* NAND support:Spansion:S34ML01G100TFI003  Micron:MT29F16G08CBACA  Sandisk:SDTNQGAMA-008G  Toshiba:TC58TEG6DDKTA00   SAMSUNG:K9GAG08U0E-SCB0   Hynix:H27U1G8F2CTR
                  ID      :0x01F1001D         ID    :0x2C48044A       ID     :0x45DE9493      ID     :0x98DE9493        ID     :0xECD58472        ID   :0xADF1801D
  */  
  if((NAND_ID.Maker_ID == NAND_AT_MakerID) && (NAND_ID.Device_ID == NAND_AT_DeviceID))    
  {

    /* NAND memory address to write to */
    WriteReadAddr.Zone = 0x00;
    WriteReadAddr.Block = 0x00;
    WriteReadAddr.Page = 0x00;

    /* Erase the NAND first Block */
    status = NAND_EraseBlock(WriteReadAddr);

    /* Write data to XMC NAND memory */
    /* Fill the buffer to send */
    Fill_Buffer(TxBuffer, BUFFER_SIZE , 0x66);
 
    status = NAND_WriteSmallPage(TxBuffer, WriteReadAddr, PageNumber);

    /* Read back the written data */
    status = NAND_ReadSmallPage (RxBuffer, WriteReadAddr, PageNumber);
   
    /* Verify the written data */
    for(j = 0; j < BUFFER_SIZE; j++)
    {
      if(TxBuffer[j] != RxBuffer[j])
      {
        WriteReadStatus++;
      }
    }
    
    /* LED1 and LED0 indicate that whether there is an error during writing and reading Nand flash */ 
    if(WriteReadStatus==0)
    {
        AT32_LEDn_ON(LED2);
    }
    else
    {
        AT32_LEDn_ON(LED3);
    }
  }
  
  /* LED0 and LED1 both turn on means device error (ID not correct) */
  else
  {
      AT32_LEDn_ON(LED2);
      AT32_LEDn_ON(LED3);
  }

  while(1)
  {
  }
}

/**
  * @brief  Fill the buffer
  * @param  pBuffer: pointer on the Buffer to fill
  * @param  BufferSize: size of the buffer to fill
  * @param  Offset: first value to fill on the Buffer
  */
void Fill_Buffer(uint8_t *pBuffer, uint16_t BufferLenght, uint32_t Offset)
{
  uint16_t IndexTmp = 0;

  /* Put in global buffer same values */
  for (IndexTmp = 0; IndexTmp < BufferLenght; IndexTmp++ )
  {
    pBuffer[IndexTmp] = IndexTmp + Offset;
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

  /* Infinite loop */
  while (1)
  {
  }
}

#endif

/**
  * @}
  */

/**
  * @}
  */

