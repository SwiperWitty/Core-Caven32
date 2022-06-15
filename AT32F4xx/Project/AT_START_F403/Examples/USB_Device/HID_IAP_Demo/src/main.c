/**
  ******************************************************************************
  * File   : USB_Device/HID_IAP_Demo/src/main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : HID IAP  demo main file
  ******************************************************************************
  */
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "at32_board.h"
#include "iap_user.h"
#include "at_spi_flash.h"

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_HID_IAP_Demo
  * @{
  */ 
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{	
    uint32_t LedTimer = 0, LedTog = SystemCoreClock/40;
    /*AT START F4xx board initialize
    * Include LED, Button
    */
    AT32_Board_Init();
    /*Check if the key Push button*/
    if ( AT32_BUTTON_State(BUTTON_WAKEUP) == 0x00)
    {
        /*Check Flag*/
        if (IAP_get_flag() == IAP_SUCCESS)
        {
            /*Jump to User Code*/
            IAP_Jump(FLASH_APP1_ADDR);
        }
    }
     
    /*USB GPIO configure*/
    AT32_USB_GPIO_init();
    
    /*Flash SPI init*/
    Flash_spi_init();
    
    /*Enable USB Interrut*/
 	  USB_Interrupts_Config();    
    
    /*Set USB Clock, USB Clock must 48MHz*/
    Set_USBClock();
    
    /* USB protocol and register initialize*/
 	  USB_Init();
    
 	  while(1)
	  {
        IAP_handle_loop();
        if ( LedTimer == LedTog)
        {
            AT32_LEDn_Toggle(LED2);
            LedTimer = 0;
        }
        LedTimer ++;
        
        
	  }	   										    			    
}

/**
  * @}
  */ 

/**
  * @}
  */





















