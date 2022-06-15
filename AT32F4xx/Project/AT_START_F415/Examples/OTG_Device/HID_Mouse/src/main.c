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
#include "usbd_hid_core.h"

/** @addtogroup AT32F415_StdPeriph_Examples
  * @{
  */

/** @addtogroup OTG_Device_HID_Mouse
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
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t Rxbuffer[64]; 
//__IO uint32_t receive_count =1;
uint16_t recvLen, sendLen;
uint8_t usb_recvBuffer[256];
uint8_t usb_sendBuffer[256];
uint8_t Mouse_Buffer[4] = {0, 0, 0, 0};
extern __IO uint32_t  data_sent;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void Set_USBOTGClock(void);
void Mouse_send(uint8_t operation);
/**
  * @brief  Main Function.
  * @param  None
  * @retval None
  */
int main(void)
{
  BUTTON_Type Button_Press;
  u8 keysta = 0;
  AT32_Board_Init();
  
  Set_USBOTGClock();
  
  USB_Device_Init(&USB_OTG_dev,
            USB_OTG_FS_CORE_ID,
            &USER_desc, 
            &USBD_HID_cb, 
            &USER_cb);
  

  while ( 1 )
  {
    Button_Press = AT32_BUTTON_Press();
    switch (Button_Press)
    {

      case BUTTON_WAKEUP:
        Mouse_send(RIGHT_BUTTON);  /*Mouse Right Button*/
        keysta = 1;
        break;
      default:
        break;
    }
    if ( keysta != 0 )
    {
      Delay_ms(30);
      keysta=0;
      Mouse_send(DEFAULT_NULL);      /*release the button*/
    }
    Delay_ms(40);
    AT32_LEDn_Toggle(LED4);
  }
}

/**
  * @brief  Mouse Send
  * @param  operation: Right button, Left button..
  * @retval None
  */
void Mouse_send(uint8_t operation)
{
  uint8_t Mouse_Buffer[4] = {0, 0, 0, 0};
  int8_t X = 0, Y = 0,MouseButton=0;

  switch (operation)
  {
    case LEFT:
      X -= CURSOR_STEP;
      break;

    case RIGHT:
      X += CURSOR_STEP;
      break;
	  
    case UP:
      Y -= CURSOR_STEP;
      break;

    case DOWN:
      Y += CURSOR_STEP;
	  break;
	   
	case LEFT_BUTTON:
      MouseButton = MouseButton|0x01;
      break;

 	case RIGHT_BUTTON:
      MouseButton = MouseButton|0x02;
      break;

    default:
      break;
  }
  Mouse_Buffer[0] = MouseButton;
  Mouse_Buffer[1] = X;
  Mouse_Buffer[2] = Y;
  usb_device_hid_sendreport (&USB_OTG_dev, 
                           Mouse_Buffer,
                           4);
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

