/**
  ******************************************************************************
  * File   : USB_Device/Mouse/src/main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Mouse demo main file
  ******************************************************************************
  */

#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "at32_board.h"

/** @addtogroup AT32F403A_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_Mouse
  * @{
  */
  
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{		
    BUTTON_Type Button_Press;
    u8 keysta = 0;	
    uint8_t temp = 0;
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
    
    /*if use USB SRAM_Size = 768 Byte, default is 512 Byte*/
    //Set_USB768ByteMode();
    
    /* USB protocol and register initialize*/
 	  USB_Init();
    
    while(1)
	  {
        Button_Press = AT32_BUTTON_Press();
        temp = _GetCTRL();
        if(bDeviceState == SUSPENDED && ((temp&0x8) == 0x8))// USB SUSP and FSUSP set
        {
          switch (Button_Press)
          {
              case BUTTON_WAKEUP:
                  Resume(RESUME_INTERNAL);  // Wake up USB
                  break;
              default:
                  break;
          }
        }
        else
        {
          switch (Button_Press)
          {     
              case BUTTON_WAKEUP:
                  Joystick_Send(2, 0, 0, 0); /*send mouse right button*/
                  keysta = 1;
                  break;
              default:
                  break;
          }
          if ( keysta != 0 )
          {
              Delay_ms(30);
              keysta=0;
              Joystick_Send(0,0,0,0);       /*release the button*/
          }
          Delay_ms(40);
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


















