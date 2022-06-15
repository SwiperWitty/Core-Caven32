 /**
  ******************************************************************************
  * File   : TMR/PWM_Input_DMA/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 


#include "at32f4xx.h"
#include "stdio.h"
#include "at32_board.h"

/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */

/** @addtogroup TMR1_PWM_Input_DMA
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TMR_TimerBaseInitType  TMR_TMReBaseStructure;
TMR_OCInitType  TMR_OCInitStructure;
TMR_BRKDTInitType TMR_BDTRInitStructure;
TMR_ICInitType  TMR_ICInitStructure;
NVIC_InitType NVIC_InitStructure;

__IO uint16_t buffer_cc1[1000];
__IO uint16_t buffer_cc2[1000];

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
/* Private functions ---------------------------------------------------------*/
DMA_InitType                DMA_InitStructure;
#define TMR1_CCR1_Address   0x40012C34
#define TMR1_CCR2_Address  0x40012C38
uint16_t  Read_VUALE;
__IO uint16_t IC1Value = 0;
__IO uint16_t DutyCycle = 0;
__IO uint32_t Frequency = 0;
/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
int main(void)
{

  /* System Clocks Configuration */
  RCC_Configuration();

  /* GPIO Configuration */
  GPIO_Configuration();
  UART_Print_Init(115200);
  
  /* DMA1 Channel2 configuration ----------------------------------------------*/
  DMA_Reset(DMA1_Channel2);
  DMA_DefaultInitParaConfig(&DMA_InitStructure);
  DMA_InitStructure.DMA_PeripheralBaseAddr  = (uint32_t)TMR1_CCR1_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr      = (uint32_t)buffer_cc1;
  DMA_InitStructure.DMA_Direction           = DMA_DIR_PERIPHERALSRC;
  DMA_InitStructure.DMA_BufferSize          = 1000;
  DMA_InitStructure.DMA_PeripheralInc       = DMA_PERIPHERALINC_DISABLE;
  DMA_InitStructure.DMA_MemoryInc           = DMA_MEMORYINC_ENABLE;
  DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_HALFWORD;
  DMA_InitStructure.DMA_MemoryDataWidth     = DMA_MEMORYDATAWIDTH_HALFWORD;
  DMA_InitStructure.DMA_Mode                = DMA_MODE_CIRCULAR;
  DMA_InitStructure.DMA_Priority            = DMA_PRIORITY_HIGH;
  DMA_InitStructure.DMA_MTOM                = DMA_MEMTOMEM_DISABLE;
  DMA_Init(DMA1_Channel2, &DMA_InitStructure);
  
  /* DMA1 Channel3 configuration ----------------------------------------------*/
  DMA_Reset(DMA1_Channel3);
  DMA_DefaultInitParaConfig(&DMA_InitStructure);
  DMA_InitStructure.DMA_PeripheralBaseAddr  = (uint32_t)TMR1_CCR2_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr      = (uint32_t)buffer_cc2;
  DMA_InitStructure.DMA_Direction           = DMA_DIR_PERIPHERALSRC;
  DMA_InitStructure.DMA_BufferSize          = 1000;
  DMA_InitStructure.DMA_PeripheralInc       = DMA_PERIPHERALINC_DISABLE;
  DMA_InitStructure.DMA_MemoryInc           = DMA_MEMORYINC_ENABLE;
  DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_HALFWORD;
  DMA_InitStructure.DMA_MemoryDataWidth     = DMA_MEMORYDATAWIDTH_HALFWORD;
  DMA_InitStructure.DMA_Mode                = DMA_MODE_CIRCULAR;
  DMA_InitStructure.DMA_Priority            = DMA_PRIORITY_HIGH;
  DMA_InitStructure.DMA_MTOM                = DMA_MEMTOMEM_DISABLE;
  DMA_Init(DMA1_Channel3, &DMA_InitStructure);
  
  DMA_INTConfig(DMA1_Channel2,DMA_INT_TC,ENABLE);

  /* Enable the DMA1_Channel2 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  DMA_ChannelEnable(DMA1_Channel2,ENABLE);
  DMA_ChannelEnable(DMA1_Channel3,ENABLE);
  
  
  TMR_ICStructInit(&TMR_ICInitStructure);
  TMR_ICInitStructure.TMR_Channel = TMR_Channel_1;
  TMR_ICInitStructure.TMR_ICPolarity = TMR_ICPolarity_Rising;
  TMR_ICInitStructure.TMR_ICSelection = TMR_ICSelection_DirectTI;
  TMR_ICInitStructure.TMR_ICDIV = TMR_ICDIV_DIV1;
  TMR_ICInitStructure.TMR_ICFilter = 0x0;
  
  TMR_PWMIConfig(TMR1, &TMR_ICInitStructure);
  
  /* Select the TMR1 Input Trigger: TI1FP1 */
  TMR_SelectInputTrigger(TMR1, TMR_TRGSEL_TI1FP1);

  /* Select the slave Mode: Reset Mode */
  TMR_SelectSlaveMode(TMR1, TMR_SlaveMode_Reset);

  /* Enable the Master/Slave Mode */
  TMR_SelectMasterSlaveMode(TMR1, TMR_MasterSlaveMode_Enable);
  
  /* Enable the CC1/CC2 DMA */
  TMR_DMACmd(TMR1,TMR_DMA_CC1,ENABLE);
  TMR_DMACmd(TMR1,TMR_DMA_CC2,ENABLE);
  
  /* TMR enable counter */
  TMR_Cmd(TMR1, ENABLE);

  while (1)
  {
  }
}



void DMA1_Channel3_2_IRQHandler(void)
{
  DMA_ClearITPendingBit(DMA1_INT_TC2);
  
  DutyCycle = (buffer_cc2[999] * 100) / buffer_cc1[999];
  Frequency = SystemCoreClock / buffer_cc1[999];
  printf("Freq=%dHZ,Duty=%d%%\r\n",Frequency,DutyCycle);
}

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{
  /* TMR1, GPIOA and DMA1 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR1, ENABLE);  
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA1 | RCC_AHBPERIPH_GPIOA,ENABLE);
}




/**
  * @brief  Configures TMR1 Pin.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure;

  /* GPIOA Configuration: TMR1 Channel1 */
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource8, GPIO_AF_2);
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
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
  
  
