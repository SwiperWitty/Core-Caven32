/**
  ******************************************************************************
  * File   : ACC/CAL_HSI/src/main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */

#include <stdio.h>
#include "at32f4xx.h"
#include "at32_board.h"
#include "usbd_cdc_core_loopback.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usbd_cdc_vcp.h"
#include "usbd_core.h"

/** @addtogroup AT32F415_StdPeriph_Examples
  * @{
  */

/** @addtogroup ACC CAL_HSI 
  * @{
  */ 
/* Private typedef -----------------------------------------------------------*/
USB_OTG_CORE_HANDLE    USB_OTG_dev;
#define USBCLK_FROM_HSE  1
#define USBCLK_FROM_HSI  2
/* Private define ------------------------------------------------------------*/
#define ACC_CAL
//#define ACC_TRIM

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t Rxbuffer[64]; 
//__IO uint32_t receive_count =1;
uint16_t recvLen, sendLen;
uint8_t usb_recvBuffer[256];
uint8_t usb_sendBuffer[256];
extern __IO uint32_t  data_sent;
int ACC_C2_value=0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void Set_USBOTGClock(u8 Clk_Source);
void MCO_Config(void);
void NVIC_Configuration(void);
void USART2_Init(uint32_t bound);

/**
  * @brief  Main Function.
  * @param  None
  * @retval None
  */
int main(void)
{
  /*AT START F4xx board initialize
  * Include LED, Button
  */
  AT32_Board_Init();
  
  /*initialize UART1 */
  USART2_Init(921600);
  
  /*initialize USB clock */
  Set_USBOTGClock(USBCLK_FROM_HSI);
  
  /*initialize USB  */
  USB_Device_Init(&USB_OTG_dev,
            USB_OTG_FS_CORE_ID,
            &USER_desc, 
            &USB_DEVICE_CDC_cb, 
            &USER_cb);
  
  /*Output HSI clock*/
  MCO_Config();
  
  /* NVIC configuration */
  NVIC_Configuration();
  
  /*Open the ACC clock*/
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_ACC, ENABLE);	
  
  /*Enable the ACC Calibration ready interrupt*/
  ACC_ITConfig(ACC_IT_CALRDYIEN,ENABLE);
  
  /*Enable the ACC Reference Signal Lost interrupt*/
  ACC_ITConfig(ACC_IT_EIEN,ENABLE);
  
  /*Update the C1\C2\C3 value*/
  ACC_C2_value = 8000;
  
  #ifdef ACC_CAL
    ACC_WriteC1(ACC_C2_value-20);
    ACC_WriteC2(ACC_C2_value);  
    ACC_WriteC3(ACC_C2_value+20);
  #else
    ACC_WriteC1(ACC_C2_value-10);
    ACC_WriteC2(ACC_C2_value);  
    ACC_WriteC3(ACC_C2_value+10);
  #endif

  /*Open ACC Calibration*/
  #if defined(ACC_CAL)
    ACC_EnterCALMode(ACC_CAL_ON,ENABLE);
  #elif defined(ACC_TRIM)
    ACC_EnterCALMode(ACC_TRIM_ON,ENABLE);
  #endif
  
  while ( 1 )
  {
    recvLen = CDC_Receive_DATA(usb_recvBuffer, 256);
    if ( recvLen > 0 )
    {  /*recvive data from USB*/

      /*Send data to PC Host*/
      sendLen = CDC_Send_DATA(usb_recvBuffer, recvLen);
      AT32_LEDn_Toggle(LED3);
    }
    
    Delay_ms(1);  
    while(USART_GetFlagStatus(USART2,USART_FLAG_TDE) == 0);
    
    /*Printf ACC Calibration/TRIM value */
    #if defined(ACC_CAL)
    USART_SendData(USART2,ACC_GetHSICAL());
    #elif defined(ACC_TRIM)
    USART_SendData(USART1,ACC_GetHSITRIM());
    #endif
    
  }
}

/**
  * @brief  Config Clock Out Function.
  * @param  None
  * @retval None
  */
void MCO_Config(void)
{
  GPIO_InitType GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);

  /*Init PA8 */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_8;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /*PA8 output HSI clock */
  RCC_CLKOUTConfig(RCC_CLKOUT_HSI,RCC_MCOPRE_1);
}

/**
  * @brief  Set USB Prescaler
  *         Set and cleared by software to generate 48MHz USB Clock 
  * @param  None
  * @retval None
  */
void Set_USBOTGClock(u8 Clk_Source)
{
  if(Clk_Source == USBCLK_FROM_HSE)
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
  }
  else if(Clk_Source == USBCLK_FROM_HSI)
  {
    /* Set usbclk from HSI */
    RCC_HSI2USB48M(ENABLE);
  }
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_USB, ENABLE) ;  
} 

/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
   NVIC_InitType NVIC_InitStructure;

  /*Enable the ACC interrupt*/
  NVIC_InitStructure.NVIC_IRQChannel = ACC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority =0 ;		
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  initialize UART1   
  * @param  bound: UART BaudRate
  * @retval None
  */
void USART2_Init(uint32_t bound)
{
  GPIO_InitType GPIO_InitStructure;
  USART_InitType USART_InitStructure;

  /*Enable the UART Clock*/
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);	
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_USART2, ENABLE);

  /* Configure the UART2 TX pin */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_2; 
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /*Configure UART param*/
  USART_StructInit(&USART_InitStructure);
  USART_InitStructure.USART_BaudRate = bound;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
  USART_Init(USART2, &USART_InitStructure); 
  
  USART_Cmd(USART2, ENABLE);   
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

