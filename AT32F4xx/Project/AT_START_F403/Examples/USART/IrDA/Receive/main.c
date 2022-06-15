/**
  ******************************************************************************
  * File   : USART/IrDA/Receive/main.c 
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

/** @addtogroup USART_IrDA_Receive
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USART_InitType USART_InitStructure;
int ReceivedData = 0;
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
  /* System Clocks Configuration */
  RCC_Configuration();

  /* Configure the GPIO ports */
  GPIO_Configuration();

  /* Initialize Leds mounted on board */
  AT32_Board_Init();

  /* USART3 configuration ------------------------------------------------------*/
  /* USART3 configured as follow:
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
  
  /* Configure the USART3 */
  USART_Init(USART3, &USART_InitStructure);
  /* Enable the USART3 */
  USART_Cmd(USART3, ENABLE);

  /* Set the USART3 prescaler */
  USART_SetPrescaler(USART3, 0x1);
  /* Configure the USART3 IrDA mode */
  USART_IrDAConfig(USART3, USART_IrDAMode_Normal);

  /* Enable the USART3 IrDA mode */
  USART_IrDACmd(USART3, ENABLE);

  while (1)
  {
    /* Wait until a byte is received */
    while(USART_GetFlagStatus(USART3, USART_FLAG_RDNE) == RESET)
    {
    }
    /* Read the received byte */
    ReceivedData = USART_ReceiveData(USART3);
    
    if(ReceivedData == 0xAA)
    {
      AT32_LEDn_ON(LED2);
      AT32_LEDn_ON(LED3);
      AT32_LEDn_ON(LED4);
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
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOC | RCC_APB2PERIPH_AFIO, ENABLE);
  
  /* Enable USART3 clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_USART3, ENABLE);
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure;

  /* Enable the USART3 Pins Partial Software Remapping */
  GPIO_PinsRemapConfig(GPIO_PartialRemap_USART3, ENABLE);
  
  /* Configure USART3USART3 Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_10;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  /* Configure USART3 Rx as input floating */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
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


