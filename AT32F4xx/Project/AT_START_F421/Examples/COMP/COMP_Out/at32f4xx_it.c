/**
  ******************************************************************************
  * File   : COMP/COMP_Out/at32f4xx_it.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  */ 


/* Includes ------------------------------------------------------------------*/
#include "at32f4xx_it.h"

/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */
  
/** @addtogroup COMP_Out
  * @{
  */
  
void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}


void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 AT32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_at32f413_xx.s).                                            */
/******************************************************************************/


__IO uint16_t CaptureNumber = 0;
__IO uint32_t IC1ReadValue1 = 0, IC1ReadValue2 = 0;
void TMR1_CC_IRQHandler(void)
{
  TMR1->STS=0;  
  switch(CaptureNumber)
  {

    case 0:
            IC1ReadValue1=TMR1->CC1;
            CaptureNumber = 1;
            break;
    case 1:
            IC1ReadValue2=TMR1->CC1;      
            CaptureNumber = 2;         
            break;
    default:
            TMR1->STS=0;
            break;
  } 
}

/**
  * @}
  */

/**
  * @}
  */
  
