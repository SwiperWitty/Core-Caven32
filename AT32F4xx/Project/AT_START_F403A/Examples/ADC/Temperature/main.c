/**
  ******************************************************************************
  * File   : ADC/Temperature/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "at32_board.h"
#include <stdio.h>

/** @addtogroup AT32F403A_StdPeriph_Examples
  * @{
  */

/** @addtogroup ADC_Temperature
  * @{
  */ 
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ADC1_DR_Address		((uint32_t)0x4001244C)
#define ADC_VREF   				(3.3)
#define ADC_TEMP_BASE   	(1.26)
#define ADC_TEMP_SLOPE   	(-0.00423)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ADC_InitType ADC_InitStructure;
DMA_InitType DMA_InitStructure;
__IO uint16_t ADCConvertedValue;

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);

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

	/*initialize Delay Function*/
	Delay_init();

	/* USART configuration */
	UART_Print_Init(115200);
	
	/* DMA1 channel1 configuration ----------------------------------------------*/
	DMA_Reset(DMA1_Channel1);
	DMA_DefaultInitParaConfig(&DMA_InitStructure);
	DMA_InitStructure.DMA_PeripheralBaseAddr    = ADC1_DR_Address;
	DMA_InitStructure.DMA_MemoryBaseAddr        = (uint32_t)&ADCConvertedValue;
	DMA_InitStructure.DMA_Direction             = DMA_DIR_PERIPHERALSRC;
	DMA_InitStructure.DMA_BufferSize            = 1;
	DMA_InitStructure.DMA_PeripheralInc         = DMA_PERIPHERALINC_DISABLE;
	DMA_InitStructure.DMA_MemoryInc             = DMA_MEMORYINC_DISABLE;
	DMA_InitStructure.DMA_PeripheralDataWidth   = DMA_PERIPHERALDATAWIDTH_HALFWORD;
	DMA_InitStructure.DMA_MemoryDataWidth       = DMA_MEMORYDATAWIDTH_HALFWORD;
	DMA_InitStructure.DMA_Mode                  = DMA_MODE_CIRCULAR;
	DMA_InitStructure.DMA_Priority              = DMA_PRIORITY_HIGH;
	DMA_InitStructure.DMA_MTOM                  = DMA_MEMTOMEM_DISABLE;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);  
	/* Enable DMA1 channel1 */
	DMA_ChannelEnable(DMA1_Channel1, ENABLE);

	/* ADC1 configuration ------------------------------------------------------*/
	ADC_StructInit(&ADC_InitStructure);
	ADC_InitStructure.ADC_Mode              = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanMode          = DISABLE;
	ADC_InitStructure.ADC_ContinuousMode    = ENABLE;
	ADC_InitStructure.ADC_ExternalTrig      = ADC_ExternalTrig_None;
	ADC_InitStructure.ADC_DataAlign         = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NumOfChannel      = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	/* ADC1 regular channels configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_TempSensor, 1, ADC_SampleTime_239_5);  
	/* Enable ADC1 DMA */
	ADC_DMACtrl(ADC1, ENABLE);		
	
	/* Enables Temperature Sensor and Vrefint Channel */
	ADC_TempSensorVrefintCtrl(ENABLE);
	
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

	/* Start ADC1 Software Conversion */ 
	ADC_SoftwareStartConvCtrl(ADC1, ENABLE);
	
	while (1)
	{
		Delay_sec(1);
		printf("Temperature: %f deg C\r\n",(ADC_TEMP_BASE-(double)ADCConvertedValue*ADC_VREF/4096)/ADC_TEMP_SLOPE+25);
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
	/* Enable DMA1 clocks */
	RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA1, ENABLE);

	/* Enable ADC1 clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_ADC1, ENABLE);
}

/**
  * @}
  */ 

/**
  * @}
  */ 

