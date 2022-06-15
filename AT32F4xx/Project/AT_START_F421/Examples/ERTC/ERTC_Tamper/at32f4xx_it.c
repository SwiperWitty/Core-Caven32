/**
  ******************************************************************************
  * File   : ERTC/ERTC_Tamper/at32f4xx_it.c 
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

/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */

/** @addtogroup ERTC_Tamper
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
extern uint32_t IsBackupRegReset(void);
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
  * @brief  This function handles ERTC Tamper and Time Stamp interrupts requests.
  * @param  None
  * @retval None
  */
void ERTC_IRQHandler(void)
{
  if(ERTC_GetFlagStatus(ERTC_FLAG_TP1F) != RESET)
  { 
    /* Tamper 1 detection event occurred */
    /* Check if ERTC Backup Data registers are cleared */
    if(IsBackupRegReset() == 0)
    {
      /* OK, ERTC Backup Data registers are reset as expected */
      /*LED3 Tooggle */
      AT32_LEDn_Toggle(LED3);
    }
    else
    {
      /* ERTC Backup Data registers are not reset */
      /* LED4 Tooggle */
      AT32_LEDn_Toggle(LED4);
    }
    
    /* Clear Tamper 1 pin Event pending flag */
    ERTC_ClearFlag(ERTC_FLAG_TP1F);

    /* Disable Tamper pin 1 */
    ERTC_TamperCmd(ERTC_TAMP_1, DISABLE);

    /* Enable Tamper pin */
    ERTC_TamperCmd(ERTC_TAMP_1, ENABLE); 
    
    printf("Tamper\r\n");
  }
}



/**
  * @}
  */ 

/**
  * @}
  */ 

