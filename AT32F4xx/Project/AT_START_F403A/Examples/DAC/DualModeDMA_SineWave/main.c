/**
  **************************************************************************
  * File   : main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  **************************************************************************
  */


#include "at32f4xx.h"

/** @addtogroup AT32F403A_StdPeriph_Examples
  * @{
  */

/** @addtogroup DAC_DualModeDMA_SineWave
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DAC_DHR12RD_Address      0x40007420

/* Init Structure definition */
DAC_InitType            DAC_InitStructure;
DMA_InitType            DMA_InitStructure;
TMR_TimerBaseInitType    TMR_TimeBaseStructure;
uint32_t Idx = 0;  
  
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const uint16_t Sine12bit[32] = {
                      2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
                      3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909, 
                      599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647};

uint32_t DualSine12bit[32];

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief   Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_at32f403_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_at32f4xx.c file
     */     
	  
  
  /* System Clocks Configuration */
  RCC_Configuration();   

  /* Once the DAC channel is enabled, the corresponding GPIO pin is automatically 
     connected to the DAC converter. In order to avoid parasitic consumption, 
     the GPIO pin should be configured in analog */
  GPIO_Configuration();

  /* TIM2 Configuration */
  /* Time base configuration */
  TMR_TimeBaseStructInit(&TMR_TimeBaseStructure); 
  TMR_TimeBaseStructure.TMR_Period = 0x19;          
  TMR_TimeBaseStructure.TMR_DIV = 0x0;       
  TMR_TimeBaseStructure.TMR_ClockDivision = 0x0;    
  TMR_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;  
  TMR_TimeBaseInit(TMR2, &TMR_TimeBaseStructure);

  /* TIM2 TRGO selection */
  TMR_SelectOutputTrigger(TMR2, TMR_TRGOSource_Update);

  /* DAC channel1 Configuration */
  DAC_StructInit(&DAC_InitStructure);
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_TMR2_TRGO;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);

  /* DAC channel2 Configuration */
  DAC_Init(DAC_Channel_2, &DAC_InitStructure);

  /* Fill Sine32bit table */
  for (Idx = 0; Idx < 32; Idx++)
  { 
    DualSine12bit[Idx] = (Sine12bit[Idx] << 16) + (Sine12bit[Idx]);
  }

  /* DMA2 channel4 configuration */
  DMA_Reset(DMA2_Channel4);
  
  DMA_DefaultInitParaConfig(&DMA_InitStructure);
  DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12RD_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&DualSine12bit;
  DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALDST;
  DMA_InitStructure.DMA_BufferSize = 32;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;
  DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE;
  DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_WORD;
  DMA_InitStructure.DMA_MemoryDataWidth = DMA_MEMORYDATAWIDTH_WORD;
  DMA_InitStructure.DMA_Mode = DMA_MODE_CIRCULAR;
  DMA_InitStructure.DMA_Priority = DMA_PRIORITY_HIGH;
  DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;

  DMA_Init(DMA2_Channel4, &DMA_InitStructure);
  /* Enable DMA2 Channel4 */
  DMA_ChannelEnable(DMA2_Channel4, ENABLE);

  /* Enable DAC Channel1: Once the DAC channel1 is enabled, PA.04 is 
     automatically connected to the DAC converter. */
  DAC_Ctrl(DAC_Channel_1, ENABLE);
  /* Enable DAC Channel2: Once the DAC channel2 is enabled, PA.05 is 
     automatically connected to the DAC converter. */
  DAC_Ctrl(DAC_Channel_2, ENABLE);

  /* Enable DMA for DAC Channel2 */
  DAC_DMACtrl(DAC_Channel_2, ENABLE);

  /* TIM2 enable counter */
  TMR_Cmd(TMR2, ENABLE);

  while (1)
  {
  }
}


/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{   
  /* Enable peripheral clocks ------------------------------------------------*/
    
  /* DMA2 clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA2, ENABLE);

  /* GPIOA Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);
  /* DAC Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_DAC, ENABLE);
  /* TIM2 Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR2, ENABLE);
}


/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure;

  /* Once the DAC channel is enabled, the corresponding GPIO pin is automatically 
     connected to the DAC converter. In order to avoid parasitic consumption, 
     the GPIO pin should be configured in analog */
  GPIO_InitStructure.GPIO_Pins =  GPIO_Pins_4 | GPIO_Pins_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_ANALOG;
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

  /* Infinite loop */
  while (1)
  {
  }
}

#endif

/**
  * @}
  */ 

/**
  * @}
  */ 
  
