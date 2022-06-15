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
#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_msc_core.h"
/** @addtogroup AT32F415_StdPeriph_Examples
  * @{
  */

/** @addtogroup OTG_Device_VCP_Loopback
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
USB_HOST  USB_Host;
USB_OTG_CORE_HANDLE           USB_OTG_Core_dev;
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void Set_USBOTGClock(void);
/**
  * @brief  Main Function.
  * @param  None
  * @retval None
  */
int main(void)
{
  AT32_Board_Init();
  UART_Print_Init(115200);
  Set_USBOTGClock();
  
  USB_Host_Init(&USB_OTG_Core_dev, 
            USB_OTG_FS_CORE_ID,
            &USB_Host,
            &USBH_MSC_cb, 
            &USR_Callbacks);
  while ( 1 )
  {
    USB_Host_Process(&USB_OTG_Core_dev , &USB_Host);
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

