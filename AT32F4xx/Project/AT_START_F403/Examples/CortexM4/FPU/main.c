/**
  ******************************************************************************
  * File   : CortexM4/FPU/main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 
  
#include <stdio.h>
#include "at32f4xx.h"
#include "at32f4xx_julia.h"
#include "at32_board.h"

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup CortexM4_FPU
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
extern __IO uint32_t TimingDelay;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

#define  SCREEN_X_SIZE               ((uint16_t)320)
#define  SCREEN_Y_SIZE               ((uint8_t)208)
#define  PBAR_X_POS                  ((uint16_t)0x100)
#define  PBAR_Y_POS_H                ((uint8_t)0)
#define  PBAR_Y_POS_L                ((uint8_t)227)
#define  PBAR_COLOR                  ((uint16_t)0xf79e)
#define  ANIMATION_LENGHT            ((uint32_t)26)

#define SUBMODE_FPU_USED_MODE          ((uint8_t)0)
#define SUBMODE_FPU_NOT_USED_MODE      ((uint8_t)1)

const int16_t animation_zoom[ANIMATION_LENGHT] =
  {
    120, 110, 100, 150, 200, 275, 350, 450,
    600, 800, 1000, 1200, 1500, 2000, 1500,
    1200, 1000, 800, 600, 450, 350, 275, 200,
    150, 100, 110
  };
uint8_t buffer[SCREEN_X_SIZE * SCREEN_Y_SIZE];

/**
  * @brief  MCO (PA8) Out.
  * @param  None
  * @retval None
  */
void MCO_Config(void)
{
    GPIO_InitType GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);   

    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_8;
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
  
    /*PA8Êä³öPLL/4*/
    RCC_CLKOUTConfig(RCC_CLKOUT_PLL_Div4);
}

int main(void)
{
  uint8_t   animation_pointer;
  uint32_t aaa =0;
  RCC_ClockType RccClkSource;
  
  MCO_Config();
  
  AT32_Board_Init(); 
  
  UART_Print_Init(115200);
  
  /*Systick³õÊ¼»¯*/
  RCC_GetClocksFreq(&RccClkSource);
  if (SysTick_Config(RccClkSource.AHBCLK_Freq / 1000))
  { 
    /* Capture error */ 
    while (1);
  }

  printf("Test  Star \r\n");
  aaa = TimingDelay;
  
  GenerateJulia_fpu(SCREEN_X_SIZE,
	              SCREEN_Y_SIZE,
	              SCREEN_X_SIZE / 2,
	              SCREEN_Y_SIZE / 2,
	              animation_zoom[animation_pointer],
	              buffer);
  
  printf("Test  end -> Take time: %d ms\r\n", TimingDelay-aaa);
  
  while(1)
  {
  }
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
  {}
}

#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

