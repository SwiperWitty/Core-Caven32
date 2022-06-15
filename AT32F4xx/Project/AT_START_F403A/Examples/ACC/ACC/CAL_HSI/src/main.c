/**
  ******************************************************************************
  * File   : ACC/CAL_HSI/src/main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : VirtualComPort loopback Demo main file
  ******************************************************************************
  */
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "at32_board.h"

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup ACC CAL_HSI 
  * @{
  */ 
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//#define ACC_CAL
#define ACC_TRIM

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
NVIC_InitType NVIC_InitStructure;
uint8_t usb_recvBuffer[256];
uint8_t usb_sendBuffer[256];
uint16_t recvLen, sendLen;
int ACC_C2_value=0;

/* Private function prototypes -----------------------------------------------*/
void MCO_Config(void);
void NVIC_Configuration(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{		
  /*AT START F4xx board initialize
  * Include LED, Button
  */
  AT32_Board_Init();
  
  /* NVIC configuration */
  NVIC_Configuration();
  
  /*initialize UART1 */
  UART_Print_Init(921600);
  
  /*USB GPIO configure*/
  AT32_USB_GPIO_init();
  
  /*Enable USB Interrut*/
  USB_Interrupts_Config();    
  
  /*Set USB Clock, USB Clock must 48MHz*/
  Set_USBClock(USBCLK_FROM_HSI);
  
  /*USB protocol and register initialize*/
  USB_Init();

  /*Output HSI clock*/
  MCO_Config();
  
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
  
  while(1)
  {
    recvLen = CDC_Receive_DATA(usb_recvBuffer, 256);
    if ( recvLen > 0 )
    {  /*recvive data from USB*/

        /*Send data to PC Host*/
        sendLen = CDC_Send_DATA(usb_recvBuffer, recvLen);
        AT32_LEDn_Toggle(LED3);
    }

    Delay_ms(1);  
    while(USART_GetFlagStatus(USART1,USART_FLAG_TDE) == 0);
    
    /*Printf ACC Calibration/TRIM value */
    #if defined(ACC_CAL)
    USART_SendData(USART1,ACC_GetHSICAL());
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
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
  NVIC_Init(&NVIC_InitStructure);
}
/**
  * @}
  */ 

/**
  * @}
  */ 






















