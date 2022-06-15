 /**
  ******************************************************************************
  * File   : USB_Device/MassStorage/src/main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : MassStorage demo main file
  ******************************************************************************
  */

#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "at32_board.h"
#include "mass_mal.h"
/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_MassStorage
  * @{
  */ 
extern uint32_t  Data_Buffer[];

int main(void)
{
    uint8_t u8FlashLed = LED4;
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
    
    /*init SD card*/
    if ( MAL_Init(0) != MAL_OK )
    {
        /*no SD Card connect*/
        u8FlashLed = LED2;
    }
    /*if use USB SRAM_Size = 768 Byte, default is 512 Byte*/
    //Set_USB768ByteMode();
    
    /* USB protocol and register initialize*/
   	USB_Init();
    
 	  while(1)
	  {
        Delay_ms(500);
        AT32_LEDn_Toggle((LED_Type)u8FlashLed);
    }
}

/**
  * @}
  */

/**
  * @}
  */



















