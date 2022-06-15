 /**
  ******************************************************************************
  * File   : TMR/TMR_DMA/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 
 
/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"

/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */

/** @addtogroup TMR_DMA
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TIM1_CCR3_Address    0x40012C3C

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TMR_TimerBaseInitType  TMR_TimeBaseStructure = {0};
TMR_OCInitType  TMR_OCInitStructure = {0};
uint16_t SRC_Buffer[3] = {0, 0, 0};
uint16_t TimerPeriod = 0;

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void DMA_Configuration(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* System Clocks Configuration */
  RCC_Configuration();

  /* GPIO Configuration */
  GPIO_Configuration();

  /* DMA Configuration */
  DMA_Configuration();

  /* TMR1 DMA Transfer example -------------------------------------------------
  TMR1CLK = SystemCoreClock, Prescaler = 0, TMR1 counter clock = SystemCoreClock
  SystemCoreClock is set to 120 MHz. 
  
  The objective is to configure TMR1 channel 3 to generate complementary PWM
  signal with a frequency equal to 17.57 KHz:
     - TMR1_Period = (SystemCoreClock / 17570) - 1
  and a variable duty cycle that is changed by the DMA after a specific number of
  Update DMA request.

  The number of this repetitive requests is defined by the TMR1 Repetition counter,
  each 3 Update Requests, the TMR1 Channel 3 Duty Cycle changes to the next new 
  value defined by the SRC_Buffer . 
  -----------------------------------------------------------------------------*/
  /* Compute the value to be set in ARR register to generate signal frequency at 17.57 Khz */
  TimerPeriod = (SystemCoreClock / 17570 ) - 1;
  /* Compute CCR1 value to generate a duty cycle at 50% */
  SRC_Buffer[0] = (uint16_t) (((uint32_t) 5 * (TimerPeriod - 1)) / 10);
  /* Compute CCR1 value to generate a duty cycle at 37.5% */
  SRC_Buffer[1] = (uint16_t) (((uint32_t) 375 * (TimerPeriod - 1)) / 1000);
  /* Compute CCR1 value to generate a duty cycle at 25% */
  SRC_Buffer[2] = (uint16_t) (((uint32_t) 25 * (TimerPeriod - 1)) / 100);

  /* TMR1 Peripheral Configuration --------------------------------------------*/
  /* Time Base configuration */
  TMR_TimeBaseStructInit(&TMR_TimeBaseStructure);
  TMR_TimeBaseStructure.TMR_DIV = 0;
  TMR_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;
  TMR_TimeBaseStructure.TMR_Period = TimerPeriod;
  TMR_TimeBaseStructure.TMR_ClockDivision = 0;
  TMR_TimeBaseStructure.TMR_RepetitionCounter = 2;

  TMR_TimeBaseInit(TMR1, &TMR_TimeBaseStructure);

  /* Channel 3 Configuration in PWM mode */
  TMR_OCStructInit(&TMR_OCInitStructure);
  TMR_OCInitStructure.TMR_OCMode = TMR_OCMode_PWM2;
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_OutputNState = TMR_OutputNState_Enable;
  TMR_OCInitStructure.TMR_Pulse = SRC_Buffer[0];
  TMR_OCInitStructure.TMR_OCPolarity = TMR_OCPolarity_Low;
  TMR_OCInitStructure.TMR_OCNPolarity = TMR_OCNPolarity_Low;
  TMR_OCInitStructure.TMR_OCIdleState = TMR_OCIdleState_Set;
  TMR_OCInitStructure.TMR_OCNIdleState = TMR_OCIdleState_Reset;

  TMR_OC3Init(TMR1, &TMR_OCInitStructure);

  /* TMR1 Update DMA Request enable */
  TMR_DMACmd(TMR1, TMR_DMA_Update, ENABLE);

  /* TMR1 counter enable */
  TMR_Cmd(TMR1, ENABLE);

  /* Main Output Enable */
  TMR_CtrlPWMOutputs(TMR1, ENABLE);

  while (1)
  {}
}

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{
  /* TMR1, GPIOA and GPIOB clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR1, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA | RCC_AHBPERIPH_GPIOB, ENABLE);
  /* DMA clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA1, ENABLE);
}

/**
  * @brief  Configure the TMR1 Pins.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure = {0};

  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource10, GPIO_AF_2);
  GPIO_PinAFConfig(GPIOB, GPIO_PinsSource15, GPIO_AF_2);
  
  /* GPIOA Configuration: Channel 3 as alternate function push-pull */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins =  GPIO_Pins_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* GPIOB Configuration: Channel 3N as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_15;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
  * @brief  Configures the DMA.
  * @param  None
  * @retval None
  */
void DMA_Configuration(void)
{
  DMA_InitType DMA_InitStructure = {0};

  /* DMA1 Channel5 Config */
  DMA_Reset(DMA1_Channel5);
  DMA_DefaultInitParaConfig(&DMA_InitStructure);
  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)TIM1_CCR3_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)SRC_Buffer;
  DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALDST;
  DMA_InitStructure.DMA_BufferSize = 3;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;
  DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE;
  DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_HALFWORD;
  DMA_InitStructure.DMA_MemoryDataWidth = DMA_MEMORYDATAWIDTH_HALFWORD;
  DMA_InitStructure.DMA_Mode = DMA_MODE_CIRCULAR;
  DMA_InitStructure.DMA_Priority = DMA_PRIORITY_HIGH;
  DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;

  DMA_Init(DMA1_Channel5, &DMA_InitStructure);

  /* DMA1 Channel5 enable */
  DMA_ChannelEnable(DMA1_Channel5, ENABLE);
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
  
