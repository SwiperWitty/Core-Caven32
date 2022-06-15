/**
  ******************************************************************************
  * File   : ERTC/ERTC_Timer/at32f4xx_it.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "at32f4xx_it.h"
#include "at32_board.h"
#include <stdio.h>

/** @addtogroup AT32F415_StdPeriph_Examples
  * @{
  */

/** @addtogroup ERTC_Timer
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t uwERTCAlarmCount = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

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
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
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

/**
  * @brief  This function handles External line 10 to 15 interrupts request.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
  if((EXTI_GetIntStatus(GPIO_Pins_13) != RESET))
  {
   
    /* Disable the ERTC Clock */
    RCC_ERTCCLKCmd(DISABLE);
   
    /* Reset Counter*/
    uwERTCAlarmCount = 0;
    
    /* Disable the alarm */
    ERTC_AlarmCmd(ERTC_AlA, DISABLE);
    
    printf("Reset the timer !\r\n");
    
    /* Clear the WAKEUP EXTI  pending bit */
    EXTI_ClearIntPendingBit(GPIO_Pins_13);  
  }
}

/**
  * @brief  This function handles External line 0 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void)
{
  static char check = 0;

    if((EXTI_GetIntStatus(EXTI_Line0) != RESET))
  {
    if(check == 0)
    {
      /* Disable the ERTC Clock */
      RCC_ERTCCLKCmd(DISABLE);

      /* Wait for ERTC APB registers synchronisation */
      ERTC_WaitForSynchro();
      
			/* Disable the alarm */
			ERTC_AlarmCmd(ERTC_AlA, DISABLE);
			
      printf("stop the timer !\r\n");
      check =1;
    }
    else
    {
      /* Enable the ERTC Clock */
      RCC_ERTCCLKCmd(ENABLE);

      /* Wait for ERTC APB registers synchronisation */
      ERTC_WaitForSynchro();

      /* Enable the alarmA */
      ERTC_AlarmCmd(ERTC_AlA, ENABLE);
      
      printf("start the timer !\r\n");

      check =0;
    }

    /* Clear the LEFT EXTI  pending bit */
    EXTI_ClearIntPendingBit(EXTI_Line0);  
  }

    
    
}

/**
  * @brief  This function handles ERTC Alarm interrupt (A and B) request.
  * @param  None
  * @retval None
  */
void ERTCAlarm_IRQHandler(void)
{
  uint32_t tmp = 0;
  
  /* Check on the Alarm A flag and on the number of interrupts per Second (60*8) */
  if(ERTC_GetINTStatus(ERTC_INT_ALA) != RESET) 
  { 
    /* Clear ERTC AlarmA Flags */
    ERTC_ClearINTPendingBINT(ERTC_INT_ALA);
    if(uwERTCAlarmCount != 480)
    {
      /* Increment the counter of Alarm A interrupts */
      uwERTCAlarmCount++;

      /* Define the rate of Progress bar */
      tmp = (uwERTCAlarmCount * 100)/ 480; 

      /* Display Char on the LCD : XXX% */
      printf("%c",(tmp / 100) +0x30);
      printf("%c",((tmp  % 100 ) / 10) +0x30);
      printf("%c",(tmp % 10) +0x30);
      printf("%c",0x25);
			printf("\r\n");
    }
    else
    {
      /* Disable the ERTC Clock */
      RCC_ERTCCLKCmd(DISABLE);
    }
  }
  /* Clear the EXTI line 17 */
  EXTI_ClearIntPendingBit(EXTI_Line17);
  
}


/**
  * @}
  */ 

/**
  * @}
  */ 

