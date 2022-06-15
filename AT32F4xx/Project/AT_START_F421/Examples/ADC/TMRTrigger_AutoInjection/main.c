/**
  ******************************************************************************
  * File   : ADC/TMRTrigger_AutoInjection/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "at32_board.h"

/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */

/** @addtogroup ADC_TMRTrigger_AutoInjection
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ADC1_DR_Address    ((uint32_t)ADC1+0x4C)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ADC_InitType           ADC_InitStructure;
DMA_InitType           DMA_InitStructure;
TMR_TimerBaseInitType  TMR_TimeBaseStructure;
TMR_OCInitType         TMR_OCInitStructure;
__IO uint16_t ADC_RegularConvertedValueTab[32], ADC_InjectedConvertedValueTab[32];

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
	AT32_Board_Init();
	/* NVIC configuration */
	NVIC_Configuration();
	
	/* GPIO configuration */
	GPIO_Configuration();
	
	/* Time Base configuration */
	TMR_TimeBaseStructInit(&TMR_TimeBaseStructure); 
	TMR_TimeBaseStructure.TMR_Period            = (1500000/15000)-1;   					// 15KHz       
	TMR_TimeBaseStructure.TMR_DIV               = (SystemCoreClock/1500000)-1;	// timer1_clk=1.5MHz       
	TMR_TimeBaseStructure.TMR_ClockDivision     = 0x0;    
	TMR_TimeBaseStructure.TMR_CounterMode       = TMR_CounterDIR_Up;  
	TMR_TimeBaseStructure.TMR_RepetitionCounter = 0x0000;
	TMR_TimeBaseInit(TMR1, &TMR_TimeBaseStructure);

	/* TMR1 channel1 configuration in PWM mode */
	TMR_OCStructInit(&TMR_OCInitStructure);
	TMR_OCInitStructure.TMR_OCMode      = TMR_OCMode_PWM1; 
	TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;                
	TMR_OCInitStructure.TMR_Pulse       = 10; 
	TMR_OCInitStructure.TMR_OCPolarity  = TMR_OCPolarity_Low;         
	TMR_OC1Init(TMR1, &TMR_OCInitStructure);

	/* DMA1 Channel1 Configuration ----------------------------------------------*/
	DMA_Reset(DMA1_Channel1);
	DMA_DefaultInitParaConfig(&DMA_InitStructure);
	DMA_InitStructure.DMA_PeripheralBaseAddr    = ADC1_DR_Address;
	DMA_InitStructure.DMA_MemoryBaseAddr        = (uint32_t)ADC_RegularConvertedValueTab;
	DMA_InitStructure.DMA_Direction             = DMA_DIR_PERIPHERALSRC;
	DMA_InitStructure.DMA_BufferSize            = 32;
	DMA_InitStructure.DMA_PeripheralInc         = DMA_PERIPHERALINC_DISABLE;
	DMA_InitStructure.DMA_MemoryInc             = DMA_MEMORYINC_ENABLE;
	DMA_InitStructure.DMA_PeripheralDataWidth   = DMA_PERIPHERALDATAWIDTH_HALFWORD;
	DMA_InitStructure.DMA_MemoryDataWidth       = DMA_MEMORYDATAWIDTH_HALFWORD;
	DMA_InitStructure.DMA_Mode                  = DMA_MODE_NORMAL;
	DMA_InitStructure.DMA_Priority              = DMA_PRIORITY_HIGH;
	DMA_InitStructure.DMA_MTOM                  = DMA_MEMTOMEM_DISABLE;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);

	/* Enable DMA1 channel1 */
	DMA_ChannelEnable(DMA1_Channel1, ENABLE);

	/* ADC1 Configuration ------------------------------------------------------*/
	ADC_StructInit(&ADC_InitStructure);
	ADC_InitStructure.ADC_Mode                  = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanMode              = DISABLE;
	ADC_InitStructure.ADC_ContinuousMode        = DISABLE;
	ADC_InitStructure.ADC_ExternalTrig          = ADC_ExternalTrig_TMR1_CC1_ADC12;
	ADC_InitStructure.ADC_DataAlign             = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NumOfChannel          = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	
	/* ADC1 regular channel14 configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 1, ADC_SampleTime_13_5);

	/* Set injected sequencer length */
	ADC_InjectedSequencerLengthConfig(ADC1, 1);
	/* ADC1 injected channel Configuration */ 
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_7_5);
	/* ADC1 injected external trigger configuration */
	ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjec_None);

	/* Enable automatic injected conversion start after regular one */
	ADC_AutoInjectedConvCtrl(ADC1, ENABLE);

	/* Enable ADC1 DMA */
	ADC_DMACtrl(ADC1, ENABLE);

	/* Enable ADC1 external trigger */ 
	ADC_ExternalTrigConvCtrl(ADC1, ENABLE);

	/* Enable JEOC interrupt */
	ADC_INTConfig(ADC1, ADC_INT_JEC, ENABLE);

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
    
    /* TMR1 counter enable */
	TMR_Cmd(TMR1, ENABLE);
	/* TMR1 CH1 Enable */
	TMR_CCxCmd(TMR1, TMR_Channel_1,TMR_CCx_Enable);  
	/* TMR1 main Output Enable */
	TMR_CtrlPWMOutputs(TMR1, ENABLE);
	
	/* Test on channel1 transfer complete flag */
	while(!DMA_GetFlagStatus(DMA1_FLAG_TC1));
	/* Clear channel1 transfer complete flag */
	DMA_ClearFlag(DMA1_FLAG_TC1); 

	/* TMR1 counter disable */
	TMR_Cmd(TMR1, DISABLE);

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

	/* Enable DMA1 clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA1, ENABLE);
	
	/* Enable ADC1 and GPIOC clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_ADC1 | RCC_APB2PERIPH_TMR1 , ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA | RCC_AHBPERIPH_GPIOB , ENABLE);
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
	GPIO_InitType GPIO_InitStructure;
	/* Configure TMR1_CH1 (PA8) as alternate function push-pull */
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pins        = GPIO_Pins_8;
	GPIO_InitStructure.GPIO_MaxSpeed    = GPIO_MaxSpeed_50MHz;
	GPIO_InitStructure.GPIO_Mode        = GPIO_Mode_AF;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA,GPIO_PinsSource8,GPIO_AF_2);

	/* Configure PB.12 and PB.15 (ADC Channel1 and Channel4) as analog input */
	GPIO_InitStructure.GPIO_Pins = GPIO_Pins_12 | GPIO_Pins_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
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
	NVIC_InitStructure.NVIC_IRQChannel                      = ADC_COMP_IRQn;
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

