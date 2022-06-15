#include <stdio.h>
#include "at32_board.h"
#include "xmc_lcd.h"

/** @addtogroup AT32F403A_StdPeriph_Examples
  * @{
  */

/** @addtogroup XMC_LCD_8BIT
  * @{
  */ 
uint16_t POINT_COLOR;
int main()
{
  /*  Configures the HardWare, LED, KEY, DELAY ..etc. */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  AT32_Board_Init();
  UART_Print_Init(115200);
  
  /* Configures XMC&LCD */
  LCD_init();
  
  /* Clear the LCD */
  POINT_COLOR = WHITE;
  LCD_Clear(POINT_COLOR);

  while(1)
  {
    /* Display number on LCD */
    LCD_Display_Number();
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

