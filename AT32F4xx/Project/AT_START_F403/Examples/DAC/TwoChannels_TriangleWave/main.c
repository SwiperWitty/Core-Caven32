/**
  **************************************************************************
  * File   : main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  **************************************************************************
  */
 
 
#include "at32f4xx.h"

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */
  
/** @addtogroup DAC_TwoChannels_TriangleWave
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Init Structure definition */
DAC_InitType            DAC_InitStructure;
TMR_TimerBaseInitType    TMR_TimeBaseStructure;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void Delay(__IO uint32_t nCount);

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
  TMR_TimeBaseStructInit(&TMR_TimeBaseStructure);
  TMR_TimeBaseStructure.TMR_Period = 0xF;          
  TMR_TimeBaseStructure.TMR_DIV = 0xF;       
  TMR_TimeBaseStructure.TMR_ClockDivision = 0x0;    
  TMR_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;  
  TMR_TimeBaseInit(TMR2, &TMR_TimeBaseStructure);

  /* TIM2 TRGO selection */
  TMR_SelectOutputTrigger(TMR2, TMR_TRGOSource_Update);

  /* DAC channel1 Configuration */
  DAC_StructInit(&DAC_InitStructure);
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_TMR2_TRGO;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_Triangle;
  DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmp_2047;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);

  /* DAC channel2 Configuration */
  DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmp_1023;
  DAC_Init(DAC_Channel_2, &DAC_InitStructure);

  /* Enable DAC Channel1: Once the DAC channel1 is enabled, PA.04 is 
     automatically connected to the DAC converter. */
  DAC_Ctrl(DAC_Channel_1, ENABLE);

  /* Enable DAC Channel2: Once the DAC channel2 is enabled, PA.05 is 
     automatically connected to the DAC converter. */
  DAC_Ctrl(DAC_Channel_2, ENABLE);

  /* Set DAC dual channel DHR12RD register */
  DAC_SetDualChannelData(DAC_Align_12b_Right, 0x100, 0x100);

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

/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length.
  * @retval None
  */
void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
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

  