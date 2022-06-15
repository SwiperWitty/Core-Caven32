
/**
  ******************************************************************************
  * File   : XMC/xmc_lcd&touch/readme.txt  
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : XMC lcd example
  ******************************************************************************
  */
	Note:
    This example provides a basic example of how to use the XMC firmware library and
	an associate driver to perform read/write operations on the LCD. And use SPI3 drive
	touch screen,user can drow picture on the LCD.When used must be use the TFT_LCD_DEMO BOARD.
	Attention: TFT_LCD_DEMO BOARD LCD ID is 0x9341 and the touchscreen is resistive touchscreen.
	
	The pin to pin with LCD and XMC:
	GPIO      PD3     LCD_RESET
	GPIO      PD3     LCD_BL
	XMC_A16   PD11    LCD_RS
	XMC_NE1   PD7     LCD_CS
	XMC_NWE   PD2     LCD_WR 
	XMC_NOE   PC5     LCD_RD 
	XMC_D0	  PB14	  DATA[0]
	XMC_D1	  PC6	  DATA[1]
	XMC_D2	  PC11	  DATA[2]
	XMC_D3	  PC12	  DATA[3]
	XMC_D4	  PA2	  DATA[4]
	XMC_D5	  PA3	  DATA[5]
	XMC_D6	  PA4	  DATA[6]
	XMC_D7	  PA5	  DATA[7]
	XMC_D8	  PE11	  DATA[8]
	XMC_D9	  PE12	  DATA[9]
	XMC_D10	  PE13	  DATA[10]
	XMC_D11	  PE14	  DATA[11]
	XMC_D12	  PE15	  DATA[12]
	XMC_D13	  PB12	  DATA[13]
	XMC_D14	  PD9	  DATA[14]
	XMC_D15	  PD10	  DATA[15]



