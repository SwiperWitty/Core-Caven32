/**
  ******************************************************************************
  * File   : ERTC/ERTC_LSI/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "at32_board.h"
#include <stdio.h>

/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */

/** @addtogroup ERTC_LSI
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ERTC_InitType ERTC_InitStructure;
__IO uint32_t   uwLsiFreq = 40000;
__IO uint32_t   uwCaptureNumber = 0; 
__IO uint32_t   uwPeriodValue = 0;

/* Private function prototypes -----------------------------------------------*/
static void     ERTC_Config(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* AT Board Initial */
  AT32_Board_Init();
  
  /* initialize UART1  */
  UART_Print_Init(115200);
  
  /* ERTC Configuration */
  ERTC_Config();

  /* Turn on LED2 */
  AT32_LEDn_ON(LED2);

  /* Calendar Configuration */
  /* ck_spre(1Hz) = ERTCCLK(LSI) /(AsynchPrediv + 1)*(SynchPrediv + 1)*/
  ERTC_InitStructure.ERTC_AsynchPrediv = 0x7F;
  ERTC_InitStructure.ERTC_SynchPrediv	= (uwLsiFreq/128) - 1;
  ERTC_InitStructure.ERTC_HourFormat = ERTC_HourFormat_24;
  ERTC_Init(&ERTC_InitStructure);

  while (1)
  {
  }
}

/**
  * @brief  Configures the ERTC peripheral by selecting the clock source.
  * @param  None
  * @retval None
  */
static void ERTC_Config(void)
{
  NVIC_InitType  NVIC_InitStructure; 
  EXTI_InitType  EXTI_InitStructure;
  ERTC_AlarmType ERTC_AlarmStruct;
  
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_PWR, ENABLE);

  /* Allow access to ERTC */
  PWR_BackupAccessCtrl(ENABLE);
	
  /* Reset ERTC Domain */
  RCC_BackupResetCmd(ENABLE);
  RCC_BackupResetCmd(DISABLE);
	
  /* LSI used as ERTC source clock */
  /* The ERTC Clock may varies due to LSI frequency dispersion. */   
  /* Enable the LSI OSC */ 
  RCC_LSICmd(ENABLE);

  /* Wait till LSI is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSISTBL) == RESET)
  {
  }

  /* Select the ERTC Clock Source */
  RCC_ERTCCLKConfig(RCC_ERTCCLKSelection_LSI);
   
  /* Enable the ERTC Clock */
  RCC_ERTCCLKCmd(ENABLE);

  /* Deinitializes the ERTC registers */  
  ERTC_Reset();  
  
  /* Wait for ERTC APB registers synchronisation */
  ERTC_WaitForSynchro();

  /* Calendar Configuration with LSI supposed at 32KHz */
  ERTC_InitStructure.ERTC_AsynchPrediv = 0x7F;
  ERTC_InitStructure.ERTC_SynchPrediv  = 0xFF; /* (32KHz / 128) - 1 = 0xFF*/
  ERTC_InitStructure.ERTC_HourFormat = ERTC_HourFormat_24;
  ERTC_Init(&ERTC_InitStructure);  

  /* EXTI configuration *******************************************************/
  EXTI_ClearIntPendingBit(EXTI_Line17);
  EXTI_InitStructure.EXTI_Line = EXTI_Line17;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineEnable = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* Enable the ERTC Alarm Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = ERTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  

  /* Configure the ERTC Alarm Clock source and Counter (Alarm event each 1 second) */
  ERTC_AlarmStruct.ERTC_AlarmDateWeekSel        = ERTC_AlarmDateWeekSel_Week;
  ERTC_AlarmStruct.ERTC_AlarmMask               = ERTC_AlarmMask_All;
  
  ERTC_AlarmStruct.ERTC_AlarmDateWeek           = 0;  
  ERTC_AlarmStruct.ERTC_AlarmTime.ERTC_AMPM     = 0;
  ERTC_AlarmStruct.ERTC_AlarmTime.ERTC_Hours    = 0;
  ERTC_AlarmStruct.ERTC_AlarmTime.ERTC_Minutes  = 0;
  ERTC_AlarmStruct.ERTC_AlarmTime.ERTC_Seconds  = 0;
  ERTC_SetAlarmValue(ERTC_Format_BIN, ERTC_AlA, &ERTC_AlarmStruct);

  /*Clears the ERTC's interrupt pending bits.*/
  ERTC_ClearINTPendingBINT(ERTC_INT_ALA);

  /* Enable the ERTC Alarm Interrupt */
  ERTC_INTConfig(ERTC_INT_ALA, ENABLE);
  
  /* Enable Alarm Counter */
  ERTC_AlarmCmd(ERTC_AlA, ENABLE);
}


/**
  * @}
  */ 

/**
  * @}
  */ 

