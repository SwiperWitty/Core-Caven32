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

/** @addtogroup AT32F415_StdPeriph_Examples
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
__IO uint32_t   uwLsiFreq = 0;
__IO uint32_t   uwCaptureNumber = 0; 
__IO uint32_t   uwPeriodValue = 0;

/* Private function prototypes -----------------------------------------------*/
static void     ERTC_Config(void);
static uint32_t GetLSIFrequency(void);

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
  
  /* Get the LSI frequency:  TMR5 is used to measure the LSI frequency */
  uwLsiFreq = GetLSIFrequency();

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
  NVIC_InitType NVIC_InitStructure; 
  EXTI_InitType EXTI_InitStructure;

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
  EXTI_ClearIntPendingBit(EXTI_Line22);
  EXTI_InitStructure.EXTI_Line = EXTI_Line22;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineEnable = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* Enable the ERTC Wakeup Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = ERTC_WKUP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  

  /* Configure the ERTC WakeUp Clock source: CK_SPRE (1Hz) */
  ERTC_WakeUpClockConfig(ERTC_WakeUpClockSelect_CK_SPRE_16bits);
  ERTC_SetWakeUpCounter(0x0);

  /*Clears the ERTC's interrupt pending bits.*/
  ERTC_ClearINTPendingBINT(ERTC_INT_WAT);

  /* Enable the ERTC Wakeup Interrupt */
  ERTC_INTConfig(ERTC_INT_WAT, ENABLE);
  
  /* Enable Wakeup Counter */
  ERTC_WakeUpCmd(ENABLE);
}

/**
  * @brief  Configures TMR5 to measure the LSI oscillator frequency. 
  * @param  None
  * @retval LSI Frequency
  */
static uint32_t GetLSIFrequency(void)
{
  NVIC_InitType   NVIC_InitStructure;
  TMR_ICInitType  TMR_ICInitStructure;
  RCC_ClockType   RCC_ClockFreq;

  /* Enable the LSI oscillator ************************************************/
  RCC_LSICmd(ENABLE);
  
  /* Wait till LSI is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSISTBL) == RESET)
  {}

  /* TMR5 configuration *******************************************************/ 
  /* Enable TMR5 AFIO clock */
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR5, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO, ENABLE);
    
  /* Connect internally the TMR5_CH4 Input Capture to the LSI clock output */
  GPIO_PinsRemapConfig(AFIO_MAP4_TMR5_1000, ENABLE);

  /* Configure TMR5 presclaer */
  TMR_DIVConfig(TMR5, 0, TMR_DIVReloadMode_Immediate);
  
  /* TMR5 configuration: Input Capture mode ---------------------
     The LSI oscillator is connected to TMR5 CH4
     The Rising edge is used as active edge,
     The TMR5 CCR4 is used to compute the frequency value 
  ------------------------------------------------------------ */
  TMR_ICInitStructure.TMR_Channel = TMR_Channel_4;
  TMR_ICInitStructure.TMR_ICPolarity = TMR_ICPolarity_Rising;
  TMR_ICInitStructure.TMR_ICSelection = TMR_ICSelection_DirectTI;
  TMR_ICInitStructure.TMR_ICDIV  = TMR_ICDIV_DIV8;
  TMR_ICInitStructure.TMR_ICFilter = 0;
  TMR_ICInit(TMR5, &TMR_ICInitStructure);
  
  /* Enable TMR5 Interrupt channel */
  NVIC_InitStructure.NVIC_IRQChannel = TMR5_GLOBAL_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable TMR5 counter */
  TMR_Cmd(TMR5, ENABLE);

  /* Reset the flags */
  TMR5->STS  = 0;
    
  /* Enable the CC4 Interrupt Request */  
  TMR_INTConfig(TMR5, TMR_INT_CC4, ENABLE);

  /* Wait until the TMR5 get 2 LSI edges (refer to TMR5_IRQHandler() in 
    at32f4xx_it.c file) ******************************************************/
  while(uwCaptureNumber != 2)
  {
  }
  /* Deinitialize the TMR5 peripheral registers to their default reset values */
  TMR_Reset(TMR5);

  /* Compute the LSI frequency, depending on TMR5 input clock frequency (PCLK1)*/
  /* Get SYSCLK, HCLK and PCLKx frequency */
  RCC_GetClocksFreq(&RCC_ClockFreq);

  /* Get PCLK1 prescaler */
  if ((RCC->CFG  & RCC_CFG_APB1PSC) == 0)
  { 
    /* PCLK1 prescaler equal to 1 => TIMCLK = PCLK1 */
    return ((RCC_ClockFreq.APB1CLK_Freq / uwPeriodValue) * 8);
  }
  else
  { /* PCLK1 prescaler different from 1 => TIMCLK = 2 * PCLK1 */
    return (((2 * RCC_ClockFreq.APB1CLK_Freq) / uwPeriodValue) * 8) ;
  }
}

/**
  * @}
  */ 

/**
  * @}
  */ 

