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
#include "usbd_hidiap_core.h"
#include "iap_user.h"

/** @addtogroup AT32F415_StdPeriph_Examples
  * @{
  */

/** @addtogroup OTG_Device_HID_Mouse
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
USB_OTG_CORE_HANDLE    USB_OTG_dev;
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t Rxbuffer[256];
__IO uint8_t Send_Buffer[64];
__IO uint8_t receive_flag = 0;
__IO uint8_t receive_len = 0;
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
  SCB->VTOR = FLASH_BASE | 0x5000;
  
  AT32_Board_Init();
  
  Set_USBOTGClock();
  
  USB_Device_Init(&USB_OTG_dev,
            USB_OTG_FS_CORE_ID,
            &USER_desc, 
            &USBD_HID_cb, 
            &USER_cb);
  

  while ( 1 )
  {
    IAP_handle_loop();
    AT32_LEDn_Toggle(LED4);
    Delay_ms(500);
  }
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

