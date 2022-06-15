/**
  **************************************************************************
  * File   : PWR/STANDBY/main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  **************************************************************************
  */
 
 /* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "at32_board.h"

/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */

/** @addtogroup PWR_STANDBY
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

GPIO_InitType GPIO_InitStructure;
EXTI_InitType EXTI_InitStructure;
NVIC_InitType NVIC_InitStructure = {0};
RCC_ClockType RccClkSource = {0};
ERTC_InitType  ERTC_InitStructure;
ERTC_TimeType  ERTC_TimeStruct;

/* Private function prototypes -----------------------------------------------*/

/**
  * @brief  Configure the ERTC peripheral by selecting the clock source.
  * @param  None
  * @retval None
  */
static void ERTC_Config(void)
{
    /* Allow access to ERTC */
    PWR_BackupAccessCtrl(ENABLE);
    if(ERTC_GetFlagStatus(ERTC_FLAG_ALAF)!=RESET)
    {
        AT32_LEDn_ON(LED3);
        ERTC_ClearFlag(ERTC_FLAG_ALAF);
    }

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
  ERTC_AlarmType ERTC_AlarmStructure;
  
  /* Set the alarm A Masks */
  ERTC_AlarmStructure.ERTC_AlarmMask = ERTC_AlarmMask_None;
  ERTC_AlarmStructure.ERTC_AlarmDateWeekSel = ERTC_AlarmDateWeekSel_Date;
  ERTC_AlarmStructure.ERTC_AlarmDateWeek = ERTC_Week_MON;
  ERTC_AlarmStructure.ERTC_AlarmTime.ERTC_Hours = 0x0;
  ERTC_AlarmStructure.ERTC_AlarmTime.ERTC_Minutes =0x0;
  ERTC_AlarmStructure.ERTC_AlarmTime.ERTC_Seconds =0x5;
  ERTC_SetAlarmValue(ERTC_Format_BCD, ERTC_AlA, &ERTC_AlarmStructure);

  /* Enable alarm A interrupt */
  ERTC_INTConfig(ERTC_INT_ALA, ENABLE);
	
	/* Enable the alarmA */
  ERTC_AlarmCmd(ERTC_AlA, ENABLE);
}

/**
  * @brief  Configures All GPIOs to Anaog Mode.
  * @param  None
  * @retval None
  */
void GPIO_Config_ALL_AIN(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA | RCC_AHBPERIPH_GPIOB
					 | RCC_AHBPERIPH_GPIOC| RCC_AHBPERIPH_GPIOF, ENABLE);

	GPIO_InitStructure.GPIO_Pins = GPIO_Pins_All;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Pull = GPIO_Pull_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pins = GPIO_Pins_All;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pins = GPIO_Pins_All;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pins = GPIO_Pins_All;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
}

/**
  * @brief  Main Function.
  * @param  None
  * @retval None
  */
int main(void)
{
  GPIO_StructInit(&GPIO_InitStructure);
  EXTI_StructInit(&EXTI_InitStructure);
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_PWR, ENABLE);
  AT32_Board_Init();
  AT32_LEDn_OFF(LED2);
  AT32_LEDn_OFF(LED3);
  AT32_LEDn_OFF(LED4);
  Delay_sec(3);
  AT32_LEDn_ON(LED2);
  AT32_LEDn_ON(LED3);
  AT32_LEDn_ON(LED4);
  Delay_sec(5);
  ERTC_Config();
  Delay_sec(3);
  ERTC_AlarmConfig();
  GPIO_Config_ALL_AIN();     ///<All gpio config into analog mode
  PWR_EnterSTANDBYMode();    ///<Enter standby mode
  while(1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif


/**
  * @}
  */ 

/**
  * @}
  */ 

