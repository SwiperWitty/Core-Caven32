/**
  ******************************************************************************
  * File   : xmc_lcd.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file contains all the functions prototypes for the 
  *          xmc_lcd firmware driver.
  ******************************************************************************
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/  
#ifndef __XMC_LCD_H
#define __XMC_LCD_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
   
#include "at32f4xx.h"
#include "at32_board.h"
   
/** @addtogroup XMC_LCD&TOUCH_16BIT
  * @{
  */ 
  
/** @addtogroup XMC_Utilities
  * @{
  */
 

/** @defgroup AT32F403A_XMC_LCD_Exported_Functions
  * @{
  */
typedef struct
{
  u16 lcd_id;
  void (*xmc_init)(void);
  void (*lcd_init)(void);
  void (*lcd_setblock)(u16 xstart,u16 ystart,u16 xend,u16 yend);
  void (*draw_point)(u16 x,u16 y,u16 color);
  void (*lcd_clear)(u16 color);
}_lcd_dev;

extern _lcd_dev  lcd_dev;
extern _lcd_dev *lcd;

/* The address of write data & command (XMC_A16) */
#define  XMC_LCD_COMMAND    0x6001FFFE
#define  XMC_LCD_DATA       0x60020000
/* The LCD Reset line */
#define LCD_RESET_HIGH  GPIO_SetBits(GPIOD,GPIO_Pins_3);
#define LCD_RESET_LOW   GPIO_ResetBits(GPIOD,GPIO_Pins_3);

#define LCD_BL_HIGH  GPIO_SetBits(GPIOB,GPIO_Pins_0);
#define LCD_BL_LOW   GPIO_ResetBits(GPIOB,GPIO_Pins_0);
static u16 LCD_ID;
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	   0x001F  
#define BRED             0XF81F
#define GRED 			       0XFFE0
#define GBLUE			       0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			     0XBC40
#define BRRED 			     0XFC07
#define GRAY  			     0X8430

void XMC_init(void);
void LCD_init(void);
void LCD_WR_COMMAND(u16 command);
void LCD_WR_DATA(u16 data);
u16 LCD_RD_DATA(void);
void LCD_SetBlock(u16 xstart,u16 ystart,u16 xend,u16 yend);
void LCD_WriteOnePoint(u16 color);
void LCD_DrawPoint(u16 x,u16 y,u16 color);
void LCD_Clear(u16 color);

#ifdef __cplusplus
}
#endif

#endif /* __AT_START_XMC_LCD_8BIT_H */
/**
  * @}
  */

/**
  * @}
  */ 

/**
  * @}
  */ 

