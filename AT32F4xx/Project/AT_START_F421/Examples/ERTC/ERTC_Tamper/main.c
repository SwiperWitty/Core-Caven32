/**
  ******************************************************************************
  * File   : ERTC/ERTC_Tamper/main.c 
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

/** @addtogroup ERTC_Tamper
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ERTC_BKP_DT_NUMBER              5  /* ERTC Backup Data Register Number */

/* Uncomment the corresponding line to select the ERTC Clock source */
//#define ERTC_CLOCK_SOURCE_LSE            /* LSE used as ERTC source clock */
#define ERTC_CLOCK_SOURCE_LSI              /* LSI used as ERTC source clock. The ERTC Clock
                                           may varies due to LSI frequency dispersion. */ 

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t aERTC_BKP_DT[ERTC_BKP_DT_NUMBER] =
  {
    ERTC_BKP_DT0, ERTC_BKP_DT1, ERTC_BKP_DT2, ERTC_BKP_DT3, ERTC_BKP_DT4
  };

/* Private functions prototypes ----------------------------------------------*/
static void  ERTC_Config(void);
static void  WriteToERTC_BKP_DT(uint32_t FirstERTCBackupData);
static uint32_t CheckERTC_BKP_DT(uint32_t FirstERTCBackupData);

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
    
  /* ERTC configuration */
  ERTC_Config();

  /* Write To ERTC Backup Data registers */
  WriteToERTC_BKP_DT(0xA53C);

  /* Check if the written data are correct */
  if(CheckERTC_BKP_DT(0xA53C) == 0)
  {
    /* Turn on LED2 */
    AT32_LEDn_ON(LED2);
  }
  else
  {
    /* Turn off LED2 */
    AT32_LEDn_OFF(LED2);
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
void ERTC_Config(void)
{
  NVIC_InitType NVIC_InitStructure;
  EXTI_InitType  EXTI_InitStructure;

  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_PWR, ENABLE);

  /* Allow access to ERTC */
  PWR_BackupAccessCtrl(ENABLE);

  /* Reset BKP Domain */
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
  
#elif defined (ERTC_CLOCK_SOURCE_LSE) /* LSE used as ERTC source clock */
  /* Enable the LSE OSC */
  RCC_LSEConfig(RCC_LSE_ENABLE);

  /* Wait till LSE is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSESTBL) == RESET)
  {
  }

  /* Select the ERTC Clock Source */
  RCC_ERTCCLKConfig(RCC_ERTCCLKSelection_LSE);
  
#endif /* ERTC_CLOCK_SOURCE_LSI */
  
  /* Enable the ERTC Clock */
  RCC_ERTCCLKCmd(ENABLE);

  /* Deinitializes the ERTC registers */  
  ERTC_Reset();  
  
  /* Wait for ERTC APB registers synchronisation */
  ERTC_WaitForSynchro();
  
  /* Enable The external line21 interrupt */
  EXTI_ClearIntPendingBit(EXTI_Line19);
  EXTI_InitStructure.EXTI_Line = EXTI_Line19;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineEnable = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable TAMPER IRQChannel */
  NVIC_InitStructure.NVIC_IRQChannel =  ERTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Disable the Tamper 1 detection */
  ERTC_TamperCmd(ERTC_TAMP_1, DISABLE);

  /* Clear Tamper 1 pin Event(TAMP1F) pending flag */
  ERTC_ClearFlag(ERTC_FLAG_TP1F);

  /* Configure the Tamper 1 Trigger */
  ERTC_TamperTriggerConfig(ERTC_TAMP_1, ERTC_TamperTrig_FallingEdge);

  /* Enable the Tamper interrupt */
  ERTC_INTConfig(ERTC_INT_TAMP, ENABLE);

  /* Clear Tamper 1 pin interrupt pending bit */
  ERTC_ClearINTPendingBINT(ERTC_INT_TAMP1);

  /* Enable the Tamper 1 detection */
  ERTC_TamperCmd(ERTC_TAMP_1, ENABLE);
}

/**
  * @brief  Writes data ERTC Backup DTx registers.
  * @param  FirstRTCBackupData: data to be written to ERTC Backup data registers.
  * @retval None
  */
static void WriteToERTC_BKP_DT(uint32_t FirstERTCBackupData)
{
  uint32_t index = 0;

  for (index = 0; index < ERTC_BKP_DT_NUMBER; index++)
  {
    /* write To bkp data register */
    ERTC_WriteBackupRegister(aERTC_BKP_DT[index], FirstERTCBackupData + (index * 0x5A));
  }
}

/**
  * @brief  Checks if the ERTC Backup DTx registers values are correct or not.
  * @param  FirstRTCBackupData: data to be compared with ERTC Backup data registers.
  * @retval - 0: All ERTC Backup DTx registers values are correct
  *         - Value different from 0: Number of the first Backup register
  *           which value is not correct
  */
static uint32_t CheckERTC_BKP_DT(uint32_t FirstERTCBackupData)
{
  uint32_t index = 0;

  for (index = 0; index < ERTC_BKP_DT_NUMBER; index++)
  {
    /* Read from data register */
    if (ERTC_ReadBackupRegister(aERTC_BKP_DT[index]) != (FirstERTCBackupData + (index * 0x5A)))
    {
      return (index + 1);
    }
  }
    return 0;
}

/**
  * @brief  Checks if the ERTC Backup DTx registers are reset or not.
  * @param  None
  * @retval - 0: All ERTC Backup DTx registers are reset
  *         - Value different from 0: Number of the first Backup register
  *           not reset
  */
uint32_t IsBackupRegReset(void)
{
  uint32_t index = 0;

  for (index = 0; index < ERTC_BKP_DT_NUMBER; index++)
  {
     /* Read from bkp Data Register */
    if (ERTC_ReadBackupRegister(aERTC_BKP_DT[index]) != 0)
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

