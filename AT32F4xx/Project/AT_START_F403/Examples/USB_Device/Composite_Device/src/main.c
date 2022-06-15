/**
  ******************************************************************************
  * File   : USB_Device/Composite_Device/src/main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Composite Device Demo main file
  ******************************************************************************
  */
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "at32_board.h"

uint8_t usb_recvBuffer[256];
uint8_t usb_sendBuffer[256];
uint16_t recvLen, sendLen;

/** @addtogroup AT32F413_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_Composite_Device
  * @{
  */ 
int main(void)
{		
    BUTTON_Type Button_Press;
    u8 keysta = 0;	
  
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
        /*Communication by mouse*/
        Button_Press = AT32_BUTTON_Press();
        switch (Button_Press)
        {
          case BUTTON_WAKEUP:
              /*send mouse right button*/
              Joystick_Send(2, 0, 0, 0); 
              keysta = 1;
              break;
          default:
              break;
        }
        if ( keysta != 0 )
        {
          Delay_ms(30);
          keysta=0;
          /*release the button*/
          Joystick_Send(0,0,0,0);       
        }
        
        /*Communication by VirtualComport*/
        recvLen = CDC_Receive_DATA(usb_recvBuffer, 256);
        if ( recvLen > 0 )
        {  
          /*Send data to PC Host*/
          sendLen = CDC_Send_DATA(usb_recvBuffer, recvLen);
          AT32_LEDn_Toggle(LED3);
        }
	  }	   										    			    
}

/**
  * @}
  */

/**
  * @}
  */




















