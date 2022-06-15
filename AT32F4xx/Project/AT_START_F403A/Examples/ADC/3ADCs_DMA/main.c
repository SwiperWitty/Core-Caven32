/**
  ******************************************************************************
  * File   : ADC/3ADCs_DMA/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "at32_board.h"

/** @addtogroup AT32F403A_StdPeriph_Examples
	* @{
	*/

/** @addtogroup ADC_3ADCs_DMA
	* @{
	*/ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ADC_InitType ADC_InitStructure;
DMA_InitType DMA_InitStructure;
__IO uint32_t ADC1ConvertedValue = 0, ADC3ConvertedValue = 0;

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);

/* Private functions ---------------------------------------------------------*/

/**
	* @brief  Main program
	* @param  None
	* @retval None
	*/
int main(void)
{
	/* System clocks configuration */
	RCC_Configuration();

	/* GPIO configuration ------------------------------------------------------*/
	GPIO_Configuration();

	/* DMA1 channel1 configuration ----------------------------------------------*/
	DMA_Reset(DMA1_Channel1);
	DMA_DefaultInitParaConfig(&DMA_InitStructure);
	DMA_InitStructure.DMA_PeripheralBaseAddr    = (uint32_t)&ADC1->RDOR;
	DMA_InitStructure.DMA_MemoryBaseAddr        = (uint32_t)&ADC1ConvertedValue;
	DMA_InitStructure.DMA_Direction             = DMA_DIR_PERIPHERALSRC;
	DMA_InitStructure.DMA_BufferSize            = 1;
	DMA_InitStructure.DMA_PeripheralInc         = DMA_PERIPHERALINC_DISABLE;
	DMA_InitStructure.DMA_MemoryInc             = DMA_MEMORYINC_DISABLE;
	DMA_InitStructure.DMA_PeripheralDataWidth   = DMA_PERIPHERALDATAWIDTH_WORD;
	DMA_InitStructure.DMA_MemoryDataWidth       = DMA_MEMORYDATAWIDTH_WORD;
	DMA_InitStructure.DMA_Mode                  = DMA_MODE_CIRCULAR;
	DMA_InitStructure.DMA_Priority              = DMA_PRIORITY_HIGH;
	DMA_InitStructure.DMA_MTOM                  = DMA_MEMTOMEM_DISABLE;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);  
	/* Enable DMA1 channel1 */
	DMA_ChannelEnable(DMA1_Channel1, ENABLE);

	/* DMA2 channel5 configuration ----------------------------------------------*/
	DMA_Reset(DMA2_Channel5);
	DMA_DefaultInitParaConfig(&DMA_InitStructure);
	DMA_InitStructure.DMA_PeripheralBaseAddr    = (uint32_t)&ADC3->RDOR;
	DMA_InitStructure.DMA_MemoryBaseAddr        = (uint32_t)&ADC3ConvertedValue;
	DMA_InitStructure.DMA_Direction             = DMA_DIR_PERIPHERALSRC;
	DMA_InitStructure.DMA_BufferSize            = 1;
	DMA_InitStructure.DMA_PeripheralInc         = DMA_PERIPHERALINC_DISABLE;
	DMA_InitStructure.DMA_MemoryInc             = DMA_MEMORYINC_DISABLE;
	DMA_InitStructure.DMA_PeripheralDataWidth   = DMA_PERIPHERALDATAWIDTH_WORD;
	DMA_InitStructure.DMA_MemoryDataWidth       = DMA_MEMORYDATAWIDTH_WORD;
	DMA_InitStructure.DMA_Mode                  = DMA_MODE_CIRCULAR;
	DMA_InitStructure.DMA_Priority              = DMA_PRIORITY_HIGH;
	DMA_InitStructure.DMA_MTOM                  = DMA_MEMTOMEM_DISABLE;
	DMA_Init(DMA2_Channel5, &DMA_InitStructure);  
	/* Enable DMA2 channel5 */
	DMA_ChannelEnable(DMA2_Channel5, ENABLE);

	/* ADC1 configuration ------------------------------------------------------*/
	ADC_StructInit(&ADC_InitStructure);
	ADC_InitStructure.ADC_Mode              = ADC_Mode_RegSimult;
	ADC_InitStructure.ADC_ScanMode          = DISABLE;
	ADC_InitStructure.ADC_ContinuousMode    = ENABLE;
	ADC_InitStructure.ADC_ExternalTrig      = ADC_ExternalTrig_None;
	ADC_InitStructure.ADC_DataAlign         = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NumOfChannel      = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	/* ADC1 regular channels configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 1, ADC_SampleTime_28_5);    
	/* Enable ADC1 DMA */
	ADC_DMACtrl(ADC1, ENABLE);

	/* ADC2 configuration ------------------------------------------------------*/
	ADC_StructInit(&ADC_InitStructure);
	ADC_InitStructure.ADC_Mode              = ADC_Mode_RegSimult;
	ADC_InitStructure.ADC_ScanMode          = DISABLE;
	ADC_InitStructure.ADC_ContinuousMode    = ENABLE;
	ADC_InitStructure.ADC_ExternalTrig      = ADC_ExternalTrig_None;
	ADC_InitStructure.ADC_DataAlign         = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NumOfChannel      = 1;
	ADC_Init(ADC2, &ADC_InitStructure);
	/* ADC2 regular channels configuration */ 
	ADC_RegularChannelConfig(ADC2, ADC_Channel_13, 1, ADC_SampleTime_28_5);
  /* Enable ADC2 external trigger conversion */
  ADC_ExternalTrigConvCtrl(ADC2, ENABLE);
	
	/* ADC3 configuration ------------------------------------------------------*/
	ADC_StructInit(&ADC_InitStructure);
	ADC_InitStructure.ADC_Mode              = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanMode          = DISABLE;
	ADC_InitStructure.ADC_ContinuousMode    = ENABLE;
	ADC_InitStructure.ADC_ExternalTrig      = ADC_ExternalTrig_None;
	ADC_InitStructure.ADC_DataAlign         = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NumOfChannel      = 1;
	ADC_Init(ADC3, &ADC_InitStructure);
	/* ADC3 regular channels configuration */ 
	ADC_RegularChannelConfig(ADC3, ADC_Channel_12, 1, ADC_SampleTime_28_5);
	/* Enable ADC3 DMA */
	ADC_DMACtrl(ADC3, ENABLE);
	
	/* Enable ADC1 */
	ADC_Ctrl(ADC1, ENABLE);
	
	/* Enable ADC2 */
	ADC_Ctrl(ADC2, ENABLE);
	
	/* Enable ADC1 reset calibration register */   
	ADC_RstCalibration(ADC1);
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));

	/* Start ADC1 calibration */
	ADC_StartCalibration(ADC1);
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));

	/* Enable ADC2 reset calibration register */   
	ADC_RstCalibration(ADC2);
	/* Check the end of ADC2 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC2));

	/* Start ADC2 calibration */
	ADC_StartCalibration(ADC2);
	/* Check the end of ADC2 calibration */
	while(ADC_GetCalibrationStatus(ADC2));

	/* Enable ADC3 */
	ADC_Ctrl(ADC3, ENABLE);

	/* Enable ADC3 reset calibration register */   
	ADC_RstCalibration(ADC3);
	/* Check the end of ADC3 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC3));

	/* Start ADC3 calibration */
	ADC_StartCalibration(ADC3);
	/* Check the end of ADC3 calibration */
	while(ADC_GetCalibrationStatus(ADC3));

	/* Start ADC1 Software Conversion */ 
	ADC_SoftwareStartConvCtrl(ADC1, ENABLE);
	/* Start ADC3 Software Conversion */ 
	ADC_SoftwareStartConvCtrl(ADC3, ENABLE);

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
	/* ADCCLK = PCLK2/6 */
	RCC_ADCCLKConfig(RCC_APB2CLK_Div6);
	
	/* Enable peripheral clocks ------------------------------------------------*/
	/* Enable DMA1 and DMA2 clocks */
	RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA1 | RCC_AHBPERIPH_DMA2, ENABLE);

	/* Enable ADC1, ADC2, ADC3 and GPIOC clocks */
	RCC_APB2PeriphClockCmd(	RCC_APB2PERIPH_ADC1 | RCC_APB2PERIPH_ADC2 |
													RCC_APB2PERIPH_ADC3 | RCC_APB2PERIPH_GPIOC, ENABLE);
}

/**
	* @brief  Configures the different GPIO ports.
	* @param  None
	* @retval None
	*/
void GPIO_Configuration(void)
{
	GPIO_InitType GPIO_InitStructure;

	/* Configure PC.02, PC.03 and PC.04 (ADC Channel12, ADC Channel13 and 
	 ADC Channel14) as analog inputs */
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pins = GPIO_Pins_2 | GPIO_Pins_3 | GPIO_Pins_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_ANALOG;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/**
	* @}
	*/ 

/**
	* @}
	*/ 

