/**
  ******************************************************************************
  * File   : main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */

#include <stdio.h>
#include "at32f4xx.h"
#include "at32_board.h"

#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usbd_core.h"
#include "usbd_cdc_hid.h"
#include "usbd_hid_keyboard.h"
#include "usbd_cdc_vcp.h"
/** @addtogroup AT32F415_StdPeriph_Examples
  * @{
  */

/** @addtogroup OTG_Device_HID_Keyboard
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
USB_OTG_CORE_HANDLE    USB_OTG_dev;
/* Private define ------------------------------------------------------------*/
#define CURSOR_STEP     10
#define DOWN            1
#define LEFT            2
#define RIGHT           3
#define UP              4
#define LEFT_BUTTON     5
#define RIGHT_BUTTON    6
#define DEFAULT_NULL    0
uint16_t recvLen, sendLen;
uint8_t usb_recvBuffer[256];
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void Set_USBOTGClock(void);
void Keyboard_send(void);
/**
  * @brief  Main Function.
  * @param  None
  * @retval None
  */
int main(void)
{
  BUTTON_Type Button_Press;
  AT32_Board_Init();
  
  Set_USBOTGClock();
  
  USB_Device_Init(&USB_OTG_dev,
            USB_OTG_FS_CORE_ID,
            &USER_desc, 
            &USB_DEVICE_CDC_HID_cb, 
            &USER_cb);
  

  while ( 1 )
  {
    Button_Press = AT32_BUTTON_Press();
    switch (Button_Press)
    {

      case BUTTON_WAKEUP:
        Keyboard_send();
        break;
      default:
        break;
    }
    recvLen = CDC_Receive_DATA(usb_recvBuffer, 256);
    if ( recvLen > 0 )
    {  /*recvive data from USB*/

      /*Send data to PC Host*/
      sendLen = CDC_Send_DATA(usb_recvBuffer, recvLen);
      AT32_LEDn_Toggle(LED3);

    }
//    Delay_ms(40);
    AT32_LEDn_Toggle(LED4);
  }
}

/**
  * @brief  Keyboard Send
  * @param  None
  * @retval None
  */
void Keyboard_send()
{
/*Send one char*/
  USB_Keyboard_Send_Char('A');  
  USB_Keyboard_Send_Char('r');
  USB_Keyboard_Send_Char('t');
  USB_Keyboard_Send_Char('e');
  USB_Keyboard_Send_Char('r');
  USB_Keyboard_Send_Char('y');

  
  /*Send String*/
  USB_Keyboard_Send_String (" Keyboard Demo\r\n", 16);
  
  USB_Keyboard_Send_Char(0x00); /*字符发送完成之后发一个空字符，发送结束*/
}

/**
  * @brief  Set USB Prescaler
  *         Set and cleared by software to generate 48MHz USB Clock 
  * @param  None
  * @retval None
  */
void Set_USBOTGClock(void)
{
  switch (SystemCoreClock)
  {
    case 48000000:
      RCC_USBCLKConfig(RCC_USBCLKSelection_PLL_Div1);
      break;
    case 72000000:
      RCC_USBCLKConfig(RCC_USBCLKSelection_PLL_Div1_5);
      break;
    case 96000000:
      RCC_USBCLKConfig(RCC_USBCLKSelection_PLL_Div2);
      break;
    case 120000000:
      RCC_USBCLKConfig(RCC_USBCLKSelection_PLL_Div2_5);
      break;
    case 144000000:
      RCC_USBCLKConfig(RCC_USBCLKSelection_PLL_Div3);
      break;
    default:
        break;
  }
   RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_USB, ENABLE) ;  
} 


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif


/**
  * @}
  */

/**
  * @}
  */

