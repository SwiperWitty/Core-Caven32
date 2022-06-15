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

/** @addtogroup AT32F403_StdPeriph_Examples
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
extern __IO uint16_t CaptureNumber;
uint8_t EXTI_flag = 0;

/* Private function prototypes -----------------------------------------------*/
void KEY_Init(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{

  /* Initialize LED and KEY*/
  AT32_Board_Init();

  KEY_Init();

  /* Check if the system has resumed from IWDG reset */
  if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
  {
    /* IWDGRST flag set */
    /* Turn on LED4 */
    AT32_LEDn_ON(LED4);

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

  while (1)
  {
    /* Toggle LED3 */
    AT32_LEDn_Toggle(LED3);

    /* Insert 200ms delay */
    Delay_ms(200);

    /* Check if EXTI_flag is set */
    if(EXTI_flag)
    {
      while(1);
    }

    /* Reload IWDG counter */
    IWDG_ReloadCounter();
  }
}

/**
  * @brief  Initialize the KEY as EXTI Line0.
  * @param  None
  * @retval None
  */
void KEY_Init(void)
{
  EXTI_InitType   EXTI_InitStructure;
  GPIO_InitType   GPIO_InitStructure;
  NVIC_InitType   NVIC_InitStructure;

  /* Enable GPIOA clock */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);

  /* Configure PA.00 pin as input floating */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Enable AFIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO, ENABLE);

  /* Connect EXTI0 Line to PA.00 pin */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinsSource0);

  /* Configure EXTI0 line */
  EXTI_StructInit(&EXTI_InitStructure);
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineEnable = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI0 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
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

