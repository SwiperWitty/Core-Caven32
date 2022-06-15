/**
  ******************************************************************************
  * File   : ADC/RegSimul_DualMode/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 
	
/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "at32_board.h"

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup ADC_RegSimul_DualMode
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ADC1_DR_Address    ((uint32_t)0x4001244C)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ADC_InitType ADC_InitStructure;
DMA_InitType DMA_InitStructure;
__IO uint32_t ADC_DualConvertedValueTab[16];

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);

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
	
	/* GPIO configuration */
	GPIO_Configuration();

	/* DMA1 channel1 configuration ----------------------------------------------*/
	DMA_Reset(DMA1_Channel1);
	DMA_DefaultInitParaConfig(&DMA_InitStructure);
	DMA_InitStructure.DMA_PeripheralBaseAddr    = (uint32_t)ADC1_DR_Address;
	DMA_InitStructure.DMA_MemoryBaseAddr        = (uint32_t)ADC_DualConvertedValueTab;
	DMA_InitStructure.DMA_Direction             = DMA_DIR_PERIPHERALSRC;
	DMA_InitStructure.DMA_BufferSize            = 16;
	DMA_InitStructure.DMA_PeripheralInc         = DMA_PERIPHERALINC_DISABLE;
	DMA_InitStructure.DMA_MemoryInc             = DMA_MEMORYINC_ENABLE;
	DMA_InitStructure.DMA_PeripheralDataWidth   = DMA_PERIPHERALDATAWIDTH_WORD;
	DMA_InitStructure.DMA_MemoryDataWidth       = DMA_MEMORYDATAWIDTH_WORD;
	DMA_InitStructure.DMA_Mode                  = DMA_MODE_NORMAL;
	DMA_InitStructure.DMA_Priority              = DMA_PRIORITY_HIGH;
	DMA_InitStructure.DMA_MTOM                  = DMA_MEMTOMEM_DISABLE;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	/* Enable DMA1 Channel1 */
	DMA_ChannelEnable(DMA1_Channel1, ENABLE);

	/* ADC1 configuration ------------------------------------------------------*/
	ADC_StructInit(&ADC_InitStructure);
	ADC_InitStructure.ADC_Mode              = ADC_Mode_RegSimult;
	ADC_InitStructure.ADC_ScanMode          = ENABLE;
	ADC_InitStructure.ADC_ContinuousMode    = ENABLE;
	ADC_InitStructure.ADC_ExternalTrig      = ADC_ExternalTrig_None;
	ADC_InitStructure.ADC_DataAlign         = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NumOfChannel      = 2;
	ADC_Init(ADC1, &ADC_InitStructure);
	/* ADC1 regular channels configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 1, ADC_SampleTime_239_5);    
	ADC_RegularChannelConfig(ADC1, ADC_Channel_17, 2, ADC_SampleTime_239_5);
	/* Enable ADC1 DMA */
	ADC_DMACtrl(ADC1, ENABLE);

	/* ADC2 configuration ------------------------------------------------------*/
	ADC_StructInit(&ADC_InitStructure);
	ADC_InitStructure.ADC_Mode              = ADC_Mode_RegSimult;
	ADC_InitStructure.ADC_ScanMode          = ENABLE;
	ADC_InitStructure.ADC_ContinuousMode    = ENABLE;
	ADC_InitStructure.ADC_ExternalTrig      = ADC_ExternalTrig_None;
	ADC_InitStructure.ADC_DataAlign         = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NumOfChannel      = 2;
	ADC_Init(ADC2, &ADC_InitStructure);
	/* ADC2 regular channels configuration */ 
	ADC_RegularChannelConfig(ADC2, ADC_Channel_11, 1, ADC_SampleTime_239_5);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_12, 2, ADC_SampleTime_239_5);
	/* Enable ADC2 external trigger conversion */
	ADC_ExternalTrigConvCtrl(ADC2, ENABLE);

	/* Enable ADC1 */
	ADC_Ctrl(ADC1, ENABLE);
	/* Enable Vrefint channel17 */
	ADC_TempSensorVrefintCtrl(ENABLE);

	/* Enable ADC1 reset calibration register */   
	ADC_RstCalibration(ADC1);
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));

	/* Start ADC1 calibration */
	ADC_StartCalibration(ADC1);
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));

	/* Enable ADC2 */
	ADC_Ctrl(ADC2, ENABLE);

	/* Enable ADC2 reset calibration register */   
	ADC_RstCalibration(ADC2);
	/* Check the end of ADC2 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC2));

	/* Start ADC2 calibration */
	ADC_StartCalibration(ADC2);
	/* Check the end of ADC2 calibration */
	while(ADC_GetCalibrationStatus(ADC2));

	/* Start ADC1 Software Conversion */ 
	ADC_SoftwareStartConvCtrl(ADC1, ENABLE);

	/* Test on DMA1 channel1 transfer complete flag */
	while(!DMA_GetFlagStatus(DMA1_FLAG_TC1));
	/* Clear DMA1 channel1 transfer complete flag */
	DMA_ClearFlag(DMA1_FLAG_TC1);

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
	/* ADCCLK = PCLK2/4 */
	RCC_ADCCLKConfig(RCC_APB2CLK_Div4);

	/* Enable DMA1 clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA1, ENABLE);

	/* Enable ADC1, ADC2 and GPIOC clock */
	RCC_APB2PeriphClockCmd(	RCC_APB2PERIPH_ADC1 | RCC_APB2PERIPH_ADC2 |
													RCC_APB2PERIPH_GPIOC, ENABLE);
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
	GPIO_InitType GPIO_InitStructure;

	/* Configure PC.01, PC.02 and PC.04 (ADC Channel11, Channel12 and Channel14)
	as analog input ----------------------------------------------------------*/
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pins = GPIO_Pins_1 | GPIO_Pins_2 | GPIO_Pins_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_ANALOG;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/**
  * @}
  */ 

/**
  * @}
  */ 

