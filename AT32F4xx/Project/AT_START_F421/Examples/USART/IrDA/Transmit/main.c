/**
  ******************************************************************************
  * File   : USART/IrDA/Transmit/main.c 
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

/** @addtogroup USART_IrDA_Transmit
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USART_InitType USART_InitStructure;
GPIO_InitType GPIO_InitStructure;
/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
char ReadKey(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{  
  /* System Clocks Configuration */
  RCC_Configuration();

  /* Configure the GPIO ports */
  GPIO_Configuration();

  /* Initialize PA0 pin*/       
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA, ENABLE);

  /* Configure WAKEUP pin */
  AT32_BUTTON_Init(BUTTON_WAKEUP);
  /* USART1 configuration ------------------------------------------------------*/
  /* USART1 configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_StructInit(&USART_InitStructure);
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  /* Configure the USART1 */
  USART_Init(USART1, &USART_InitStructure);
  /* Enable the USART1 */
  USART_Cmd(USART1, ENABLE);
  /* Set the USART1 prescaler */
  USART_SetPrescaler(USART1, 0x1);
  /* Configure the USART1 IrDA mode */
  USART_IrDAConfig(USART1, USART_IrDAMode_Normal);

  /* Enable the USART1 IrDA mode */
  USART_IrDACmd(USART1, ENABLE);

  while (1)
  {
    /* Judge whether PA0 press down */
    if(ReadKey() == 1)
    {
      /* USART1 send Data*/
      USART_SendData(USART1, 0xAA);
      /* Checks whether the USART TXE flag is set*/
      while(USART_GetFlagStatus(USART1, USART_FLAG_TDE) == RESET)
      {
      }
    }
  }
}

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{  
  /* Enable GPIO clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA, ENABLE);
  
  /* Enable USART1 clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_USART1, ENABLE);
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure;

  /* Connect PXx to USART1_Tx */
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource9, GPIO_AF_1);
  /* Connect PXx to USART1_Rx */
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource10, GPIO_AF_1);

  /* Configure USART1 Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_9 | GPIO_Pins_10;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OutType = GPIO_OutType_PP;
  GPIO_InitStructure.GPIO_Pull = GPIO_Pull_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
  * @brief  Reads key from board.
  * @param  None
  * @retval 0 : haven't press down PA0
  *         1 : press down PA0
  */
char ReadKey(void)
{
  /* "right" key is pressed */
  if(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pins_0))
  {
    while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pins_0)== Bit_RESET);
    return 1; 
  }
  return 0;
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



