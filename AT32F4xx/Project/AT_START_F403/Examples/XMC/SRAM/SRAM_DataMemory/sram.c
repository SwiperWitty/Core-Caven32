
/* Includes ------------------------------------------------------------------*/
#include "sram.h"

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup XMC_SRAM_DataMemory
  * @brief      This file provides a set of functions needed to drive the 
  *             IS61WV51216BLL SRAM memory.
  * @{
  */ 

/** @defgroup XMC_SRAM_Private_Defines
  * @{
  */ 
/** 
  * @brief  XMC Bank 1 NOR/SRAM3  
  */
#define Bank1_SRAM3_ADDR    ((uint32_t)0x68000000)     
/**
  * @}
  */ 

/** @defgroup XMC_SRAM_Private_Functions
  * @{
  */ 

/**
  * @brief  Configures the FSMC and GPIOs to interface with the SRAM memory.
  *         This function must be called before any write/read operation
  *         on the SRAM.
  * @param  None 
  * @retval None
  */
void SRAM_Init(void)
{
    XMC_NORSRAMInitType  XMC_NORSRAMInitStructure;
    XMC_NORSRAMTimingInitType  p;
    GPIO_InitType GPIO_InitStructure; 
  
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOD | RCC_APB2PERIPH_GPIOG | RCC_APB2PERIPH_GPIOE |
                         RCC_APB2PERIPH_GPIOF, ENABLE);
  
    /*-- GPIO Configuration ------------------------------------------------------*/
    /*!< SRAM Data lines configuration */
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_0 | GPIO_Pins_1 | GPIO_Pins_8 | GPIO_Pins_9 |
                                GPIO_Pins_10 | GPIO_Pins_14 | GPIO_Pins_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure); 
  
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_7 | GPIO_Pins_8 | GPIO_Pins_9 | GPIO_Pins_10 |
                                GPIO_Pins_11 | GPIO_Pins_12 | GPIO_Pins_13 | GPIO_Pins_14 | 
                                GPIO_Pins_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
  
    /*!< SRAM Address lines configuration */
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_0 | GPIO_Pins_1 | GPIO_Pins_2 | GPIO_Pins_3 | 
                                GPIO_Pins_4 | GPIO_Pins_5 | GPIO_Pins_12 | GPIO_Pins_13 | 
                                GPIO_Pins_14 | GPIO_Pins_15;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
  
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_0 | GPIO_Pins_1 | GPIO_Pins_2 | GPIO_Pins_3 | 
                                GPIO_Pins_4 | GPIO_Pins_5;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
  
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_11 | GPIO_Pins_12 | GPIO_Pins_13; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);
   
    /*!< NOE and NWE configuration */  
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_4 |GPIO_Pins_5;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
  
    /*!< NE3 configuration */
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_10; 
    GPIO_Init(GPIOG, &GPIO_InitStructure);
  
    /*!< NBL0, NBL1 configuration */
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_0 | GPIO_Pins_1; 
    GPIO_Init(GPIOE, &GPIO_InitStructure); 
  
    /*-- FSMC Configuration ------------------------------------------------------*/
    p.XMC_AdrOpTime         = 2;
    p.XMC_AdrHoldTime       = 0;
    p.XMC_DataOpTime        = 12;
    p.XMC_IntervalBetweenOP = 0;
    p.XMC_CLKPsc            = 0;
    p.XMC_DataStableTime    = 0;
    p.XMC_Mode              = XMC_Mode_A;

    XMC_NORSRAMInitStructure.XMC_Bank                   = XMC_Bank1_NORSRAM3;
    XMC_NORSRAMInitStructure.XMC_DataAdrMux             = XMC_DataAdrMux_Disable;
    XMC_NORSRAMInitStructure.XMC_Dev                    = XMC_Dev_SRAM;
    XMC_NORSRAMInitStructure.XMC_BusType                = XMC_BusType_16b;
    XMC_NORSRAMInitStructure.XMC_EnableBurstMode        = XMC_BurstMode_Disable;
    XMC_NORSRAMInitStructure.XMC_EnableAsynWait         = XMC_AsynWait_Disable;  
    XMC_NORSRAMInitStructure.XMC_WaitSignalLv           = XMC_WaitSignalLv_Low;
    XMC_NORSRAMInitStructure.XMC_EnableBurstModeSplit   = XMC_BurstModeSplit_Disable;
    XMC_NORSRAMInitStructure.XMC_WaitSignalConfig       = XMC_WaitSignalConfig_BeforeWaitState;
    XMC_NORSRAMInitStructure.XMC_EnableWrite            = XMC_WriteOperation_Enable;
    XMC_NORSRAMInitStructure.XMC_EnableWaitSignal       = XMC_WaitSignal_Disable;
    XMC_NORSRAMInitStructure.XMC_EnableWriteTiming      = XMC_WriteTiming_Disable;
    XMC_NORSRAMInitStructure.XMC_WriteBurstSyn          = XMC_WriteBurstSyn_Disable;
    XMC_NORSRAMInitStructure.XMC_RWTimingStruct         = &p;
    XMC_NORSRAMInitStructure.XMC_WTimingStruct          = &p;

    XMC_NORSRAMInit(&XMC_NORSRAMInitStructure); 

    /*!< Enable FSMC Bank1_SRAM Bank */
    XMC_NORSRAMCmd(XMC_Bank1_NORSRAM3, ENABLE);  
}

/**
  * @brief  Writes a Half-word buffer to the FSMC SRAM memory. 
  * @param  pBuffer : pointer to buffer. 
  * @param  WriteAddr : SRAM memory internal address from which the data will be 
  *         written.
  * @param  NumHalfwordToWrite : number of half-words to write. 
  * @retval None
  */
void SRAM_WriteBuffer(uint16_t* pBuffer, uint32_t WriteAddr, uint32_t NumHalfwordToWrite)
{
  for(; NumHalfwordToWrite != 0; NumHalfwordToWrite--) /*!< while there is data to write */
  {
    /*!< Transfer data to the memory */
    *(uint16_t *) (Bank1_SRAM3_ADDR + WriteAddr) = *pBuffer++;
    
    /*!< Increment the address*/  
    WriteAddr += 2;
  }   
}

/**
  * @brief  Reads a block of data from the FSMC SRAM memory.
  * @param  pBuffer : pointer to the buffer that receives the data read from the 
  *         SRAM memory.
  * @param  ReadAddr : SRAM memory internal address to read from.
  * @param  NumHalfwordToRead : number of half-words to read.
  * @retval None
  */
void SRAM_ReadBuffer(uint16_t* pBuffer, uint32_t ReadAddr, uint32_t NumHalfwordToRead)
{
  for(; NumHalfwordToRead != 0; NumHalfwordToRead--) /*!< while there is data to read */
  {
    /*!< Read a half-word from the memory */
    *pBuffer++ = *(__IO uint16_t*) (Bank1_SRAM3_ADDR + ReadAddr);

    /*!< Increment the address*/  
    ReadAddr += 2;
  }  
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */



