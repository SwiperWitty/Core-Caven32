/**
  ******************************************************************************
  * File   : USB_Device/HID_IAP_Demo/src/hw_config.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : USB Hardware Configuration & setup
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "at32_board.h"

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_HID_IAP_Demo
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
__IO uint8_t Send_Buffer[2];
__IO uint8_t PrevXferComplete = 1;
__IO uint8_t ADCSimValue=0;
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len);
/* Extern variables ----------------------------------------------------------*/
 
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Set USB Prescaler
  *         Set and cleared by software to generate 48MHz USB Clock 
  * @param  None
  * @retval None
  */
void Set_USBClock()
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
        case 168000000:
            RCC_USBCLKConfig(RCC_USBCLKSelection_PLL_Div3_5);
            break;
        case 192000000:
            RCC_USBCLKConfig(RCC_USBCLKSelection_PLL_Div4);
            break;
        default:
            break;
    }
    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_USB, ENABLE);
				 
} 
 
/**
  * @brief  Power-off system clocks and power while entering suspend mode.
  * @param  None.              
  * @retval None.
  */
void Enter_LowPowerMode(void)
{
  /* Set the device state to suspend */
  bDeviceState = SUSPENDED;	  
  /* Request to enter STOP mode with regulator in low power mode */
//  PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
}			  

/**
  * @brief  Restores system clocks and power while exiting suspend mode.
  * @param  None.              
  * @retval None.
  */
void Leave_LowPowerMode(void)
{
  DEVICE_INFO *pInfo = &Device_Info;
								   
													   
  /* Set the device state to the correct state */
  if (pInfo->Current_Configuration != 0)
  {
    /* Device configured */
    bDeviceState = CONFIGURED;
  }
  else
  {
    bDeviceState = ATTACHED;
  }
}

/**
  * @brief  Configure USB Interrupt
  * @param  None
  * @retval None
  */
void USB_Interrupts_Config(void)
{
    NVIC_InitType  NVIC_InitStructure;
    EXTI_InitType EXTI_InitStructure;
    
    /*Enable EXTI18 as interrupt Mode*/
    EXTI_ClearIntPendingBit(EXTI_Line18);
    EXTI_InitStructure.EXTI_Line = EXTI_Line18;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineEnable = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    
     /*Enable USB low priority interrupt*/
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /*Enable USB WakeUP interrupt*/
    NVIC_InitStructure.NVIC_IRQChannel = USBWakeUp_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    	 	 	
}
		    
/**
  * @brief  Create the serial number string descriptor.
  * @param  None.
  * @retval None.
  */
void Get_SerialNum(void)
{
  u32 Device_Serial0, Device_Serial1, Device_Serial2;

  Device_Serial0 = *(u32*)(0x1FFFF7E8);
  Device_Serial1 = *(u32*)(0x1FFFF7EC);
  Device_Serial2 = *(u32*)(0x1FFFF7F0);

  Device_Serial0 += Device_Serial2;

  if (Device_Serial0 != 0)
  {
    IntToUnicode (Device_Serial0, &IapHID_StringSerial[2] , 8);
    IntToUnicode (Device_Serial1, &IapHID_StringSerial[18], 4);
  }
}

/**
  * @brief  Convert Hex 32Bits value into char.
  * @param  value: need convert 32bit value
  * @param  pbuf: dest buffer
  * @param  len: convert len
  * @retval None.
  */
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len)
{
  uint8_t idx = 0;
  
  for( idx = 0 ; idx < len ; idx ++)
  {
    if( ((value >> 28)) < 0xA )
    {
      pbuf[ 2* idx] = (value >> 28) + '0';
    }
    else
    {
      pbuf[2* idx] = (value >> 28) + 'A' - 10; 
    }
    
    value = value << 4;
    
    pbuf[ 2* idx + 1] = 0;
  }
}

/**
  * @}
  */ 

/**
  * @}
  */




