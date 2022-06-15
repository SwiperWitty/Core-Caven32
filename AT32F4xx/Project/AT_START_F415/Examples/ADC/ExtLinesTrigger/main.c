/**
  ******************************************************************************
  * File   : ADC/ExtLinesTrigger/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 
	
/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "at32_board.h"

/** @addtogroup AT32F415_StdPeriph_Examples
  * @{
  */

/** @addtogroup ADC_ExtLinesTrigger
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ADC1_DR_Address    ((uint32_t)0x4001244C)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ADC_InitType   ADC_InitStructure;
DMA_InitType   DMA_InitStructure;
__IO uint16_t ADC_RegularConvertedValueTab[64], ADC_InjectedConvertedValueTab[32];

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void EXTI_Configuration(void);

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
	
	/* EXTI configuration */
	EXTI_Configuration();
	
	/* DMA1 channel1 configuration ----------------------------------------------*/
	DMA_Reset(DMA1_Channel1);
	DMA_DefaultInitParaConfig(&DMA_InitStructure);
	DMA_InitStructure.DMA_PeripheralBaseAddr    = ADC1_DR_Address;
	DMA_InitStructure.DMA_MemoryBaseAddr        = (uint32_t)ADC_RegularConvertedValueTab;
	DMA_InitStructure.DMA_Direction             = DMA_DIR_PERIPHERALSRC;
	DMA_InitStructure.DMA_BufferSize            = 64;
	DMA_InitStructure.DMA_PeripheralInc         = DMA_PERIPHERALINC_DISABLE;
	DMA_InitStructure.DMA_MemoryInc             = DMA_MEMORYINC_ENABLE;
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
	ADC_InitStructure.ADC_ScanMode          = ENABLE;
	ADC_InitStructure.ADC_ContinuousMode    = DISABLE;
	ADC_InitStructure.ADC_ExternalTrig      = ADC_ExternalTrig_Ext_INT11_TMR8_TRGO_ADC12;
	ADC_InitStructure.ADC_DataAlign         = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NumOfChannel      = 2;
	ADC_Init(ADC1, &ADC_InitStructure);

	/* ADC1 regular channels configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_28_5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 2, ADC_SampleTime_28_5);

	/* Regular discontinuous mode channel number configuration */
	ADC_DiscModeChannelCountConfig(ADC1, 1);
	/* Enable regular discontinuous mode */
	ADC_DiscModeCtrl(ADC1, ENABLE);

	/* Enable ADC1 external trigger conversion */ 
	ADC_ExternalTrigConvCtrl(ADC1, ENABLE);

	/* Set injected sequencer length */
	ADC_InjectedSequencerLengthConfig(ADC1, 2);
	/* ADC1 injected channel configuration */ 
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_28_5);
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_12, 2, ADC_SampleTime_28_5);
	/* ADC1 injected external trigger configuration */
	ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjec_Ext_INT15_TMR8_CC4_ADC12);
	/* Enable ADC1 injected external trigger conversion */
	ADC_ExternalTrigInjectedConvCtrl(ADC1, ENABLE);

	/* Enable JEOC interrupt */
	ADC_INTConfig(ADC1, ADC_INT_JEC, ENABLE);

	/* Enable ADC1 DMA */
	ADC_DMACtrl(ADC1, ENABLE);

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

	/* Enable GPIOs and ADC1 clock */
	RCC_APB2PeriphClockCmd(	RCC_APB2PERIPH_GPIOA | RCC_APB2PERIPH_GPIOC | 
													RCC_APB2PERIPH_GPIOB | RCC_APB2PERIPH_AFIO |
													RCC_APB2PERIPH_ADC1, ENABLE);
}

/**
  * @brief  Configures the different EXTI lines.
  * @param  None
  * @retval None
  */
void EXTI_Configuration(void)
{
	EXTI_InitType EXTI_InitStructure;

	/* Select the EXTI Line11 the GPIO pin source */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinsSource11);
	/* EXTI Line11 configuration -----------------------------------------------*/  
	EXTI_StructInit(&EXTI_InitStructure);
	EXTI_InitStructure.EXTI_Mode        = EXTI_Mode_Event;
	EXTI_InitStructure.EXTI_Trigger     = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_Line        = EXTI_Line11;
	EXTI_InitStructure.EXTI_LineEnable  = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Select the EXTI Line15 the GPIO pin source */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinsSource15);
	/* EXTI line15 configuration -----------------------------------------------*/ 
	EXTI_StructInit(&EXTI_InitStructure);
	EXTI_InitStructure.EXTI_Mode        = EXTI_Mode_Event;
	EXTI_InitStructure.EXTI_Trigger     = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_Line        = EXTI_Line15;
	EXTI_InitStructure.EXTI_LineEnable  = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
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
	as analog input -----------------------------------------------------------*/
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pins = GPIO_Pins_1 | GPIO_Pins_2 | GPIO_Pins_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_ANALOG;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Configure PA.04 (ADC Channel4) as analog input --------------------------*/
	GPIO_InitStructure.GPIO_Pins = GPIO_Pins_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_ANALOG;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure EXTI line11 ---------------------------------------------------*/
	GPIO_InitStructure.GPIO_Pins = GPIO_Pins_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure EXTI line15 ---------------------------------------------------*/
	GPIO_InitStructure.GPIO_Pins = GPIO_Pins_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
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

