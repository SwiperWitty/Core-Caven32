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


/** @addtogroup AT32F415_StdPeriph_Examples
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
void SWJ_DP_config(void);
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

  SWJ_DP_config();

  while (1)
  {
    if(BUTTON_WAKEUP == AT32_BUTTON_Press())
    {
      /* Disable the Serial Wire Jtag Debug Port SWJ-DP */
      GPIO_PinsRemapConfig(GPIO_Remap_SWJ_AllDisable, ENABLE);
      /* Turn on LED3 */
      AT32_LEDn_ON(LED3);
    }

    /* Toggle JTMS/SWDAT pin */
    GPIO_TogglePins(GPIOA, GPIO_Pins_13);
    Delay_us(200);

    /* Toggle JTCK/SWCLK pin */
    GPIO_TogglePins(GPIOA, GPIO_Pins_14);
    Delay_us(200);

    /* Toggle JTDI pin */
    GPIO_TogglePins(GPIOA, GPIO_Pins_15);
    Delay_us(200);

    /* Toggle JTDO pin */
    GPIO_TogglePins(GPIOB, GPIO_Pins_3);
    Delay_us(200);

    /* Toggle JTRST pin */
    GPIO_TogglePins(GPIOB, GPIO_Pins_4);
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
  /* Enable GPIOA, GPIOB and AFIO clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA | RCC_APB2PERIPH_GPIOB |
                         RCC_APB2PERIPH_GPIOF | RCC_APB2PERIPH_AFIO, ENABLE);
}

/**
  * @brief  Configure the GPIO of SWJ-DP.
  * @param  None
  * @retval None
  */
void SWJ_DP_config(void)
{
  GPIO_InitType GPIO_InitStructure;

  /* Configure PA.13 (JTMS/SWDAT), PA.14 (JTCK/SWCLK) and PA.15 (JTDI) as
     output push-pull */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_13 | GPIO_Pins_14 | GPIO_Pins_15;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure PB.03 (JTDO) and PB.04 (JTRST) as output push-pull */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_3 | GPIO_Pins_4;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
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

