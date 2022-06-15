 /**
  ******************************************************************************
  * File   : TMR/Hall_Xor_TMR2/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 


#include "at32f4xx.h"

/** @addtogroup AT32F415_StdPeriph_Examples
  * @{
  */

/** @addtogroup TMR_Hall_Xor_TMR2
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr))
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))

#define GPIOA_ODR_Addr    (GPIOA_BASE+12)

#define PortA_Pin8  BIT_ADDR(GPIOA_ODR_Addr ,8)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TMR_TimerBaseInitType  TMR_TimerBaseInitStructure = {0};
TMR_ICInitType  TIM_ICInitStructure = {0};

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
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

  /* Interrupt Configuration */
  NVIC_Configuration();

  /* Configure the GPIO ports */
  GPIO_Configuration();

  /* TMR2 XOR mode configuration--------------------------------------------
  TMR2 channel1 ,channel2 and channel3 as HALL signal input channel, Trigger
  generated each signal edge.-----------------------------------------------
  ------------------------------------------------------------ */
  /* Timer 2 pluse mode configuration */
  TMR_SelectPlusMode(TMR2, TMR_Plus_Mode_Enable);
  TMR_TimeBaseStructInit(&TMR_TimerBaseInitStructure);
  TMR_TimerBaseInitStructure.TMR_Period = 0xFFFFFFFF;
  TMR_TimerBaseInitStructure.TMR_ClockDivision = TMR_CKD_DIV1;
  TMR_TimerBaseInitStructure.TMR_CounterMode = TMR_CounterDIR_Up;
  TMR_TimerBaseInitStructure.TMR_DIV = 0;
  TMR_TimeBaseInit(TMR2, &TMR_TimerBaseInitStructure);

  /* Config TI1 TRC as input source */
  TMR_ICStructInit(&TIM_ICInitStructure);
  TIM_ICInitStructure.TMR_Channel = TMR_Channel_1;
  TIM_ICInitStructure.TMR_ICSelection = TMR_ICSelection_TRC;
  TMR_ICInit(TMR2, &TIM_ICInitStructure);

  /* XOR funtion enable */
  TMR_SelectHallSensor(TMR2, ENABLE);

  /* Select XOR signal as trigger */
  TMR_SelectInputTrigger(TMR2, TMR_TRGSEL_TI1F_ED);

  /* TMR2 counter reset each trigger */
  TMR_SelectSlaveMode(TMR2, TMR_SlaveMode_Reset);

  /* Trigger interrupt enable */
  TMR_INTConfig(TMR2, TMR_INT_Trigger , ENABLE);

  /* TMR enable counter */
  TMR_Cmd(TMR2, ENABLE);

  while (1)
  {
    /* Generate HALL signal */
    GPIO_SetBits(GPIOA, GPIO_Pins_3);
    Delay(1000);
    GPIO_ResetBits(GPIOA, GPIO_Pins_6);
    Delay(1000);
    GPIO_SetBits(GPIOA, GPIO_Pins_7);
    Delay(1000);
    GPIO_ResetBits(GPIOA, GPIO_Pins_3);
    Delay(1000);
    GPIO_SetBits(GPIOA, GPIO_Pins_6);
    Delay(1000);
    GPIO_ResetBits(GPIOA, GPIO_Pins_7);
    Delay(1000);
  }
}

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{
  /* TMR2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR2, ENABLE);

  /* GPIOA clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);
}

/**
  * @brief  Configure the GPIO Pins.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure = {0};

  /* TMR2 channel1 ,channel2 and channel3 configuration */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_0 | GPIO_Pins_1 | GPIO_Pins_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* HALL signal (PA3 ,PA6 ,PA7) and trigger flag signal(PA8) configuration */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_3 | GPIO_Pins_6 | GPIO_Pins_7 | GPIO_Pins_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
  * @brief  Configure the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
  NVIC_InitType NVIC_InitStructure;

  /* Enable the TMR2 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TMR2_GLOBAL_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
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

/**
  * @brief  This function handles TMR2 global interrupt request.
  * @param  None
  * @retval None
  */
void TMR2_GLOBAL_IRQHandler(void)
{
  if(TMR_GetFlagStatus(TMR2 , TMR_FLAG_Trigger) != RESET)
  {
    PortA_Pin8 = !PortA_Pin8;
    TMR2->STS &= 0;
  }

  TMR2->STS &= 0;
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

