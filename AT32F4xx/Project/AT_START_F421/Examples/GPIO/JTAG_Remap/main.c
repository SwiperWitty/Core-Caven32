/**
  ******************************************************************************
  * File   : GPIO/JTAG_Remap/main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */

#include "at32f4xx.h"
#include "at32_board.h"


/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */
  
/** @addtogroup GPIO_JTAG_Remap
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void SWJ_DP_Disable(void);
void GPIO_TogglePins(GPIO_Type* GPIOx, uint16_t GPIO_Pin);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  RCC_Configuration();

  /* Initialize LED and KEY*/
  AT32_Board_Init();

  while (1)
  {
    if(BUTTON_WAKEUP == AT32_BUTTON_Press())
    {
      /* Disable the Serial Wire Jtag Debug Port SWJ-DP */
			SWJ_DP_Disable();
			
      /* Turn on LED4 */
      AT32_LEDn_ON(LED4);
    }

    /* Toggle SWDIO pin */
    GPIO_TogglePins(GPIOA, GPIO_Pins_13);
    Delay_us(200);

    /* Toggle SWCLK pin */
    GPIO_TogglePins(GPIOA, GPIO_Pins_14);
    Delay_us(200);
  }
}

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{
  /* Enable GPIOA, GPIOB and GPIOF clocks */
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA | RCC_AHBPERIPH_GPIOB |
                         RCC_AHBPERIPH_GPIOF, ENABLE);
}

/**
  * @brief  Disable the Serial Wire Jtag Debug Port SWJ-DP.
  * @param  None
  * @retval None
  */
void SWJ_DP_Disable(void)
{
  GPIO_InitType GPIO_InitStructure;

  /* Configure PA.13 (SWDIO), PA.14 (SWCLK) as
     output push-pull */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_13 | GPIO_Pins_14;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OutType = GPIO_OutType_PP;
	GPIO_InitStructure.GPIO_Pull = GPIO_Pull_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
  * @brief  Toggles the specified GPIO pin
  * @param  GPIOx: where x can be (A..G depending on device used) to select the GPIO peripheral
  * @param  GPIO_Pin: Specifies the pins to be toggled.
  * @retval None
  */
void GPIO_TogglePins(GPIO_Type* GPIOx, uint16_t GPIO_Pin)
{
  /* Check the parameters */
  assert_param(IS_GPIO_PINS(GPIO_Pin));

  GPIOx->OPTDT ^= GPIO_Pin;
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

