/**
  ******************************************************************************
  * File   : ACC/CAL_HSI/src/at32f403_it.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  */ 


/* Includes ------------------------------------------------------------------*/
#include "at32f4xx_it.h"
#include "usb_dcd_int.h"

extern USB_OTG_CORE_HANDLE    USB_OTG_dev;
/** @addtogroup AT32F415_StdPeriph_Examples
  * @{
  */

/** @addtogroup ACC CAL_HSI 
  * @{
  */

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}


void USBWakeUp_IRQHandler(void)
{  
    EXTI_ClearIntPendingBit(EXTI_Line18);
}


void USBOTG_IRQHandler(void)
{
  USB_DEVICE_OTG_INTSTS_Handler(&USB_OTG_dev);
}
/******************************************************************************/
/*                 AT32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_at32f413_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles ACC interrupt request.
  * @param  None
  * @retval None
  */
void ACC_IRQHandler(void)
{
  if(ACC_GetFlagStatus(ACC_FLAG_CALRDY) == SET)
  {
    AT32_LEDn_Toggle(LED2);
    /*Claer ACC Calibration ready flag*/
    ACC_ClearFlag(ACC_FLAG_CALRDY);
  }
  if(ACC_GetFlagStatus(ACC_FLAG_RSLOST) == SET)
  {
    AT32_LEDn_Toggle(LED3);
    /*Claer ACC Reference Signal Lost flag*/
    ACC_ClearFlag(ACC_FLAG_RSLOST);
  }

}

/**
  * @}
  */

/**
  * @}
  */

