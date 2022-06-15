/**
  ******************************************************************************
  * File   : ERTC/ERTC_StopWatch/main.c 
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

/** @addtogroup ERTC_StopWatch
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  uint8_t tab[12];
}Table_TypeDef;
/* Private define ------------------------------------------------------------*/
#define MESSAGE1   "**** STOPWATCH ****        " 
#define MESSAGE2   "WAKEUP : Stop \\ Start       " 
#define MESSAGE3   "TAMPER : Storage ERTC value "

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t temp = 0;
__IO uint32_t uwSecondfraction = 0;
__IO uint8_t  ubStartevent = 0;

ERTC_InitType ERTC_InitStructure;
ERTC_TimeType ERTC_TimeStruct; 

/* Private function prototypes -----------------------------------------------*/
static void ERTC_Config(void);
Table_TypeDef ERTC_Get_Time(uint32_t Second_fraction , ERTC_TimeType *ERTC_TimeStructure);
void ERTC_Time_display(Table_TypeDef table);
void EXTI_Port_Init(void);

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
  UART_Print_Init(921600);
  
  /* Display message */
  printf(MESSAGE1);
  printf("\r\n");
	
  /* ERTC configuration  */
  ERTC_Config();

	/*EXTI Port Init.*/
  EXTI_Port_Init();
	
  /* Display message */
  printf(MESSAGE2);
  printf("\r\n");
	printf(MESSAGE3);
  printf("\r\n");
	
  while (1)
  {
    /* Check on the event 'start' */
    if(ubStartevent != 1)
    {
      /* Get the ERTC sub seconds fraction */
      uwSecondfraction = 1000 - ((uint32_t)((uint32_t)ERTC_GetSubSecondValue() * 1000) / (uint32_t)0x3FF);
    }
    else
    {
      /* Idle */
      uwSecondfraction = 1;
    }
    
    /* Get the Current time */
    ERTC_GetTimeValue(ERTC_Format_BCD, &ERTC_TimeStruct);
    
		if( uwSecondfraction != temp)
		{
			temp = uwSecondfraction;
			/* Display the current time and the sub seconds on the LCD */
			ERTC_Time_display(ERTC_Get_Time(uwSecondfraction , &ERTC_TimeStruct));
		}
  }
}

/**
  * @brief  Configures the ERTC peripheral by selecting the clock source.
  * @param  None
  * @retval None
  */
static void ERTC_Config(void)
{
  ERTC_TimeType  ERTC_TimeStructure;
  
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
  ERTC_InitStructure.ERTC_AsynchPrediv = 0x1F;
  ERTC_InitStructure.ERTC_SynchPrediv  = 0x3FF;
  ERTC_InitStructure.ERTC_HourFormat   = ERTC_HourFormat_24;
  ERTC_Init(&ERTC_InitStructure);
  
  /* Set the time to 00h 00mn 00s AM */
  ERTC_TimeStructure.ERTC_AMPM    = ERTC_H12_AM;
  ERTC_TimeStructure.ERTC_Hours   = 0;
  ERTC_TimeStructure.ERTC_Minutes = 0;
  ERTC_TimeStructure.ERTC_Seconds = 0;  
  ERTC_SetTimeValue(ERTC_Format_BCD, &ERTC_TimeStructure);
}

/**
  * @brief  Configures the EXTI Port Interrupt Init.
  * @param  None
  * @retval None
  */
void EXTI_Port_Init(void)
{
	NVIC_InitType  NVIC_InitStructure;
  EXTI_InitType  EXTI_InitStructure;
	
	/* Enable the SYSCFG clock */    
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_SYSCFGCOMP, ENABLE);
  
	/* Enable the GPIO clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA | RCC_AHBPERIPH_GPIOC, ENABLE);
	
	/* Port source configuration*/  
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

  /* EXTI configuration *******************************************************/
  EXTI_ClearIntPendingBit(EXTI_Line13);
  EXTI_InitStructure.EXTI_Line = EXTI_Line13;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
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
  * @brief  Returns the current time and sub second.
  * @param  Secondfraction: the sub second fraction.
  * @param  ERTC_TimeStructure : pointer to a ERTC_TimeType structure that 
  *         contains the current time values. 
  * @retval table : return current time and sub second in a table form
  */
Table_TypeDef ERTC_Get_Time(uint32_t Secondfraction , ERTC_TimeType* ERTC_TimeStructure)
{
  Table_TypeDef table2;

  /* Fill the table fields with the current Time*/
  table2.tab[0]   = (((uint8_t)(ERTC_TimeStructure->ERTC_Hours & 0xF0) >> 0x04) + 0x30);
  table2.tab[1]   = (((uint8_t)(ERTC_TimeStructure->ERTC_Hours & 0x0F))+ 0x30);
  table2.tab[2]   = 0x3A;
  
  table2.tab[3]   = (((uint8_t)(ERTC_TimeStructure->ERTC_Minutes & 0xF0) >> 0x04) + 0x30);
  table2.tab[4]   =(((uint8_t)(ERTC_TimeStructure->ERTC_Minutes & 0x0F))+ (uint8_t)0x30);
  table2.tab[5]   = 0x3A;

  table2.tab[6]   = (((uint8_t)(ERTC_TimeStructure->ERTC_Seconds & 0xF0) >> 0x04)+ 0x30);
  table2.tab[7]   = (((uint8_t)(ERTC_TimeStructure->ERTC_Seconds & 0x0F)) + 0x30);
  table2.tab[8]   = 0x2E;
  
  table2.tab[9]   = (uint8_t)((Secondfraction / 100) + 0x30);
  table2.tab[10]  = (uint8_t)(((Secondfraction % 100 ) / 10) + 0x30);
  table2.tab[11]  =  (uint8_t)((Secondfraction % 10) + 0x30);
  
  /* return table2 */
  return table2;
}

/**
  * @brief  Displays the current Time on the LCD.
  * @param  Line:  the Line where to display the Current time .
  *           This parameter can be one of the following values:
  *             @arg Linex: where x can be 0..9
  * @param  Color_x: specifies the Background Color.
  * @param  table: the Current time and sub second.
  * @retval None
  */
void ERTC_Time_display(Table_TypeDef table )
{   
  uint8_t index = 0;
  
  for (index = 0; index < 12; index++)
  {
    /* Display char by printf */
    printf("%c",table.tab[index]);
  }  
   printf("\r\n");
}
/**
  * @}
  */ 

/**
  * @}
  */ 

