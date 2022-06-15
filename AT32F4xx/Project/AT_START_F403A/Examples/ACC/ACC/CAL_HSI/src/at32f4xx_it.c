/**
  ******************************************************************************
  * File   : ACC/CAL_HSI/src/at32f403_it.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main Interrupt Service Routines.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx_it.h"
#include "usb_istr.h"
#include "at32_board.h"

/** @addtogroup AT32F413_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_VirtualComPort_loopback
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
  * @brief  This function handles DebugMon exception.
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
  * @brief  This function handles USB High Priority or CAN TX interrupts
  *         requests.
  * @param  None
  * @retval None
  */
void USB_HP_CAN1_TX_IRQHandler(void)
{}

/**
  * @brief  This function handles USB Low Priority or CAN RX0 interrupts
  *         requests.
  * @param  None
  * @retval None
  */
void USB_LP_CAN1_RX0_IRQHandler (void)
{
  USB_Istr();
}   

/**
  * @brief  This function handles USB WakeUp interrupt request.
  * @param  None
  * @retval None
  */
void USBWakeUp_IRQHandler(void)
{
    EXTI_ClearIntPendingBit(EXTI_Line18);
}

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

