#include <stdio.h>
#include "at32f4xx.h"

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup XMC_SRAM_DataMemory
  * @{
  */ 

uint32_t Tab[1024], Index;
__IO uint32_t TabAddr, MSPValue = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    for (Index = 0; Index <1024 ; Index++)
    {
        Tab[Index] =Index;
    }

    TabAddr = (uint32_t)Tab; /* should be 0x680xxxxx */

    /* Get main stack pointer value */
    MSPValue = __get_MSP(); /* should be 0x200xxxxxx */

    /* Infinite loop */
    while (1)
    {
    }
}

/**
  * @}
  */ 

/**
  * @}
  */ 



