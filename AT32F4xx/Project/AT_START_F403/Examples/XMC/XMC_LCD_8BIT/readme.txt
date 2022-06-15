
/**
  ******************************************************************************
  * File   : XMC/xmc_lcd/readme.txt  
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : XMC lcd example
  ******************************************************************************
  */

	Note:
    This example provides a basic example of how to use the XMC firmware library and
	an associate driver to perform read/write operations on the LCD. The LCD drive IC
	is ST7789V and  will display some number, it's actually showing pictures. This 
	example support 100pin and 144pin package at the same time.
	
	The pin to pin with LCD and XMC:
	GPIO      PB10    LCD_RESET
	XMC_A16   PD11    LCD_RS
	XMC_NE1   PD7     LCD_CS
	XMC_NWE   PD5     LCD_WR 
	XMC_NOE   PD4     LCD_RD 
	XMC_D0	  PD14	  DATA[0]
	XMC_D1	  PD15	  DATA[1]
	XMC_D2	  PD0	  DATA[2]
	XMC_D3	  PD1	  DATA[3]
	XMC_D4	  PE7	  DATA[4]
	XMC_D5	  PE8	  DATA[5]
	XMC_D6	  PE9	  DATA[6]
	XMC_D7	  PE10	  DATA[7]	



