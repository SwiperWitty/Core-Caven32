/**
  ******************************************************************************
  * File   : USB_Device/VirtualComPort_loopback/src/main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : VirtualComPort loopback Demo main file
  ******************************************************************************
  */
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "at32_board.h"

uint8_t usb_recvBuffer[256];
uint8_t usb_sendBuffer[256];
uint16_t recvLen, sendLen;


/** @addtogroup AT32F403A_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_VirtualComPort_loopback
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
    
    /*Set USB Clock, USB Clock must 48MHz and clock source is HSE or HSI*/
    Set_USBClock(USBCLK_FROM_HSI);
    
    /* USB protocol and register initialize*/
 	  USB_Init();
    
   	while(1)
	  {
        recvLen = CDC_Receive_DATA(usb_recvBuffer, 256);
        if ( recvLen > 0 )
        {  /*recvive data from USB*/
    
            /*Send data to PC Host*/
            sendLen = CDC_Send_DATA(usb_recvBuffer, recvLen);
            AT32_LEDn_Toggle(LED3);

        }else
        {
            /*no data recv*/
            Delay_ms(500);
            AT32_LEDn_Toggle(LED4);
        }

	  }	   										    			    
}

/**
  * @}
  */

/**
  * @}
  */




















