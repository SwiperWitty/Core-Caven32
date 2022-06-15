/* Includes ------------------------------------------------------------------*/
#include "xmc_lcd.h"

/** @addtogroup AT32F403A_StdPeriph_Examples
  * @{
  */

/** @addtogroup XMC_LCD&TOUCH_16BIT
  * @brief      This file provides a set of functions needed to drive the 
  *             LCD.
  * @{
  */ 
  
/** @defgroup XMC_LCD&TOUCH_16BIT_Private_Defines
  * @{
  */
  
_lcd_dev  lcd_dev = 
{
  0,
  XMC_init,
  LCD_init,
  LCD_SetBlock,
  LCD_DrawPoint,
  LCD_Clear,
};

_lcd_dev *lcd;

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
  XMC_NORSRAMTimingInitType  XMC_NORSRAMTimingInitStruct;
  GPIO_InitType GPIO_InitStructure;
  
  /* Enable the GPIO Clock */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB|RCC_APB2PERIPH_GPIOD| RCC_APB2PERIPH_GPIOE |RCC_APB2PERIPH_GPIOC|RCC_APB2PERIPH_GPIOA|
                         RCC_APB2PERIPH_AFIO, ENABLE);
  /* Enable the XMC Clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_XMC, ENABLE);
  
  GPIO_PinsRemapConfig(AFIO_MAP7_XMC_0001,ENABLE);
  /*-- GPIO Configuration ------------------------------------------------------*/
  /*!< LCD Data lines configuration */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_11 | GPIO_Pins_12 | GPIO_Pins_13 | GPIO_Pins_14|GPIO_Pins_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_Init(GPIOE, &GPIO_InitStructure); 
  
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_14|GPIO_Pins_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
  
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_9|GPIO_Pins_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure); 
  
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_6|GPIO_Pins_11|GPIO_Pins_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure); 
  
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_2 | GPIO_Pins_3 | GPIO_Pins_4 | GPIO_Pins_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
   
  /*!< LCD Reset lines configuration */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure); 
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_0;
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

  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure); 
  
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure); 
  
  /*-- FSMC Configuration ------------------------------------------------------*/
  XMC_NORSRAMTimingInitStruct.XMC_AdrHoldTime = 0x00;
  XMC_NORSRAMTimingInitStruct.XMC_AdrOpTime = 0x02;
  XMC_NORSRAMTimingInitStruct.XMC_CLKPsc = 0x00;
  XMC_NORSRAMTimingInitStruct.XMC_DataOpTime = 0x02;
  XMC_NORSRAMTimingInitStruct.XMC_DataStableTime = 0x00;
  XMC_NORSRAMTimingInitStruct.XMC_IntervalBetweenOP = 0x00;
  XMC_NORSRAMTimingInitStruct.XMC_Mode = XMC_Mode_A;
  
  XMC_NORSRAMInitStruct.XMC_Bank = XMC_Bank1_NORSRAM1;
  XMC_NORSRAMInitStruct.XMC_BusType = XMC_BusType_16b;
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
  XMC_NORSRAMCmd(XMC_Bank1_NORSRAM1, ENABLE);
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
  lcd->xmc_init();
  LCD_RESET_HIGH;
  Delay_ms(50);
  /* Read ID*/
  LCD_WR_COMMAND(0x0000);
  Delay_us(5);
  lcd->lcd_id = LCD_RD_DATA();
  LCD_WR_COMMAND(0XD3);
  
  lcd->lcd_id = LCD_RD_DATA();
  lcd->lcd_id = LCD_RD_DATA();
  lcd->lcd_id = LCD_RD_DATA();
  lcd->lcd_id = LCD_RD_DATA();
  lcd->lcd_id = lcd->lcd_id<<8;
  lcd->lcd_id |= LCD_RD_DATA();
  
  LCD_WR_COMMAND(0xCF);  
  LCD_WR_DATA(0x00); 
  LCD_WR_DATA(0xC1); 
  LCD_WR_DATA(0X30); 
  LCD_WR_COMMAND(0xED);  
  LCD_WR_DATA(0x64); 
  LCD_WR_DATA(0x03); 
  LCD_WR_DATA(0X12); 
  LCD_WR_DATA(0X81); 
  LCD_WR_COMMAND(0xE8);  
  LCD_WR_DATA(0x85); 
  LCD_WR_DATA(0x10); 
  LCD_WR_DATA(0x7A); 
  LCD_WR_COMMAND(0xCB);  
  LCD_WR_DATA(0x39); 
  LCD_WR_DATA(0x2C); 
  LCD_WR_DATA(0x00); 
  LCD_WR_DATA(0x34); 
  LCD_WR_DATA(0x02); 
  LCD_WR_COMMAND(0xF7);  
  LCD_WR_DATA(0x20); 
  LCD_WR_COMMAND(0xEA);  
  LCD_WR_DATA(0x00); 
  LCD_WR_DATA(0x00); 
  LCD_WR_COMMAND(0xC0);    //Power control 
  LCD_WR_DATA(0x1B);   //VRH[5:0] 
  LCD_WR_COMMAND(0xC1);    //Power control 
  LCD_WR_DATA(0x01);   //SAP[2:0];BT[3:0] 
  LCD_WR_COMMAND(0xC5);    //VCM control 
  LCD_WR_DATA(0x30); 	 //3F
  LCD_WR_DATA(0x30); 	 //3C
  LCD_WR_COMMAND(0xC7);    //VCM control2 
  LCD_WR_DATA(0XB7); 
  LCD_WR_COMMAND(0x36);    // Memory Access Control 
  LCD_WR_DATA(0x48); 
  LCD_WR_COMMAND(0x3A);   
  LCD_WR_DATA(0x55); 
  LCD_WR_COMMAND(0xB1);   
  LCD_WR_DATA(0x00);   
  LCD_WR_DATA(0x1A); 
  LCD_WR_COMMAND(0xB6);    // Display Function Control 
  LCD_WR_DATA(0x0A); 
  LCD_WR_DATA(0xA2); 
  LCD_WR_COMMAND(0xF2);    // 3Gamma Function Disable 
  LCD_WR_DATA(0x00); 
  LCD_WR_COMMAND(0x26);    //Gamma curve selected 
  LCD_WR_DATA(0x01); 
  LCD_WR_COMMAND(0xE0);    //Set Gamma 
  LCD_WR_DATA(0x0F); 
  LCD_WR_DATA(0x2A); 
  LCD_WR_DATA(0x28); 
  LCD_WR_DATA(0x08); 
  LCD_WR_DATA(0x0E); 
  LCD_WR_DATA(0x08); 
  LCD_WR_DATA(0x54); 
  LCD_WR_DATA(0XA9); 
  LCD_WR_DATA(0x43); 
  LCD_WR_DATA(0x0A); 
  LCD_WR_DATA(0x0F); 
  LCD_WR_DATA(0x00); 
  LCD_WR_DATA(0x00); 
  LCD_WR_DATA(0x00); 
  LCD_WR_DATA(0x00); 		 
  LCD_WR_COMMAND(0XE1);    //Set Gamma 
  LCD_WR_DATA(0x00); 
  LCD_WR_DATA(0x15); 
  LCD_WR_DATA(0x17); 
  LCD_WR_DATA(0x07); 
  LCD_WR_DATA(0x11); 
  LCD_WR_DATA(0x06); 
  LCD_WR_DATA(0x2B); 
  LCD_WR_DATA(0x56); 
  LCD_WR_DATA(0x3C); 
  LCD_WR_DATA(0x05); 
  LCD_WR_DATA(0x10); 
  LCD_WR_DATA(0x0F); 
  LCD_WR_DATA(0x3F); 
  LCD_WR_DATA(0x3F); 
  LCD_WR_DATA(0x0F); 
  LCD_WR_COMMAND(0x2B); 
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x01);
  LCD_WR_DATA(0x3f);
  LCD_WR_COMMAND(0x2A); 
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xef);	 
  LCD_WR_COMMAND(0x11); //Exit Sleep
  Delay_ms(120);
  LCD_WR_COMMAND(0x29); //display on
  
  LCD_WR_COMMAND(0x36);
  LCD_WR_DATA(0x08);
  
  LCD_WR_COMMAND(0x2a);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xef>>8);
  LCD_WR_DATA(0xef&0xff);
  
  LCD_WR_COMMAND(0x2b);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x13f>>8);
  LCD_WR_DATA(0x13f&0xff);
  
  LCD_BL_HIGH;
    
}

/**
  * @brief  This function is write command to LCD.
  * @param  command : the command to write.
  * @retval None
  */
void LCD_WR_COMMAND(u16 command)
{
  *(u16*) XMC_LCD_COMMAND = command;
}

/**
  * @brief  This function is write data to LCD.
  * @param  data : the data to write.
  * @retval None
  */
void LCD_WR_DATA(u16 data)
{
  *(u16*) XMC_LCD_DATA = data;
}

u16 LCD_RD_DATA(void)
{
  vu16 data;  
  data = *(vu16*) XMC_LCD_DATA;
  return data;
}

/**
  * @brief  This function is set row&column coordinates for LCD.
  * @param  xstart : Row coordinates starting vaule.
  * @param  ystart : column coordinates starting vaule.
  * @param  xend : Row coordinates ending vaule.
  * @param  yend : column coordinates ending vaule.
  * @retval None
  */
void LCD_SetBlock(u16 xstart,u16 ystart,u16 xend,u16 yend)
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
void LCD_WriteOnePoint(u16 color)
{
  LCD_WR_DATA(color);
}

/**
  * @brief  This function is draw point to LCD.
  * @param  data : the data to write.
  * @retval None
  */
void LCD_DrawPoint(u16 x,u16 y,u16 color)
{
  
  lcd->lcd_setblock(x,y,x,y); 
  LCD_WriteOnePoint(color);
}

/**
  * @brief  This function is clear the LCD.
  * @param  data : the data to write.
  * @retval None
  */
void LCD_Clear(u16 color)
{
  u32 i;
  lcd->lcd_setblock(0,0,240,320);
  for(i=0;i<76800;i++)
   LCD_WriteOnePoint(color);
 
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


