 /**
  **************************************************************************
  * File   : PWR/STOP/main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  **************************************************************************
  */
 
 /* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "at32f4xx.h"
#include "at32_board.h"

/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */

/** @addtogroup PWR_STOP
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ERTC_CLOCK_SOURCE_LSI
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
GPIO_InitType GPIO_InitStructure;
EXTI_InitType EXTI_InitStructure;
NVIC_InitType NVIC_InitStructure = {0};
ErrorStatus HSEStartUpStatus;
ERTC_InitType  ERTC_InitStructure;
ERTC_TimeType  ERTC_TimeStruct;

/* Private function prototypes -----------------------------------------------*/
void SYSCLKConfig_STOP(void);
void EXTI_Configuration(void);
void NVIC_Configuration(void);
static void ERTC_Config(void);
static void ERTC_AlarmConfig(void);

/* Private functions ---------------------------------------------------------*/

/**
 *	@brief  main function
 *	@param  None
 *	@retval None
 */
int main(void)
{
  uint32_t i;
  GPIO_StructInit(&GPIO_InitStructure);
  EXTI_StructInit(&EXTI_InitStructure);      
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_PWR, ENABLE);
  AT32_Board_Init();      ///<Initialize LEDs
  UART_Print_Init(115200);
  AT32_LEDn_ON(LED2);
  AT32_LEDn_ON(LED3);
  AT32_LEDn_ON(LED4);
  ERTC_Config();
  ERTC_AlarmConfig();
  while (1)
  {
    SysTick->CTRL &= 0xFFFFFFFE;
    AT32_LEDn_OFF(LED2);                             ///<Turn off LED2

    PWR_EnterSTOPMode(PWR_Regulator_LowPower_Extra, PWR_STOPEntry_WFI);///Request to enter STOP mode with regulator in low power mode
    SysTick->CTRL |= 0x1;
    SYSCLKConfig_STOP();                             ///<Configures system clock after wake-up from STOP
    AT32_LEDn_ON(LED2);                              ///<Turn on LED2
    for(i=0;i<500000;i++);
  }
}

/**
  * @brief  Configures system clock after wake-up from STOP: enable HSE, PLL and select PLL as system clock source.
  * @param  None
  * @retval None
  */
void SYSCLKConfig_STOP(void)
{
  uint32_t i;
  RCC_HSEConfig(RCC_HSE_ENABLE);   ///<Enable HSE
  for(i=0;i<20000;i++);
  HSEStartUpStatus = RCC_WaitForHSEStable();
  if(HSEStartUpStatus == SUCCESS)
  {
    RCC_PLLCmd(ENABLE);   ///<Enable PLL
    while(RCC_GetFlagStatus(RCC_FLAG_PLLSTBL) == RESET)  ///<Wait till PLL is ready
    {
    }
    RCC_SYSCLKConfig(RCC_SYSCLKSelction_PLL);            ///<Select PLL as system clock source
    while(RCC_GetSYSCLKSelction() != 0x08)               ///<Wait till PLL is used as system clock source
    {
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
  /* Allow access to ERTC */
  PWR_BackupAccessCtrl(ENABLE);

  /* Reset ERTC Domain */
  RCC_BackupResetCmd(ENABLE);
  RCC_BackupResetCmd(DISABLE);

  /* Enable the LSE OSC */
  RCC_LSEConfig(RCC_LSE_ENABLE);

  /* Wait till LSE is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSESTBL) == RESET)
  {
  }

  /* Select the ERTC Clock Source */
  RCC_ERTCCLKConfig(RCC_ERTCCLKSelection_LSE);


  /* Enable the ERTC Clock */
  RCC_ERTCCLKCmd(ENABLE);

  /* Wait for ERTC APB registers synchronisation */
  ERTC_WaitForSynchro();

  /* Configure the ERTC data register and ERTC prescaler */
  /* ck_spre(1Hz) = ERTCCLK(LSI) /(AsynchPrediv + 1)*(SynchPrediv + 1)*/
  ERTC_InitStructure.ERTC_AsynchPrediv = 0x7F;
  ERTC_InitStructure.ERTC_SynchPrediv  = 0xFF;
  ERTC_InitStructure.ERTC_HourFormat   = ERTC_HourFormat_24;
  ERTC_Init(&ERTC_InitStructure);

  /* Set the time to 00h 00mn 00s AM */
  ERTC_TimeStruct.ERTC_AMPM     = ERTC_H12_AM;
  ERTC_TimeStruct.ERTC_Hours   = 0;
  ERTC_TimeStruct.ERTC_Minutes = 0;
  ERTC_TimeStruct.ERTC_Seconds = 0;  
  ERTC_SetTimeValue(ERTC_Format_BCD, &ERTC_TimeStruct);
}

/**
  * @brief  Configures the ERTC Alarm.
  * @param  None
  * @retval None
  */
static void ERTC_AlarmConfig(void)
{
  EXTI_InitType EXTI_InitStructure;
  ERTC_AlarmType ERTC_AlarmStructure;
  NVIC_InitType NVIC_InitStructure;
  
  /* EXTI configuration */
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
 
  /* Set the alarm A Masks */
  ERTC_AlarmStructure.ERTC_AlarmMask = ERTC_AlarmMask_All;
  ERTC_AlarmStructure.ERTC_AlarmDateWeekSel = ERTC_AlarmDateWeekSel_Date;
  ERTC_AlarmStructure.ERTC_AlarmDateWeek = ERTC_Week_MON;
  ERTC_AlarmStructure.ERTC_AlarmTime.ERTC_Hours = 0x0;
  ERTC_AlarmStructure.ERTC_AlarmTime.ERTC_Minutes =0x0;
  ERTC_AlarmStructure.ERTC_AlarmTime.ERTC_Seconds =0x8;
  ERTC_SetAlarmValue(ERTC_Format_BCD, ERTC_AlA, &ERTC_AlarmStructure);
  
  /* Set alarm A sub seconds and enable SubSec Alarm : generate 8 interrupts per Second */
//  ERTC_AlarmSubSecondConfig(ERTC_AlA, 0xFF, ERTC_AlarmSubSecondMask_SBS14_5);

  /* Enable alarm A interrupt */
  ERTC_INTConfig(ERTC_INT_ALA, ENABLE);
	
	/* Enable the alarmA */
  ERTC_AlarmCmd(ERTC_AlA, ENABLE);
}


/**
  * @}
  */ 

/**
  * @}
  */ 

