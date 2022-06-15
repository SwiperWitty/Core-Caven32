/**
  ******************************************************************************
  * File   : USB_Device/USB_Printer/src/main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : printer Demo main file
  ******************************************************************************
  */
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "at32_board.h"

uint8_t usb_recvBuffer[256];
uint8_t usb_sendBuffer[256];
uint16_t recvLen, sendLen;


/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_USB_Printer
  * @{
  */ 
int main(void)
{		
    /*AT START F4xx board initialize
    * Include LED, Button
    */
    AT32_Board_Init();
    
    /*USB GPIO configure*/
    AT32_USB_GPIO_init();
    
    /*Enable USB Interrut*/
 	  USB_Interrupts_Config();    
    
    /*Set USB Clock, USB Clock must 48MHz*/
    Set_USBClock();
    
    /* USB protocol and register initialize*/
 	  USB_Init();
    
   	while(1)
	  {

	  }	   										    			    
}

/**
  * @}
  */

/**
  * @}
  */




















