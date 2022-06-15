/**
  ******************************************************************************
  * File   : USB_Device/USB_Audio_Demo/src/main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : USB Audio Demo main file
  ******************************************************************************
  */
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "at32_board.h"
#include "at32_audio_conf.h"
#include "wm8988.h"

/** @addtogroup AT32F413_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_Audio_Demo
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
    Set_USBClock(USBCLK_FROM_HSE);
    
    /*WM8988 initialize*/
    if ( Audio_Init(AT32_DEFAULT_AUDIO_FREQ, AT32_DEFAULT_AUDIO_BITW) != AUDIO_OK )
    {
        /*WM8988 init Failed*/
        AT32_LEDn_Toggle(LED2);
    }      
    
    /*if use USB SRAM_Size = 768 Byte, default is 512 Byte*/
    Set_USB768ByteMode();
    
    /* USB protocol and register initialize*/
    USB_Init();         
    while(1)
    {
          AT32_LEDn_Toggle(LED4);
          WM8988_Poll();
          Delay_ms(200);

    }	   										    			    
}
/**
  * @}
  */

/**
  * @}
  */






















