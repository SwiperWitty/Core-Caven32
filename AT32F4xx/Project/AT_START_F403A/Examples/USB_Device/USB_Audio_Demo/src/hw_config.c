/**
  ******************************************************************************
  * File   : USB_Device/USB_Audio_Demo/src/hw_config.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Hardware Configuration & setup
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "at32_board.h"

/** @addtogroup AT32F413_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_Audio_Demo
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/

int ACC_C2_value=0;
//#define ACC_CAL
#define ACC_TRIM

static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len);
/* Extern variables ----------------------------------------------------------*/
 
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Set USB Prescaler
  *         Set and cleared by software to generate 48MHz USB Clock 
  * @param  Clk_Source:Clock source is HSE or HSI
  * @retval None
  */
void Set_USBClock(u8 Clk_Source)
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
        case 168000000:
            RCC_USBCLKConfig(RCC_USBCLKSelection_PLL_Div3_5);
            break;
        case 192000000:
            RCC_USBCLKConfig(RCC_USBCLKSelection_PLL_Div4);
            break;
        default:
            break;
    }
  }
  else if(Clk_Source == USBCLK_FROM_HSI)
  {
    /*Open the ACC clock*/
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_ACC, ENABLE);	
    
    /*Enable the ACC Calibration ready interrupt*/
    ACC_ITConfig(ACC_IT_CALRDYIEN,ENABLE);
    
    /*Enable the ACC Reference Signal Lost interrupt*/
    ACC_ITConfig(ACC_IT_EIEN,ENABLE);
    
    /*Update the C1\C2\C3 value*/
    ACC_C2_value = 8000;
    ACC_WriteC1(ACC_C2_value-10);
    ACC_WriteC2(ACC_C2_value);  
    ACC_WriteC3(ACC_C2_value+10);

    /*Open ACC Calibration*/
    #if defined(ACC_CAL)
      ACC_EnterCALMode(ACC_CAL_ON,ENABLE);
    #elif defined(ACC_TRIM)
      ACC_EnterCALMode(ACC_TRIM_ON,ENABLE);
    #endif
    /* Set usbclk from HSI */
    RCC_HSI2USB48M(ENABLE);
  }
    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_USB, ENABLE);				 
} 
 
/*******************************************************************************
* Function Name  : Enter_LowPowerMode.
* Description    : Power-off system clocks and power while entering suspend mode.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Enter_LowPowerMode(void)
{
  /* Set the device state to suspend */
  bDeviceState = SUSPENDED;	  
  /* Request to enter STOP mode with regulator in low power mode */
//  PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
}			  

/*******************************************************************************
* Function Name  : Leave_LowPowerMode.
* Description    : Restores system clocks and power while exiting suspend mode.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Leave_LowPowerMode(void)
{
  DEVICE_INFO *pInfo = &Device_Info;
								   
													   
  /* Set the device state to the correct state */
  if (pInfo->Current_Configuration != 0)
  {
    /* Device configured */
    bDeviceState = CONFIGURED;
		//printf("CONFIGURED\r\n");
  }
  else
  {
    bDeviceState = ATTACHED;
		//printf("ATTACHED\r\n");
  }
}

/**
  * @brief  Configure Interrupt
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
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    /*Enable USB low priority interrupt*/
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /*Enable USB High priority interrupt*/
    NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /*Enable USB WakeUP interrupt*/
    NVIC_InitStructure.NVIC_IRQChannel = USBWakeUp_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /*Enable the ACC interrupt*/
    NVIC_InitStructure.NVIC_IRQChannel = ACC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 0x0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x1;		
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
    NVIC_Init(&NVIC_InitStructure);
    	 	 	
}
		    
/*******************************************************************************
* Function Name  : Get_SerialNum.
* Description    : Create the serial number string descriptor.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Get_SerialNum(void)
{
  u32 Device_Serial0, Device_Serial1, Device_Serial2;

  Device_Serial0 = *(u32*)(0x1FFFF7E8);
  Device_Serial1 = *(u32*)(0x1FFFF7EC);
  Device_Serial2 = *(u32*)(0x1FFFF7F0);

  Device_Serial0 += Device_Serial2;

  if (Device_Serial0 != 0)
  { 
    IntToUnicode (Device_Serial0, &Audio_StringSerial[2] , 8);
    IntToUnicode (Device_Serial1, &Audio_StringSerial[18], 4);
  }
}

/*******************************************************************************
* Function Name  : HexToChar.
* Description    : Convert Hex 32Bits value into char.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
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


