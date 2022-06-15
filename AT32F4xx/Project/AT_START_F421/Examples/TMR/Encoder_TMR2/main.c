 /**
  ******************************************************************************
  * File   : TMR/Encoder_TMR2/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 


#include "at32f4xx.h"

/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */

/** @addtogroup TMR_Encoder_TMR2
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TMR_TimerBaseInitType  TMR_TimerBaseInitStructure = {0};
uint32_t counter;

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void Delay(uint32_t time);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured,
       this is done through SystemInit() function which is called from startup
       file (startup_at32f415_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_at32f4xx.c file
     */

  /* System Clocks Configuration */
  RCC_Configuration();

  /* Configure the GPIO ports */
  GPIO_Configuration();

  /* TMR2 encoder mode configuration--------------------------
  TMR2 TI1PF1 ,TI2FP2 as encoder input pin, TMR2 counter
  changed each signal edge.-----------------------------------
  ------------------------------------------------------------ */
  /* Timer 2 pluse mode configuration */
  TMR_TimeBaseStructInit(&TMR_TimerBaseInitStructure);
  TMR_TimerBaseInitStructure.TMR_Period = 0xFFFFFFFF;
  TMR_TimerBaseInitStructure.TMR_CounterMode = TMR_CounterDIR_Up;
  TMR_TimerBaseInitStructure.TMR_ClockDivision = TMR_CKD_DIV1;
  TMR_TimerBaseInitStructure.TMR_DIV = 0;
  TMR_TimeBaseInit(TMR3, &TMR_TimerBaseInitStructure);

  /* Timer 2 Encoder mode configuration */
  TMR_EncoderInterfaceConfig(TMR3, TMR_EncoderMode_TI12, TMR_ICPolarity_Rising, TMR_ICPolarity_Rising);

  /* TMR enable counter */
  TMR_Cmd(TMR3, ENABLE);

  while (1)
  {
    /* Generate encoder signal */
    GPIO_SetBits(GPIOA, GPIO_Pins_2);
    Delay(150);
    GPIO_SetBits(GPIOA, GPIO_Pins_3);
    Delay(150);
    GPIO_ResetBits(GPIOA, GPIO_Pins_2);
    Delay(150);
    GPIO_ResetBits(GPIOA, GPIO_Pins_3);
    Delay(150);

    /* Get current counter value */
    counter = TMR_GetCounter(TMR3);
  }
}

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{
  /* TMR3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR3, ENABLE);

  /* GPIOA clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA, ENABLE);
}

/**
  * @brief  Configure the GPIO Pins.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure = {0};

  
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_2 | GPIO_Pins_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* TMR3 channel 1/2 pin (PA.06/PA.07) configuration */
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource6, GPIO_AF_1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource7, GPIO_AF_1);
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_6 | GPIO_Pins_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
  * @brief  Delay function
  * @param  time:Number of time to delay
  * @retval None
  */
void Delay(uint32_t time)
{
  uint32_t i;

  for(i = 0; i < time; i++);
}

#ifdef  USE_FULL_ASSERT

/**.\Objects\Encoder_TMR2.axf: Error: L6218E: Undefined symbol TMR_SelectPlusMode (referred from main.o).
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

  
