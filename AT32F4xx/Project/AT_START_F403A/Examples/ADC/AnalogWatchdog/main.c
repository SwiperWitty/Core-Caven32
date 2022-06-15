/**
  ******************************************************************************
  * File   : ADC/AnalogWatchdog/main.c 
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

/** @addtogroup ADC_AnalogWatchdog
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ADC_InitType  ADC_InitStructure;

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
	
	/* NVIC configuration */
	NVIC_Configuration();
	
	/* GPIO configuration */
	GPIO_Configuration();

	/* AT32 Board Initial */
	AT32_Board_Init();

	/* ADC1 Configuration ------------------------------------------------------*/
	ADC_StructInit(&ADC_InitStructure);
	ADC_InitStructure.ADC_Mode                  = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanMode              = DISABLE;
	ADC_InitStructure.ADC_ContinuousMode        = ENABLE;
	ADC_InitStructure.ADC_ExternalTrig          = ADC_ExternalTrig_None;
	ADC_InitStructure.ADC_DataAlign             = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NumOfChannel          = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	/* ADC1 regular channel14 configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 1, ADC_SampleTime_28_5);

	/* Configure high and low analog watchdog thresholds */
	ADC_AnalogWDGThresholdsConfig(ADC1, 0x0B00, 0x0300);
	/* Configure channel14 as the single analog watchdog guarded channel */
	ADC_AnalogWDGSingleChannelConfig(ADC1, ADC_Channel_14);
	/* Enable analog watchdog on one regular channel */
	ADC_AnalogWDGCtrl(ADC1, ADC_AnalogWDG_SingleRegEnable);

	/* Enable AWD interrupt */
	ADC_INTConfig(ADC1, ADC_INT_AWD, ENABLE);

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

	/* Enable ADC1 and GPIOC clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_ADC1 | RCC_APB2PERIPH_GPIOC, ENABLE);
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
	GPIO_InitType GPIO_InitStructure;

	/* Configure PC.04 (ADC Channel14) as analog input */
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pins = GPIO_Pins_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_ANALOG;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/**
  * @brief  Configures NVIC and Vector Table base location.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
	NVIC_InitType NVIC_InitStructure;
	
	/* Configure and enable ADC interrupt */
	NVIC_InitStructure.NVIC_IRQChannel                      = ADC1_2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority           = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**
  * @}
  */ 

/**
  * @}
  */ 

