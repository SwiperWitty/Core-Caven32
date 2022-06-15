/**
  ******************************************************************************
  * File   : ERTC/ERTC_Calendar/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "at32_board.h"
#include <stdio.h>

/** @addtogroup AT32F415_StdPeriph_Examples
  * @{
  */

/** @addtogroup ERTC_Calendar
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Uncomment the corresponding line to select the ERTC Clock source */
#define ERTC_CLOCK_SOURCE_LSE          /* LSE used as ERTC source clock */
//#define ERTC_CLOCK_SOURCE_LSI         /* LSI used as ERTC source clock. The ERTC Clock may varies due to LSI frequency dispersion. */ 

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ERTC_TimeType  ERTC_TimeStructure;
ERTC_InitType  ERTC_InitStructure;
ERTC_AlarmType ERTC_AlarmStructure;

__IO uint32_t uwAsynchPrediv = 0;
__IO uint32_t uwSynchPrediv = 0;
  uint8_t aShowTime[50] = {0};
  
/* Private function prototypes -----------------------------------------------*/
static void ERTC_Config(void);
void ERTC_AlarmShow(void);
void ERTC_TimeShow(void);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  NVIC_InitType  NVIC_InitStructure;
  EXTI_InitType  EXTI_InitStructure;
  int temp=0;
  /* AT Board Initial */
  AT32_Board_Init();
  
  /* initialize UART1  */
  UART_Print_Init(115200);

  if (ERTC_ReadBackupRegister(ERTC_BKP_DT0) != 0x32F1)
  {  
    /* ERTC configuration  */
    ERTC_Config();
    /* Display the ERTC Time and Alarm */
    ERTC_TimeShow();
    ERTC_AlarmShow();
  }
  else
  {
    /* Check if the Power On Reset flag is set */
    if (RCC_GetFlagStatus(RCC_FLAG_PORST) != RESET)
    {
      /* Power On Reset occurred     */
      AT32_LEDn_ON(LED3);
    }
    /* Check if the Pin Reset flag is set */
    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
    {
      /* External Reset occurred */
      AT32_LEDn_ON(LED4);
    }
    
    /* Enable the PWR clock */
    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_PWR, ENABLE);

    /* Allow access to ERTC */
    PWR_BackupAccessCtrl(ENABLE);

    /* Wait for ERTC APB registers synchronisation */
    ERTC_WaitForSynchro();

    /* Clear the ERTC Alarm Flag */
    ERTC_ClearFlag(ERTC_FLAG_ALAF);

    /* Clear the EXTI Line 17 Pending bit (Connected internally to ERTC Alarm) */
    EXTI_ClearIntPendingBit(EXTI_Line17);

    /* Display the ERTC Time and Alarm */
    ERTC_TimeShow();
    ERTC_AlarmShow();
  }
   
  /* ERTC Alarm A Interrupt Configuration */
  /* EXTI configuration *******************************************************/
  EXTI_ClearIntPendingBit(EXTI_Line17);
  EXTI_InitStructure.EXTI_Line = EXTI_Line17;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineEnable = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* Enable the ERTC Alarm Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = ERTCAlarm_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  while (1)
  {
    ERTC_GetTimeValue(ERTC_Format_BIN, &ERTC_TimeStructure);
    if(temp != ERTC_TimeStructure.ERTC_Seconds)
    {
      temp = ERTC_TimeStructure.ERTC_Seconds;
      /* Display time Format : hh:mm:ss */
      printf("%0.2d:%0.2d:%0.2d\r\n",ERTC_TimeStructure.ERTC_Hours, ERTC_TimeStructure.ERTC_Minutes, ERTC_TimeStructure.ERTC_Seconds);
    }
  }
}

/**
  * @brief  Configure the ERTC peripheral by selecting the clock source.
  * @param  None
  * @retval None
  */
static void ERTC_Config(void)
{
  ERTC_DateType ERTC_DateStructure;
  
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_PWR, ENABLE);

  /* Allow access to ERTC */
  PWR_BackupAccessCtrl(ENABLE);
   
  /* Reset ERTC Domain */
  RCC_BackupResetCmd(ENABLE);
  RCC_BackupResetCmd(DISABLE);
  
#if defined (ERTC_CLOCK_SOURCE_LSI)  /* LSI used as ERTC source clock*/
  /* The ERTC Clock may varies due to LSI frequency dispersion */   
  /* Enable the LSI OSC */ 
  RCC_LSICmd(ENABLE);

  /* Wait till LSI is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSISTBL) == RESET)
  {
  }

  /* Select the ERTC Clock Source */
  RCC_ERTCCLKConfig(RCC_ERTCCLKSelection_LSI);
  
  /* ck_spre(1Hz) = ERTCCLK(LSI) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)*/
  uwSynchPrediv = 0xFF;
  uwAsynchPrediv = 0x7F;

#elif defined (ERTC_CLOCK_SOURCE_LSE) /* LSE used as ERTC source clock */
  /* Enable the LSE OSC */
  RCC_LSEConfig(RCC_LSE_ENABLE);

  /* Wait till LSE is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSESTBL) == RESET)
  {
  }

  /* Select the ERTC Clock Source */
  RCC_ERTCCLKConfig(RCC_ERTCCLKSelection_LSE);
  /* ck_spre(1Hz) = ERTCCLK(LSE) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)*/
  uwSynchPrediv = 0xFF;
  uwAsynchPrediv = 0x7F;
#endif /* ERTC_CLOCK_SOURCE_LSI */
  
  /* Enable the ERTC Clock */
  RCC_ERTCCLKCmd(ENABLE);

  /* Deinitializes the ERTC registers */  
  ERTC_Reset();  
  
  /* Wait for ERTC APB registers synchronisation */
  ERTC_WaitForSynchro();
  
  /* Configure the ERTC data register and ERTC prescaler */
  ERTC_InitStructure.ERTC_AsynchPrediv = uwAsynchPrediv;
  ERTC_InitStructure.ERTC_SynchPrediv = uwSynchPrediv;
  ERTC_InitStructure.ERTC_HourFormat = ERTC_HourFormat_24;
  ERTC_Init(&ERTC_InitStructure);
  
  /* Set the alarm 05h:20min:30s */
  ERTC_AlarmStructure.ERTC_AlarmTime.ERTC_AMPM    = ERTC_H12_AM;
  ERTC_AlarmStructure.ERTC_AlarmTime.ERTC_Hours   = 0x05;
  ERTC_AlarmStructure.ERTC_AlarmTime.ERTC_Minutes = 0x20;
  ERTC_AlarmStructure.ERTC_AlarmTime.ERTC_Seconds = 0x30;
  ERTC_AlarmStructure.ERTC_AlarmDateWeek = 0x31;
  ERTC_AlarmStructure.ERTC_AlarmDateWeekSel = ERTC_AlarmDateWeekSel_Date;
  ERTC_AlarmStructure.ERTC_AlarmMask = ERTC_AlarmMask_DateWeek;
  
  /* Configure the ERTC Alarm A register */
  ERTC_SetAlarmValue(ERTC_Format_BCD, ERTC_AlA, &ERTC_AlarmStructure);
  
  /* Enable ERTC Alarm A Interrupt */
  ERTC_INTConfig(ERTC_INT_ALA, ENABLE);
  
  /* Enable the alarm */
  ERTC_AlarmCmd(ERTC_AlA, ENABLE);
  
  ERTC_ClearFlag(ERTC_FLAG_ALAF);
  
  /* Set the date: Friday January 11th 2013 */
  ERTC_DateStructure.ERTC_Year = 0x13;
  ERTC_DateStructure.ERTC_Month = ERTC_Month_JAN;
  ERTC_DateStructure.ERTC_Date = 0x11;
  ERTC_DateStructure.ERTC_WeekDay = ERTC_Week_SAT;
  ERTC_SetDateValue(ERTC_Format_BCD, &ERTC_DateStructure);
  
  /* Set the time to 05h 20mn 00s AM */
  ERTC_TimeStructure.ERTC_AMPM    = ERTC_H12_AM;
  ERTC_TimeStructure.ERTC_Hours   = 0x05;
  ERTC_TimeStructure.ERTC_Minutes = 0x20;
  ERTC_TimeStructure.ERTC_Seconds = 0x00; 
  
  ERTC_SetTimeValue(ERTC_Format_BCD, &ERTC_TimeStructure);   
  
  /* Indicator for the ERTC configuration */
  ERTC_WriteBackupRegister(ERTC_BKP_DT0, 0x32F1);
}

/**
  * @brief  Display the current time.
  * @param  None
  * @retval None
  */
void ERTC_TimeShow(void)
{
  /* Get the current Time */
  ERTC_GetTimeValue(ERTC_Format_BIN, &ERTC_TimeStructure);
  /* Display time Format : hh:mm:ss */
  printf("%0.2d:%0.2d:%0.2d\r\n",ERTC_TimeStructure.ERTC_Hours, ERTC_TimeStructure.ERTC_Minutes, ERTC_TimeStructure.ERTC_Seconds);
}

/**
  * @brief  Display the current Alarm.
  * @param  None
  * @retval None
  */
void ERTC_AlarmShow(void)
{
  /* Get the current Alarm */
  ERTC_GetAlarmValue(ERTC_Format_BIN, ERTC_AlA, &ERTC_AlarmStructure);
  printf("Alarm %0.2d:%0.2d:%0.2d\r\n", ERTC_AlarmStructure.ERTC_AlarmTime.ERTC_Hours, ERTC_AlarmStructure.ERTC_AlarmTime.ERTC_Minutes, ERTC_AlarmStructure.ERTC_AlarmTime.ERTC_Seconds);
}

/**
  * @}
  */ 

/**
  * @}
  */ 

