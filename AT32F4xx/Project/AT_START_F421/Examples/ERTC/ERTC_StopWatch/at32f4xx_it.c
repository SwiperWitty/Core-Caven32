/**
  ******************************************************************************
  * File   : ERTC/ERTC_StopWatch/at32f4xx_it.c 
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
#include "stdio.h"

/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */

/** @addtogroup ERTC_StopWatch
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MESSAGE1   "Push Wakeup to start"
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
typedef struct
{
  uint8_t tab[12];
}Table_TypeDef;

uint32_t uwBackupindex = 0;
__IO uint8_t  ubERTCCount = 0;
__IO uint32_t  ubSSecondfraction = 0;
extern __IO uint8_t ubStartevent;
ERTC_TimeType ERTC_StampTimeStruct;
ERTC_TimeType ERTC_TimeStructure;

/* Define the backup register */
uint32_t aBKPDataReg[5] = { ERTC_BKP_DT0, ERTC_BKP_DT1, ERTC_BKP_DT2,ERTC_BKP_DT3, ERTC_BKP_DT4};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
extern void ERTC_Time_display(Table_TypeDef table);
extern Table_TypeDef ERTC_Get_Time(uint32_t Secondfraction , ERTC_TimeType* ERTC_TimeStructure);
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
  * @brief  This function handles External line 0 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI1_0_IRQHandler(void)
{
	static char check = 0;
		
  if((EXTI_GetIntStatus(EXTI_Line0) != RESET) )
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
    /* start count */
		uwBackupindex = 0;
		
    ubStartevent = 0;
  }
  /* Clear the Wakeup EXTI pending bit */
  EXTI_ClearIntPendingBit(EXTI_Line0);  
	
}  

/**
  * @brief  This function handles External lines 15 to 10 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI15_4_IRQHandler(void)
{
  if((EXTI_GetIntStatus(EXTI_Line13) != RESET) && (ubStartevent !=1) && (uwBackupindex < 3))
  {
    if (uwBackupindex < 2)
    {
      /* Increment counter */
      ubERTCCount++;
      
      /* Get the Current sub seconds and time */
      ubSSecondfraction = 1000 - ((uint32_t)((uint32_t)ERTC_GetSubSecondValue() * 1000) / (uint32_t)0x3FF);
      ERTC_GetTimeValue(ERTC_Format_BCD, &ERTC_StampTimeStruct);
      
			printf("The Storage value is : \r\n");
      /* Display result on the LCD */
      ERTC_Time_display(ERTC_Get_Time(ubSSecondfraction , &ERTC_StampTimeStruct)); 
			printf("\r\n");
      
      /* Save time register  to Backup register (the first 10 registers are reserved for time) */
      ERTC_WriteBackupRegister(aBKPDataReg[uwBackupindex],(uint32_t)ERTC->TIME);
      
      /* Save sub second time stamp register (the latest 10 registers are reserved for sub second) */
      ERTC_WriteBackupRegister(aBKPDataReg[uwBackupindex + 2], ubSSecondfraction);
    }
    else
    {      
      
      /* Reset Counters */
      ubERTCCount = 0;
      uwBackupindex = 0 ;
      
      /* Enter to idle */
      ubStartevent =1;
      
      /* Set the time to 00h 00mn 00s AM */
      ERTC_TimeStructure.ERTC_AMPM     = ERTC_H12_AM;
      ERTC_TimeStructure.ERTC_Hours   = 0;
      ERTC_TimeStructure.ERTC_Minutes = 0;
      ERTC_TimeStructure.ERTC_Seconds = 0;  
      ERTC_SetTimeValue(ERTC_Format_BCD, &ERTC_TimeStructure);
      
      /* Disable the ERTC Clock */
      RCC_ERTCCLKCmd(DISABLE);
			
      /* Display message to the LCD */
			printf(MESSAGE1);
      
      /* Clear EXTI line 19 */
      EXTI_ClearIntPendingBit(EXTI_Line19);
      
      /* Clear Tamper pin interrupt pending bit */
      ERTC_ClearINTPendingBINT(ERTC_INT_TAMP1);
    }  
    
    uwBackupindex++;
  }
  /* Clear the TAMPER EXTI pending bit */
  EXTI_ClearIntPendingBit(EXTI_Line13);     
}


/**
  * @}
  */ 

/**
  * @}
  */ 

