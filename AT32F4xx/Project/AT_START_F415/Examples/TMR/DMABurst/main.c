 /**
  ******************************************************************************
  * File   : TMR/TMR_DMABurst/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */
 
/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"

/** @addtogroup AT32F415_StdPeriph_Examples
  * @{
  */

/** @addtogroup TMR_DMABurst
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TIM1_DMAR_ADDRESS ((uint32_t)0x40012C4C) /* TMR ARR (Auto Reload Register) address */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
GPIO_InitType         GPIO_InitStructure = {0};
DMA_InitType          DMA_InitStructure = {0};
TMR_TimerBaseInitType  TMR_TimeBaseStructure = {0};
TMR_OCInitType        TMR_OCInitStructure = {0};
uint16_t SRC_Buffer[6] = {0x0FFF, 0x0000, 0x0555};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* TMR1 and GPIOA clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR1 | RCC_APB2PERIPH_GPIOA, ENABLE);

  /* DMA clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA1, ENABLE);
  
  /* GPIOA Configuration: Channel 1 as alternate function push-pull */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins =  GPIO_Pins_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  

  /* TMR1 DeInit */
  TMR_Reset(TMR1);

  /* DMA1 Channel5 Config */
  DMA_Reset(DMA1_Channel5);
  DMA_DefaultInitParaConfig(&DMA_InitStructure);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)TIM1_DMAR_ADDRESS; 
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)SRC_Buffer; 
  DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALDST;
  DMA_InitStructure.DMA_BufferSize = 3;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;
  DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE;
  DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_HALFWORD;
  DMA_InitStructure.DMA_MemoryDataWidth = DMA_MEMORYDATAWIDTH_HALFWORD;
  DMA_InitStructure.DMA_Mode = DMA_MODE_NORMAL;
  DMA_InitStructure.DMA_Priority = DMA_PRIORITY_HIGH;
  DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;
  DMA_Init(DMA1_Channel5, &DMA_InitStructure);
  
  /* Time base configuration */
  /* -----------------------------------------------------------------------
    TMR1 Configuration: generate 1 PWM signal using the DMA burst mode:
    The TMR1CLK frequency is set to SystemCoreClock (Hz), to get TMR1 counter
    clock at 24 MHz the Prescaler is computed as following:
     - Prescaler = (TIM1CLK / TMR1 counter clock) - 1
    SystemCoreClock is set to 144 MHz .
    The TMR1 period is 5.86 KHz: TMR1 Frequency = TMR1 counter clock/(ARR + 1)
                                               = 24 MHz / 4096 = 5.86KHz KHz
    TMR1 Channel1 duty cycle = (TIM1_CCR1/ TMR1_ARR)* 100 = 33.33%
  ----------------------------------------------------------------------- */  
  TMR_TimeBaseStructInit(&TMR_TimeBaseStructure);
  TMR_TimeBaseStructure.TMR_Period = 0xFFFF;          
  TMR_TimeBaseStructure.TMR_DIV = (uint16_t) (SystemCoreClock / 24000000) - 1;       
  TMR_TimeBaseStructure.TMR_ClockDivision = 0x0;    
  TMR_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;   
  TMR_TimeBaseInit(TMR1, &TMR_TimeBaseStructure);

  /* TMR Configuration in PWM Mode */
  TMR_OCStructInit(&TMR_OCInitStructure);
  TMR_OCInitStructure.TMR_OCMode =  TMR_OCMode_PWM1;    
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;          
  TMR_OCInitStructure.TMR_Pulse = 0xFFF;  
  TMR_OC1Init(TMR1, &TMR_OCInitStructure); 

  /* TMR1 DMAR Base register and DMA Burst Length Config */
  TMR_DMAConfig(TMR1, TMR_DMABase_AR, TMR_DMABurstLength_3Transfers);

  /* TMR1 DMA Update enable */
  TMR_DMACmd(TMR1, TMR_DMA_Update, ENABLE);

  /* TMR1 enable */
  TMR_Cmd(TMR1, ENABLE);
  
  /* TMR1 PWM Outputs Enable */
  TMR_CtrlPWMOutputs(TMR1, ENABLE);

  /* DMA1 Channel5 enable */
  DMA_ChannelEnable(DMA1_Channel5, ENABLE);

  /* Wait until DMA1 Channel5 end of Transfer */
  while (!DMA_GetFlagStatus(DMA1_FLAG_TC5))
  {
  }

  /* Infinite loop */ 
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
