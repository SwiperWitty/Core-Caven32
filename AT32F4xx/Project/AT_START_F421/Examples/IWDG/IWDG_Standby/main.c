/**
  ******************************************************************************
  * File   : IWDG/IWDG_Reset/main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */

#include "at32f4xx.h"
#include "at32_board.h"
#include "stdio.h"

/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */
  
/** @addtogroup IWDG_Reset
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t LsiFreq = 40000;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{

  /* Initialize LED KEY and Uart*/
  AT32_Board_Init();
  
  UART_Print_Init(115200);

  /* Enable PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_PWR, ENABLE);
  
  /* Check if the system has resumed from IWDG reset */
  if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
  {
    /* IWDGRST flag set */
    /* Turn on LED4 */
    AT32_LEDn_ON(LED4);
    
    printf("Wakeup From Standby By IWDG..\r\n");

    /* Clear reset flags */
    RCC_ClearFlag();
  }
  else
  {
    /* IWDGRST flag is not set */
    /* Turn off LED4 */
    AT32_LEDn_OFF(LED4);
  }

  /* IWDG timeout equal to 250 ms (the TMReout may varies due to LSI frequency dispersion) */
  /* Enable write access to IWDG_PSC and IWDG_RLD registers */
  IWDG_KeyRegWrite(IWDG_KeyRegWrite_Enable);

  /* IWDG counter clock: LSI/32 */
  IWDG_SetPrescaler(IWDG_Psc_32);

  /* Set counter reload value to obtain 250ms IWDG TMReOut.
     Counter Reload Value = 250ms/IWDG counter clock period
                          = 250ms / (32/LSI)
                          = 0.25s / (32/LsiFreq)
                          = LsiFreq/(32 * 4)
                          = LsiFreq/128
  */
  IWDG_SetReload(LsiFreq / 128);

  /* Reload IWDG counter */
  IWDG_ReloadCounter();

  /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
  IWDG_Enable();

  /* Delay 100us wait for IWDG enable successfully */
  Delay_us(100);
  
  /* Enter Standby mode */
  PWR_EnterSTANDBYMode();
  
  while (1)
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

