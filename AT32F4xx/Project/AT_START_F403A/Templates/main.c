/**
  ******************************************************************************
  * File   : Templates/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "at32f4xx.h"
#include "at32_board.h"
/** @addtogroup AT32F403A_StdPeriph_Templates
  * @{
  */

/** @addtogroup Template
  * @{
  */

/* Private define ------------------------------------------------------------*/
#define DELAY          100
#define FAST           1
#define SLOW           4

/* Extern variables ---------------------------------------------------------*/
extern GPIO_Type *BUTTON_GPIO_PORT[BUTTON_NUM];
extern uint16_t BUTTON_GPIO_PIN[BUTTON_NUM];
extern uint32_t BUTTON_GPIO_RCC_CLK [BUTTON_NUM];

/* Private variables ---------------------------------------------------------*/
uint16_t BUTTON_EXTI_LINE[BUTTON_NUM] = {EXTI_Line0, EXTI_Line13};
uint16_t BUTTON_EXTI_IRQ[BUTTON_NUM] = {EXTI0_IRQn, EXTI15_10_IRQn};
uint8_t BUTTON_EXTI_SOURCE_PORT[BUTTON_NUM] = {GPIO_PortSourceGPIOA, GPIO_PortSourceGPIOC};
uint8_t BUTTON_EXTI_SOURCE_PIN[BUTTON_NUM] = {GPIO_PinsSource0, GPIO_PinsSource13};

BUTTON_Type gButtonType = BUTTON_WAKEUP;
uint8_t gSpeed = FAST;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Config Clock Out Function.
  * @param  None
  * @retval None
  */
void MCO_Config(void)
{
  GPIO_InitType GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);

  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_8;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /*PA8Êä³öPLL/4*/
  RCC_CLKOUTConfig(RCC_CLKOUT_PLL_Div4, RCC_MCOPRE_1);
}

/**
  * @brief  Configure Button EXTI   
  * @param  Button: Specifies the Button to be configured.
  * @retval None
  */
void BUTTON_EXTI_Init(BUTTON_Type button)
{
  EXTI_InitType EXTI_InitStructure;
  NVIC_InitType NVIC_InitStructure;

  GPIO_EXTILineConfig(BUTTON_EXTI_SOURCE_PORT[button], BUTTON_EXTI_SOURCE_PIN[button]);
  EXTI_StructInit(&EXTI_InitStructure);
  EXTI_InitStructure.EXTI_Line = BUTTON_EXTI_LINE[button];
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_LineEnable = ENABLE;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = BUTTON_EXTI_IRQ[button];
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Button EXTI0 Interrupt Handler
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void)
{
  Delay_ms(5);
  EXTI_ClearIntPendingBit(BUTTON_EXTI_LINE[gButtonType]);
  if ((GPIO_ReadInputData(BUTTON_GPIO_PORT[gButtonType]) & BUTTON_GPIO_PIN[gButtonType]) == \
      BUTTON_GPIO_PIN[gButtonType])
  {
    if(gSpeed == SLOW)
      gSpeed = FAST;
    else
      gSpeed = SLOW;
  }
}

/**
  * @brief  Button EXTI13 Interrupt Handler
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
  Delay_ms(5);
  EXTI_ClearIntPendingBit(BUTTON_EXTI_LINE[gButtonType]);
  if ((GPIO_ReadInputData(BUTTON_GPIO_PORT[gButtonType]) & BUTTON_GPIO_PIN[gButtonType]) == \
      BUTTON_GPIO_PIN[gButtonType])
  {
    if(gSpeed == SLOW)
      gSpeed = FAST;
    else
      gSpeed = SLOW;
  }
}

/**
  * @brief  Main Function.
  * @param  None
  * @retval None
  */
int main(void)
{
  gButtonType = BUTTON_WAKEUP;

  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO, ENABLE);

  AT32_Board_Init();

  MCO_Config();

  BUTTON_EXTI_Init(gButtonType);

  for(;;)
  {
    AT32_LEDn_Toggle(LED2);
    Delay_ms(gSpeed * DELAY);
    AT32_LEDn_Toggle(LED3);
    Delay_ms(gSpeed * DELAY);
    AT32_LEDn_Toggle(LED4);
    Delay_ms(gSpeed * DELAY);
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



