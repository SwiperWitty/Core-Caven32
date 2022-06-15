/**
  ******************************************************************************
  * File   : ERTC/ERTC_TimeStamp/main.c 
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

/** @addtogroup ERTC_TimeStamp
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Uncomment the corresponding line to select the ERTC Clock source */
//#define ERTC_CLOCK_SOURCE_LSE            /* LSE used as ERTC source clock */
#define ERTC_CLOCK_SOURCE_LSI             /* LSI used as ERTC source clock. The ERTC Clock
                                           may varies due to LSI frequency dispersion. */ 
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ERTC_InitType ERTC_InitStructure;
ERTC_TimeType ERTC_TimeStructure;
ERTC_DateType ERTC_DateStructure;
ERTC_TimeType ERTC_TimeStampStructure;
ERTC_DateType ERTC_TimeStampDateStructure;

uint32_t uwAsynchPrediv = 0;
uint32_t uwSynchPrediv = 0;
uint32_t uwSecondfraction = 0;

/* Private function prototypes -----------------------------------------------*/
void         ERTC_TimeShow       (void);
void         ERTC_TimeStampShow  (void);
static void  ERTC_Config         (void);
static void  ERTC_TimeRegulate   (void);
void         ERTC_TimeStampConfig(void);
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
  
  printf("TimeStamp Example \r\n");
 
  if (ERTC_ReadBackupRegister(ERTC_BKP_DT0) != 0x32F2)
  {
    /* ERTC configuration  */
    ERTC_Config();

    /* Configure the time&date register */
    ERTC_TimeRegulate(); 
    
    /* Configure the TimeStamp register */
    ERTC_TimeStampConfig();
    
    /* Display the Date and Time */    
    ERTC_TimeShow();
    
  }
  else
  {
    /* Check if the Power On Reset flag is set */
    if (RCC_GetFlagStatus(RCC_FLAG_PORST) != RESET)
    {
      printf("Power On Reset occurred       \r\n");
    }
    /* Check if the Pin Reset flag is set */
    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
    {
      printf("External Reset occurred      \r\n");
    }

    printf("No need to configure ERTC     \r\n");
    
    /* Enable the PWR clock */
    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_PWR, ENABLE);

    /* Allow access to ERTC */
    PWR_BackupAccessCtrl(ENABLE);

    /* Configure the TimeStamp register */
    ERTC_TimeStampConfig();
    
    /* Wait for ERTC APB registers synchronisation */
    ERTC_WaitForSynchro();
 
    /* Display the Date and Time */    
    ERTC_TimeShow();
  }

  while (1)
  {
    
  }
}

/**
  * @brief  Configure the ERTC peripheral by selecting the clock source.
  * @param  None
  * @retval None
  */
static void ERTC_Config(void)
{
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_PWR, ENABLE);

  /* Allow access to ERTC */
  PWR_BackupAccessCtrl(ENABLE);

  /* Reset the Backup domain */
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

  /* Deinitializes the ERTC registers */  
  ERTC_Reset();  

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
  
  /* Wait for ERTC APB registers synchronisation */
  ERTC_WaitForSynchro();
  
  /* Configure the ERTC data register and ERTC prescaler */
  ERTC_InitStructure.ERTC_AsynchPrediv = uwAsynchPrediv;
  ERTC_InitStructure.ERTC_SynchPrediv = uwSynchPrediv;
  ERTC_InitStructure.ERTC_HourFormat = ERTC_HourFormat_24;
  
  /* Check on ERTC init */
  if (ERTC_Init(&ERTC_InitStructure) == ERROR)
  {
    printf("ERTC Prescaler Config failed!!\r\n" );
  }
}

/**
  * @brief  Configure the ERTC TimeStamp.
  * @param  None
  * @retval None
  */
void ERTC_TimeStampConfig(void)
{
  NVIC_InitType NVIC_InitStruct;
  EXTI_InitType EXTI_InitStruct;
   
  /* Configure NVIC */
  NVIC_InitStruct.NVIC_IRQChannel                   = TAMP_STAMP_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority        = 0;
  NVIC_Init(&NVIC_InitStruct);

  /* Configure EXTI */  
  EXTI_InitStruct.EXTI_Line       = EXTI_Line21;
  EXTI_InitStruct.EXTI_LineEnable = ENABLE;
  EXTI_InitStruct.EXTI_Mode       = EXTI_Mode_Interrupt;
  EXTI_InitStruct.EXTI_Trigger    = EXTI_Trigger_Rising;
  EXTI_Init(&EXTI_InitStruct);
  
  /* Enable The TimeStamp */
  ERTC_TimeStampCmd(ERTC_TimeStampEdge_Falling, ENABLE);
  
  /* Enable The TimeStamp INT */  
  ERTC_INTConfig(ERTC_INT_TS, ENABLE);  
}

/**
  * @brief  Display the current date on the Hyperterminal.
  * @param  None
  * @retval None
  */
void ERTC_TimeShow(void)
{
  /* Get the current Date */
  ERTC_GetDateValue(ERTC_Format_BIN, &ERTC_DateStructure);
  
  /* Get the current Time and Date */
  ERTC_GetTimeValue(ERTC_Format_BIN, &ERTC_TimeStructure);
  
  /* Display the curent time*/  
  printf("\r\nCurrent Time Display \r\n"); 
  
  printf("%02d-%02d-%02d ",ERTC_DateStructure.ERTC_Year,ERTC_DateStructure.ERTC_Month,ERTC_DateStructure.ERTC_Date);        
  printf(" %02d:%02d:%02d\r\n",ERTC_TimeStructure.ERTC_Hours,ERTC_TimeStructure.ERTC_Minutes,ERTC_TimeStructure.ERTC_Seconds);  
}

/**
  * @brief  Display the current TimeStamp (time and date) on the Hyperterminal.
  * @param  None
  * @retval None
  */
void ERTC_TimeStampShow(void)
{
  /* Get the current TimeStamp */
  ERTC_GetTimeStamp(ERTC_Format_BIN, &ERTC_TimeStampStructure, &ERTC_TimeStampDateStructure);

  /* Display the curent time*/
  printf("\r\nTimeStamp Display \r\n");
  
  printf("   %02d-%02d ",ERTC_TimeStampDateStructure.ERTC_Month, ERTC_TimeStampDateStructure.ERTC_Date);        
  printf(" %02d:%02d:%02d\r\n",ERTC_TimeStampStructure.ERTC_Hours, ERTC_TimeStampStructure.ERTC_Minutes, ERTC_TimeStampStructure.ERTC_Seconds);
}

/**
  * @brief  Returns the time entered by user, using Hyperterminal.
  * @param  None
  * @retval None
  */
static void ERTC_TimeRegulate(void)
{
  /* Set Time hh:mm:ss */
  ERTC_TimeStructure.ERTC_AMPM     = ERTC_H12_AM;
  ERTC_TimeStructure.ERTC_Hours   = 0x08;  
  ERTC_TimeStructure.ERTC_Minutes = 0x10;
  ERTC_TimeStructure.ERTC_Seconds = 0x00;
  ERTC_SetTimeValue(ERTC_Format_BCD, &ERTC_TimeStructure);

  /* Set Date Week/Date/Month/Year */
  ERTC_DateStructure.ERTC_WeekDay = 5;
  ERTC_DateStructure.ERTC_Date  = 0x4;
  ERTC_DateStructure.ERTC_Month = 0x1;
  ERTC_DateStructure.ERTC_Year  = 0x19;
  ERTC_SetDateValue(ERTC_Format_BCD, &ERTC_DateStructure);
  
  /* Write BkUp DR0 */
  ERTC_WriteBackupRegister(ERTC_BKP_DT0, 0x32F2);
}


/**
  * @}
  */ 

/**
  * @}
  */ 

