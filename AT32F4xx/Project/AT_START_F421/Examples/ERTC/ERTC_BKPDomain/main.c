/**
  ******************************************************************************
  * File   : ERTC/ERTC_BKPDomain//main.c 
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
/** @addtogroup ERTC_BKPDomain
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Uncomment the corresponding line to select the ERTC Clock source */
#define ERTC_CLOCK_SOURCE_LSE           /* LSE used as ERTC source clock */
/*#define ERTC_CLOCK_SOURCE_LSI */     /* LSI used as ERTC source clock. The ERTC Clock
                                          may varies due to LSI frequency dispersion. */
#define ERTC_BKP_DR_NUMBER   5
#define FIRST_DATA          0x32F2

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ERTC_InitType   ERTC_InitStructure;
ERTC_TimeType   ERTC_TimeStructure;
ERTC_DateType   ERTC_DateStructure;

__IO uint32_t uwAsynchPrediv = 0;
__IO uint32_t uwSynchPrediv = 0;
__IO uint32_t uwTimeDisplay = 0;

uint32_t uwErrorIndex = 0;
uint32_t uwIndex = 0;

uint32_t aBKPDataReg[ERTC_BKP_DR_NUMBER] =
  {
    ERTC_BKP_DT0, ERTC_BKP_DT1, ERTC_BKP_DT2, 
    ERTC_BKP_DT3, ERTC_BKP_DT4
  };

/* Private function prototypes -----------------------------------------------*/
static void     ERTC_Config(void);
static void     Time_Adjust(void);
static void     Time_Display(void);
static void     Date_Adjust(void);
static void     Date_Display(void);
static void     Calendar_Show(void);
static void     WriteToBackupReg(uint16_t FirstBackupData);
static uint32_t CheckBackupReg(uint16_t FirstBackupData);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{     
  NVIC_InitType NVIC_InitStructure;
  EXTI_InitType  EXTI_InitStructure;

  /* AT Board Initial */
  AT32_Board_Init();
  
  /* initialize UART1  */
  UART_Print_Init(115200);
  
  /* Configure the Module */
  printf("ERTC Backup Domain Example\r\n");

  /* Display the default RCC BDCR and ERTC TPAF Registers */
  printf("Entry Point \r\n");
  printf("RCC BDCR = 0x%x\r\n", RCC->BDC);
  printf("ERTC TPAF = 0x%x\r\n", ERTC->TPAF);

  /* Enable the PWR APB1 Clock Interface */
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_PWR, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCtrl(ENABLE);

  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  /* Enable the ERTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = ERTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* EXTI configuration */
  EXTI_ClearIntPendingBit(EXTI_Line17);
  EXTI_InitStructure.EXTI_Line = EXTI_Line17;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineEnable = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  if(ERTC_ReadBackupRegister(ERTC_BKP_DT0) != FIRST_DATA)
  {
    printf("ERTC Config PLZ Wait. \r\n");

    /* ERTC Configuration */
    ERTC_Config();

    /* Adjust Current Time */
    Time_Adjust();

    /* Adjust Current Date */
    Date_Adjust();
  }
  else
  {
    /* Wait for ERTC APB registers synchronisation */
    ERTC_WaitForSynchro();
    ERTC_ClearINTPendingBINT(ERTC_INT_ALA);
    EXTI_ClearIntPendingBit(EXTI_Line17);

    /* ERTC Backup Data Registers **************************************************/
    /* Check if ERTC Backup DRx registers data are correct */
    if (CheckBackupReg(FIRST_DATA) == 0x00)
    { 
      /* OK, ERTC Backup DRx registers data are correct */
      printf("OK, ERTC Backup DTx registers data are correct. \n");
    }
    else
    { 
      /* Error, ERTC Backup DRx registers data are not correct */
      printf("ERTC Backup DTx registers data are not correct\n");
    }
  }

  while (1)
  {
    /* Infinite loop */
    Calendar_Show();
  }
}

/**
  * @brief  Configure the ERTC peripheral by selecting the clock source.
  * @param  None
  * @retval None
  */
static void ERTC_Config(void)
{
  ERTC_AlarmType ERTC_AlarmStruct;
  
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_PWR, ENABLE);

  /* Allow access to ERTC */
  PWR_BackupAccessCtrl(ENABLE);
    
  /* Reset ERTC Domain */
  RCC_BackupResetCmd(ENABLE);
  RCC_BackupResetCmd(DISABLE);
  
#if defined (ERTC_CLOCK_SOURCE_LSI)  /* LSI used as ERTC source clock*/
/* The ERTC Clock may varies due to LSI frequency dispersion. */
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

  /* Write to the first ERTC Backup Data Register */
  ERTC_WriteBackupRegister(ERTC_BKP_DT0, FIRST_DATA);

  /* Display the new RCC BDCR and ERTC TPAF Registers */
  printf("ERTC Reconfig \r\n");
  printf("RCC BDCR = 0x%x\r\n", RCC->BDC);
  printf("ERTC TPAF = 0x%x\r\n", ERTC->TPAF); 

  /* Set the Time */
  ERTC_TimeStructure.ERTC_Hours   = 0x08;
  ERTC_TimeStructure.ERTC_Minutes = 0x00;
  ERTC_TimeStructure.ERTC_Seconds = 0x00;

  /* Set the Date */
  ERTC_DateStructure.ERTC_Month = ERTC_Month_JAN;
  ERTC_DateStructure.ERTC_Date = 0x04;  
  ERTC_DateStructure.ERTC_Year = 0x19; 
  ERTC_DateStructure.ERTC_WeekDay = ERTC_Week_FRI; 

  /* Calendar Configuration */
  ERTC_InitStructure.ERTC_AsynchPrediv = uwAsynchPrediv;
  ERTC_InitStructure.ERTC_SynchPrediv =  uwSynchPrediv;
  ERTC_InitStructure.ERTC_HourFormat = ERTC_HourFormat_24;
  ERTC_Init(&ERTC_InitStructure);
  
  /* Set Current Time and Date */
  ERTC_SetTimeValue(ERTC_Format_BCD, &ERTC_TimeStructure);  
  ERTC_SetDateValue(ERTC_Format_BCD, &ERTC_DateStructure); 

  /* Configure the ERTC Alarm Clock source and Counter (Alarm event each 1 second) */
  ERTC_AlarmStruct.ERTC_AlarmDateWeekSel        = ERTC_AlarmDateWeekSel_Week;
  ERTC_AlarmStruct.ERTC_AlarmMask               = ERTC_AlarmMask_All;
  
  ERTC_AlarmStruct.ERTC_AlarmDateWeek           = 0;  
  ERTC_AlarmStruct.ERTC_AlarmTime.ERTC_AMPM     = 0;
  ERTC_AlarmStruct.ERTC_AlarmTime.ERTC_Hours    = 0;
  ERTC_AlarmStruct.ERTC_AlarmTime.ERTC_Minutes  = 0;
  ERTC_AlarmStruct.ERTC_AlarmTime.ERTC_Seconds  = 0;
  ERTC_SetAlarmValue(ERTC_Format_BIN, ERTC_AlA, &ERTC_AlarmStruct);
  
  /* Enable the Alarm Interrupt */
  ERTC_INTConfig(ERTC_INT_ALA, ENABLE);

  /* Enable Alarm Counter */
  ERTC_AlarmCmd(ERTC_AlA, ENABLE);
  

  if(uwErrorIndex)
  {
    printf("BKP SRAM Number of errors = %d\r\n", uwErrorIndex);
  }
  else
  {
    printf("BKP SRAM write OK \r\n");
  }


/* ERTC Backup Data Registers **************************************************/
  /* Write to ERTC Backup Data Registers */
  WriteToBackupReg(FIRST_DATA);
}

/**
  * @brief  Returns the time entered by user, using demo board keys.
  * @param  None
  * @retval None
  */
static void Time_Adjust(void)
{
  /* Display the current time */
  Time_Display();
}

/**
  * @brief  Displays the current time.
  * @param  None
  * @retval None
  */
static void Time_Display(void)
{
  /* Get the current Time */
  ERTC_GetTimeValue(ERTC_Format_BIN, &ERTC_TimeStructure);
  
  /* Display time hours */
  printf("%c",((ERTC_TimeStructure.ERTC_Hours / 10) + 0x30));
  printf("%c",((ERTC_TimeStructure.ERTC_Hours % 10) + 0x30));
  printf(":");
  /* Display time minutes */
  printf("%c",((ERTC_TimeStructure.ERTC_Minutes /10) + 0x30));
  printf("%c",((ERTC_TimeStructure.ERTC_Minutes % 10) + 0x30));
  printf(":");
  /* Display time seconds */
  printf("%c",((ERTC_TimeStructure.ERTC_Seconds / 10) + 0x30));
  printf("%c",((ERTC_TimeStructure.ERTC_Seconds % 10) + 0x30));
  
  printf("\r\n");
}

/**
  * @brief  Returns the date entered by user, using demoboard keys.
  * @param  None
  * @retval None
  */
static void Date_Adjust(void)
{
  /* Display the current Date */
  Date_Display();
}

/**
  * @brief  Displays the current date.
  * @param  None
  * @retval None
  */
static void Date_Display(void)
{

  /* Get the current Date */
  ERTC_GetDateValue(ERTC_Format_BIN, &ERTC_DateStructure);

  /* Display Date WeekDay */
  printf("%c",((ERTC_DateStructure.ERTC_WeekDay) + 0x30));
  printf("/");
  /* Display Date Day */
  printf("%c",((ERTC_DateStructure.ERTC_Date /10) + 0x30));
  printf("%c",((ERTC_DateStructure.ERTC_Date % 10) + 0x30));
  printf("/");
  /* Display Date Month */
  printf("%c",((ERTC_DateStructure.ERTC_Month / 10) + 0x30));
  printf("%c",((ERTC_DateStructure.ERTC_Month % 10) + 0x30));
  printf("/");
  /* Display Date Year */
  printf("2");
  printf("0");
  printf("%c",((ERTC_DateStructure.ERTC_Year / 10) + 0x30));
  printf("%c",((ERTC_DateStructure.ERTC_Year % 10) + 0x30));
  
  printf("\r\n");
}

/**
  * @brief  Shows the current time and date on LCD.
  * @param  None
  * @retval None
  */
static void Calendar_Show(void)
{
  /* If 1s has elapsed */
  if (uwTimeDisplay == 1)
  {
    /* Display current time */
    Time_Display();
    
    /* Display current date */
    Date_Display();
    
    uwTimeDisplay = 0;
  }
}

/**
  * @brief  Writes data to all Backup data registers.
  * @param  FirstBackupData: data to write to first backup data register.
  * @retval None
  */
static void WriteToBackupReg(uint16_t FirstBackupData)
{
  uint32_t index = 0;

  for (index = 0; index < ERTC_BKP_DR_NUMBER; index++)
  {
    ERTC_WriteBackupRegister(aBKPDataReg[index], FirstBackupData + (index * 0x5A));
  }

}

/**
  * @brief  Checks if the Backup data registers values are correct or not.
  * @param  FirstBackupData: data to read from first backup data register
  * @retval - 0: All Backup DRx registers data are correct
  *         - Value different from 0: Number of the first Backup register which 
  *           value is not correct
  */
static uint32_t CheckBackupReg(uint16_t FirstBackupData)
{
  uint32_t index = 0;

  for (index = 0; index < ERTC_BKP_DR_NUMBER; index++)
  {
    if (ERTC_ReadBackupRegister(aBKPDataReg[index]) != (FirstBackupData + (index * 0x5A)))
    {
      return (index + 1);
    }
  }

  return 0;
}


/**
  * @}
  */ 

/**
  * @}
  */ 

