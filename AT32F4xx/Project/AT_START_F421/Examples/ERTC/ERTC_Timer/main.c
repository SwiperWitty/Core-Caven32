/**
  ******************************************************************************
  * File   : ERTC/ERTC_Timer/main.c 
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

/** @addtogroup ERTC_Timer
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MESSAGE1   "*** Progress Bar ***" 
#define MESSAGE2   "WAKEUP   Stop \\ Start" 
#define MESSAGE3   "TAMPER   Reset" 
    
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ERTC_InitType  ERTC_InitStructure;
ERTC_TimeType  ERTC_TimeStruct;

/* Private function prototypes -----------------------------------------------*/
void Enable_Button_Int(void);
static void ERTC_Config(void);
static void ERTC_AlarmConfig(void);

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
	
	/* Enable the Button interrupt.*/
  Enable_Button_Int();
  /* initialize UART1  */
  UART_Print_Init(115200);
   
  /* Displays MESSAGE1 on line 0 */
  printf(MESSAGE1);
  printf("\r\n");
	
	 /* Displays MESSAGE2 and MESSAGE3 on the LCD */
  printf(MESSAGE2); 
  printf("\r\n");
  printf(MESSAGE3);  
  printf("\r\n");
	
  /* ERTC configuration */
  ERTC_Config();

  /* Configure ERTC alarm A register to generate 8 interrupts per 1 Second */
  ERTC_AlarmConfig();

  /* Infinite loop */
  while (1)
  {}
}

/**
  * @brief  Configures the ERTC peripheral and select the clock source.
  * @param  None
  * @retval None
  */
static void ERTC_Config(void)
{
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_PWR, ENABLE);

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
  
  /* Deinitializes the ERTC registers */  
  ERTC_Reset();  
  
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
  * @brief  Enable the Button interrupt.
  * @param  None
  * @retval None
  */
void Enable_Button_Int(void)
{
  GPIO_InitType GPIO_InitStructure;
  NVIC_InitType  NVIC_InitStructure;
  EXTI_InitType  EXTI_InitStructure;
 
  /* Enable the SYSCFG clock */    
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_SYSCFGCOMP, ENABLE);
  
  /*Enable the Button Clock*/
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA | RCC_AHBPERIPH_GPIOC, ENABLE);

  /* Connect Button EXTI Line to Button GPIO Pin */ 
  SYSCFG_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinsSource13);	
  SYSCFG_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinsSource0);	
  
  /* EXTI configuration *******************************************************/
  EXTI_ClearIntPendingBit(EXTI_Line0);
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineEnable = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* Enable the ERTC Alarm Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
    
  /*Configure Button pin as input with pull-up/pull-down*/
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins     = GPIO_Pins_13;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;  
  GPIO_InitStructure.GPIO_Mode     = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Pull     = GPIO_Pull_PD;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
    
  /* EXTI configuration *******************************************************/
  EXTI_ClearIntPendingBit(EXTI_Line13);
  EXTI_InitStructure.EXTI_Line = EXTI_Line13;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineEnable = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* Enable the ERTC Alarm Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

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
  ERTC_AlarmStructure.ERTC_AlarmTime.ERTC_Seconds =0x0;
  ERTC_SetAlarmValue(ERTC_Format_BCD, ERTC_AlA, &ERTC_AlarmStructure);
  
  /* Set alarm A sub seconds and enable SubSec Alarm : generate 8 interrupts per Second */
  ERTC_AlarmSubSecondConfig(ERTC_AlA, 0xFF, ERTC_AlarmSubSecondMask_SBS14_5);

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

