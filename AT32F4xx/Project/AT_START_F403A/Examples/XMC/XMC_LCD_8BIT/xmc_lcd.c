/* Includes ------------------------------------------------------------------*/
#include "xmc_lcd.h"
#include "picture.h"

/** @addtogroup AT32F403A_StdPeriph_Examples
  * @{
  */

/** @addtogroup XMC_LCD_8BIT
  * @brief      This file provides a set of functions needed to drive the 
  *             LCD(drive IC:ST7789v).
  * @{
  */ 
  
/** @defgroup XMC_LCD_8BIT_Private_Defines
  * @{
  */ 
  
/**
  * @brief  Configures the FSMC and GPIOs to interface with the LCD.
  *         This function must be called before any write/read operation
  *         on the LCD.
  * @param  None 
  * @retval None
  */
void XMC_init(void)
{
  XMC_NORSRAMInitType  XMC_NORSRAMInitStruct;
  XMC_NORSRAMTimingInitType  XMC_NORSRAMTimingInitStruct = {0};
  GPIO_InitType GPIO_InitStructure;
  
  /* Enable the GPIO Clock */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB|RCC_APB2PERIPH_GPIOC | RCC_APB2PERIPH_GPIOA | RCC_APB2PERIPH_GPIOD |RCC_APB2PERIPH_AFIO, ENABLE);
  /* Enable the XMC Clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_XMC, ENABLE);
  /* REMAP */
  GPIO_PinsRemapConfig(AFIO_MAP7_XMC_0001,ENABLE);
  /*-- GPIO Configuration ------------------------------------------------------*/
  /*!< LCD Data lines configuration */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_6 | GPIO_Pins_11 | GPIO_Pins_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure); 
  
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_2 | GPIO_Pins_3 | GPIO_Pins_4 | GPIO_Pins_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
   
  /*!< LCD Reset lines configuration */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
  
  /*!< LCD RS/CS/WR/RD lines configuration */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure); 
  
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_3 | GPIO_Pins_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure); 
  
  
  /*-- FSMC Configuration ------------------------------------------------------*/
  XMC_NORSRAMTimingInitStruct.XMC_AdrHoldTime = 0x00;
  XMC_NORSRAMTimingInitStruct.XMC_AdrOpTime = 0x02;
  XMC_NORSRAMTimingInitStruct.XMC_CLKPsc = 0x00;
  XMC_NORSRAMTimingInitStruct.XMC_DataOpTime = 0x02;
  XMC_NORSRAMTimingInitStruct.XMC_DataStableTime = 0x00;
  XMC_NORSRAMTimingInitStruct.XMC_IntervalBetweenOP = 0x00;
  XMC_NORSRAMTimingInitStruct.XMC_Mode = XMC_Mode_A;
  
  XMC_NORSRAMInitStruct.XMC_Bank = XMC_Bank1_NORSRAM4;
  XMC_NORSRAMInitStruct.XMC_BusType = XMC_BusType_8b;
  XMC_NORSRAMInitStruct.XMC_DataAdrMux = XMC_DataAdrMux_Disable;
  XMC_NORSRAMInitStruct.XMC_Dev = XMC_Dev_SRAM;
  XMC_NORSRAMInitStruct.XMC_EnableAsynWait = XMC_AsynWait_Disable;
  XMC_NORSRAMInitStruct.XMC_EnableBurstMode = XMC_BurstMode_Disable;
  XMC_NORSRAMInitStruct.XMC_EnableBurstModeSplit = XMC_BurstModeSplit_Disable;
  XMC_NORSRAMInitStruct.XMC_EnableWaitSignal = XMC_WaitSignal_Disable;
  XMC_NORSRAMInitStruct.XMC_EnableWrite = XMC_WriteOperation_Enable;
  XMC_NORSRAMInitStruct.XMC_EnableWriteTiming = XMC_WriteTiming_Enable;
  XMC_NORSRAMInitStruct.XMC_WaitSignalConfig = XMC_WaitSignalConfig_BeforeWaitState;
  XMC_NORSRAMInitStruct.XMC_WaitSignalLv = XMC_WaitSignalLv_Low;
  XMC_NORSRAMInitStruct.XMC_WriteBurstSyn = XMC_WriteBurstSyn_Disable;
  XMC_NORSRAMInitStruct.XMC_WTimingStruct = &XMC_NORSRAMTimingInitStruct;
  XMC_NORSRAMInitStruct.XMC_RWTimingStruct = &XMC_NORSRAMTimingInitStruct;
  XMC_NORSRAMInit(&XMC_NORSRAMInitStruct);
  /*!< Enable FSMC Bank1_SRAM Bank */
  XMC_NORSRAMCmd(XMC_Bank1_NORSRAM4, ENABLE);
}

/**
  * @brief  Configures the LCD.
  *         This function must be called before any write/read operation
  *         on the LCD.
  * @param  None 
  * @retval None
  */
void LCD_init(void)
{
  /* Init XMC */
  XMC_init();
  Delay_ms(50);
  /* Reset LCD */
  LCD_RESET_HIGH;
  Delay_ms(1);
  LCD_RESET_LOW;
  Delay_ms(10);
  LCD_RESET_HIGH;
  Delay_ms(120);
  
  LCD_WR_COMMAND(0x36);
  LCD_WR_DATA(0x00); 
  LCD_WR_COMMAND(0x3a); 
  LCD_WR_DATA(0x55); 
  /* 320*240 */
  LCD_WR_COMMAND(0x2a);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x01);
  LCD_WR_DATA(0x3f);
  LCD_WR_COMMAND(0x2b);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xef);
  
  LCD_WR_COMMAND(0xb2); 
  LCD_WR_DATA(0x0c); 
  LCD_WR_DATA(0x0c); 
  LCD_WR_DATA(0x00); 
  LCD_WR_DATA(0x33); 
  LCD_WR_DATA(0x33); 
  LCD_WR_COMMAND(0xb7); 
  LCD_WR_DATA(0x35); 
 
  LCD_WR_COMMAND(0xbb); 
  LCD_WR_DATA(0x30); 
  
  LCD_WR_COMMAND(0xc3); 
  LCD_WR_DATA(0x1c); 
  LCD_WR_COMMAND(0xc4); 
  LCD_WR_DATA(0x18); 
  LCD_WR_COMMAND(0xc6); 
  LCD_WR_DATA(0x0f); 
  LCD_WR_COMMAND(0xd0); 
  LCD_WR_DATA(0xa4); 
  LCD_WR_DATA(0xa2); 

  LCD_WR_COMMAND(0xe0); 
  LCD_WR_DATA(0xf0); 
  LCD_WR_DATA(0x00); 
  LCD_WR_DATA(0x0a); 
  LCD_WR_DATA(0x10); 
  LCD_WR_DATA(0x12); 
  LCD_WR_DATA(0x1b); 
  LCD_WR_DATA(0x39); 
  LCD_WR_DATA(0x44); 
  LCD_WR_DATA(0x47); 
  LCD_WR_DATA(0x28); 
  LCD_WR_DATA(0x12); 
  LCD_WR_DATA(0x10); 
  LCD_WR_DATA(0x16); 
  LCD_WR_DATA(0x1b); 
  LCD_WR_COMMAND(0xe1); 
  LCD_WR_DATA(0xf0); 
  LCD_WR_DATA(0x00); 
  LCD_WR_DATA(0x0a); 
  LCD_WR_DATA(0x10); 
  LCD_WR_DATA(0x11); 
  LCD_WR_DATA(0x1a); 
  LCD_WR_DATA(0x3b); 
  LCD_WR_DATA(0x34); 
  LCD_WR_DATA(0x4e); 
  LCD_WR_DATA(0x3a); 
  LCD_WR_DATA(0x17); 
  LCD_WR_DATA(0x16); 
  LCD_WR_DATA(0x21); 
  LCD_WR_DATA(0x22);  
  LCD_WR_COMMAND(0x3a); 
  LCD_WR_DATA(0x55);
  LCD_WR_COMMAND(0x11);
  Delay_ms(120);
  /* display on */
  LCD_WR_COMMAND(0x29);
}

/**
  * @brief  This function is write command to LCD.
  * @param  command : the command to write.
  * @retval None
  */
void LCD_WR_COMMAND(uint8_t command)
{
  *(uint8_t *) XMC_LCD_COMMAND = command;
}

/**
  * @brief  This function is write data to LCD.
  * @param  data : the data to write.
  * @retval None
  */
void LCD_WR_DATA(uint8_t data)
{
  *(uint8_t *) XMC_LCD_DATA = data;
}

/**
  * @brief  This function is set row&column coordinates for LCD.
  * @param  xstart : Row coordinates starting vaule.
  * @param  ystart : column coordinates starting vaule.
  * @param  xend : Row coordinates ending vaule.
  * @param  yend : column coordinates ending vaule.
  * @retval None
  */
void LCD_SetBlock(uint16_t xstart,uint16_t ystart,uint16_t xend,uint16_t yend)
{
  /* Set row coordinates */
  LCD_WR_COMMAND(0x2a);
  LCD_WR_DATA(xstart>>8);
  LCD_WR_DATA(xstart);
  LCD_WR_DATA(xend>>8);
  LCD_WR_DATA(xend);
  
  /* Set column coordinates */
  LCD_WR_COMMAND(0x2b);
  LCD_WR_DATA(ystart>>8);
  LCD_WR_DATA(ystart);
  LCD_WR_DATA(yend>>8);
  LCD_WR_DATA(yend);
  /* Enable write menory */
  LCD_WR_COMMAND(0x2c);
}

/**
  * @brief  This function is write one point to LCD.
  * @param  data : the data to write.
  * @retval None
  */
void LCD_WriteOnePoint(uint16_t color)
{
  LCD_WR_DATA(color>>8);
  LCD_WR_DATA(color);
}

/**
  * @brief  This function is draw point to LCD.
  * @param  data : the data to write.
  * @retval None
  */
void LCD_DrawPoint(uint16_t x,uint16_t y,uint16_t color)
{
  
  LCD_SetBlock(x,y,x,y); 
  LCD_WriteOnePoint(color);
}

/**
  * @brief  This function is clear the LCD.
  * @param  data : the data to write.
  * @retval None
  */
void LCD_Clear(uint16_t color)
{
  u32 i;
  LCD_SetBlock(0,0,240,320);
  for(i=0;i<76800;i++)
   LCD_WriteOnePoint(color);
 
}

/**
  * @brief  This function is display number on the LCD.
  * @param  None
  * @retval None
  */
void LCD_Display_Number(void)
{
  int i;
  LCD_SetBlock(50,50,199,219);
  for(i=0;i<sizeof(gImage_8);i++)
  {
    LCD_WR_DATA(gImage_8[i]);
  }
  Delay_ms(1000);
  LCD_SetBlock(50,50,199,219);
  for(i=0;i<sizeof(gImage_3);i++)
  {
    LCD_WR_DATA(gImage_3[i]);
  }   
  Delay_ms(1000);
  LCD_SetBlock(50,50,199,219);
  for(i=0;i<sizeof(gImage_5);i++)
  {
    LCD_WR_DATA(gImage_5[i]);
  }  
  Delay_ms(1000);
  LCD_SetBlock(50,50,199,219);
  for(i=0;i<sizeof(gImage_9);i++)
  {
    LCD_WR_DATA(gImage_9[i]);
  }  
  Delay_ms(1000);
  LCD_SetBlock(50,50,199,219);
  for(i=0;i<sizeof(gImage_0);i++)
  {
    LCD_WR_DATA(gImage_0[i]);
  }  
  Delay_ms(1000);
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


