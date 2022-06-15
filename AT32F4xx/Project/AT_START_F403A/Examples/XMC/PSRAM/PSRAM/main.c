#include <stdio.h>
#include "at32f4xx.h"
#include "psram.h"
#include "at32_board.h"
/** @addtogroup AT32F403A_StdPeriph_Examples
  * @{
  */

/** @addtogroup XMC_PSRAM
  * @{
  */ 

#define BUFFER_SIZE        0x400
#define WRITE_READ_ADDR    0x8000

uint16_t TxBuffer[BUFFER_SIZE];
uint16_t RxBuffer[BUFFER_SIZE];
uint32_t WriteReadStatus = 0, Index = 0;

void Fill_Buffer(uint16_t *pBuffer, uint16_t BufferLenght, uint32_t Offset);

int main(void)
{
    /* Initialize Leds */
    AT32_Board_Init();
  
    /* Write/read to/from XMC PSRAM memory  *************************************/
    /* Enable the XMC Clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_XMC, ENABLE);
  
    /* Configure XMC Bank1 NOR/SRAM1 */
    PSRAM_Init();

    /* Write data to XMC PSRAM memory */
    /* Fill the buffer to send */
    Fill_Buffer(TxBuffer, BUFFER_SIZE, 0x3212);
    PSRAM_WriteBuffer(TxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);


    /* Read data from XMC SRAM memory */
    PSRAM_ReadBuffer(RxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);  

    /* Read back SRAM memory and check content correctness */   
    for (Index = 0x00; (Index < BUFFER_SIZE) && (WriteReadStatus == 0); Index++)
    {
        if (RxBuffer[Index] != TxBuffer[Index])
        {
            WriteReadStatus = Index + 1;
        }
    }

    if (WriteReadStatus == 0)
    { 
        /* OK */
        /* Turn on LED2 */
        AT32_LEDn_ON(LED2);
    }
    else
    { 
        /* KO */
        /* Turn on LED3 */
        AT32_LEDn_ON(LED3);
    }

    while (1)
    {
    }
}

/**
  * @brief  Fill the global buffer
  * @param  pBuffer: pointer on the Buffer to fill
  * @param  BufferSize: size of the buffer to fill
  * @param  Offset: first value to fill on the Buffer
  */
void Fill_Buffer(uint16_t *pBuffer, uint16_t BufferLenght, uint32_t Offset)
{
    uint16_t IndexTmp = 0;

    /* Put in global buffer same values */
    for (IndexTmp = 0; IndexTmp < BufferLenght; IndexTmp++ )
    {
        pBuffer[IndexTmp] = IndexTmp + Offset;
    }
}

/**
  * @}
  */ 

/**
  * @}
  */ 

