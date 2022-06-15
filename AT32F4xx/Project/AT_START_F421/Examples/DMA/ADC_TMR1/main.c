 /**
 **************************************************************************
 * File   : DMA/ADC_TMR1/main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
 * Brief  : Main program body
 **************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "at32_board.h"

/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */

/** @addtogroup DMA_ADC_TMR1
  * @{
  */ 
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ADC1_DR_Address     0x4001244C
#define TMR1_CCR1_Address   0x40012C34

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ADC_InitType            ADC_InitStructure      = {0};
TMR_TimerBaseInitType   TMR_TimeBaseStructure  = {0};
TMR_OCInitType          TMR_OCInitStructure    = {0};
DMA_InitType            DMA_InitStructure      = {0};
  
/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
   
/* Private functions ---------------------------------------------------------*/

/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* System Clocks Configuration */
  RCC_Configuration();

  /* Configure the GPIO ports */
  GPIO_Configuration();

  /* Board.c init */
  AT32_Board_Init();
  
  /* DMA1 Channel5 configuration ----------------------------------------------*/
  DMA_Reset(DMA1_Channel5);
  DMA_DefaultInitParaConfig(&DMA_InitStructure);
  DMA_InitStructure.DMA_PeripheralBaseAddr  = (uint32_t)TMR1_CCR1_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr      = (uint32_t)ADC1_DR_Address;
  DMA_InitStructure.DMA_Direction           = DMA_DIR_PERIPHERALDST;
  DMA_InitStructure.DMA_BufferSize          = 1;
  DMA_InitStructure.DMA_PeripheralInc       = DMA_PERIPHERALINC_DISABLE;
  DMA_InitStructure.DMA_MemoryInc           = DMA_MEMORYINC_DISABLE;
  DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_HALFWORD;
  DMA_InitStructure.DMA_MemoryDataWidth     = DMA_MEMORYDATAWIDTH_HALFWORD;
  DMA_InitStructure.DMA_Mode                = DMA_MODE_CIRCULAR;
  DMA_InitStructure.DMA_Priority            = DMA_PRIORITY_HIGH;
  DMA_InitStructure.DMA_MTOM                = DMA_MEMTOMEM_DISABLE;
  DMA_Init(DMA1_Channel5, &DMA_InitStructure);
  /* Enable DMA1 Channel5 */
  DMA_ChannelEnable(DMA1_Channel5, ENABLE);

  /* ADC1 configuration ------------------------------------------------------*/
  ADC_StructInit(&ADC_InitStructure);
  ADC_InitStructure.ADC_Mode           = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanMode       = DISABLE;
  ADC_InitStructure.ADC_ContinuousMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrig   = ADC_ExternalTrig_None;
  ADC_InitStructure.ADC_DataAlign      = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NumOfChannel   = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 RegularChannelConfig Test */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 1, ADC_SampleTime_55_5);

  /* TMR1 configuration ------------------------------------------------------*/
  /* Time Base configuration */
  TMR_TimeBaseStructInit(&TMR_TimeBaseStructure); 
  TMR_TimeBaseStructure.TMR_Period        = 0xFF0;
  TMR_TimeBaseStructure.TMR_DIV           = 0x0;
  TMR_TimeBaseStructure.TMR_ClockDivision = 0x0;
  TMR_TimeBaseStructure.TMR_CounterMode   = TMR_CounterDIR_Up;
  TMR_TimeBaseInit(TMR1, &TMR_TimeBaseStructure);
  /* Channel1 Configuration in PWM mode */
  TMR_OCInitStructure.TMR_OCMode      = TMR_OCMode_PWM1; 
  TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
  TMR_OCInitStructure.TMR_OCPolarity  = TMR_OCPolarity_High;
  TMR_OC1Init(TMR1, &TMR_OCInitStructure);

  /* Enable TMR1 */  
  TMR_Cmd(TMR1, ENABLE);
  /* Enable TMR1 outputs */
  TMR_CtrlPWMOutputs(TMR1, ENABLE);

  /* Enable TMR1 DMA interface */
  TMR_DMACmd(TMR1, TMR_DMA_Update, ENABLE);

  /* Enable ADC1 */
  ADC_Ctrl(ADC1, ENABLE);

  /* Enable ADC1 reset calibration register */
  ADC_RstCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));

  /* Start ADC1 calibration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));

  /* Start ADC1 conversion */ 
  ADC_SoftwareStartConvCtrl(ADC1, ENABLE);

  while (1)
  {
    /* Delay 1000ms */
    Delay_ms(1000);
    
    /* Toggle LED4*/
    AT32_LEDn_Toggle(LED4);
  }
}

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{
  /* ADCCLK = PCLK2/4 */
  RCC_ADCCLKConfig(RCC_APB2CLK_Div4);   

  /* Enable peripheral clocks ------------------------------------------------*/
  /* Enable DMA1 clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA1, ENABLE);
  /* Enable GPIOA, ADC1 and TMR1 Periph clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_ADC1 | RCC_APB2PERIPH_TMR1, ENABLE);
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure = {0};

  /* Configure TMR1 Channel1 output */
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource8, GPIO_AF_2);  
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_OutType = GPIO_OutType_PP;
  GPIO_InitStructure.GPIO_Pull = GPIO_Pull_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  
  /* Configure PA0 (ADC Channel0) as analog input -------------------------*/
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
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

  printf("Wrong parameters value: file %s on line %d\r\n", file, line);

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
  
