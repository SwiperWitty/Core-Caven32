/**
  ******************************************************************************
  * File   : USART/MultiProcessor/main.c 
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

/** @addtogroup USART_MultiProcessor
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USART_InitType USART_InitStructure;

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void EXTI_Wakeup_Key_Init(void);

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

  /* Initialize Leds, Wakeup and Key Buttons  */ 
  AT32_Board_Init();
  Delay_init();
  
  EXTI_Wakeup_Key_Init();
  
  /* USART2 and USART3 configuration -------------------------------------------*/
  /* USART2 and USART3 configured as follow:
        - BaudRate = 9600 baud  
        - Word Length = 9 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_StructInit(&USART_InitStructure);
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_9b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  /* Configure USART2 */
  USART_Init(USART2, &USART_InitStructure);
  /* Configure USART3 */
  USART_Init(USART3, &USART_InitStructure);
  
  /* Enable the USART2 */
  USART_Cmd(USART2, ENABLE);
  /* Enable the USART3 */
  USART_Cmd(USART3, ENABLE);

  /* Set the USART2 Address */
  USART_SetAddress(USART2, 0x1);
  /* Set the USART3 Address */
  USART_SetAddress(USART3, 0x2);

  /* Select the USART3 WakeUp Method */
  USART_WakeUpConfig(USART3, USART_WakeUp_AddressMark);
  
  while (1)
  {
    /* Send one byte from USART2 to USART3 */
    USART_SendData(USART2, 0x33);
    
    /* Wait while USART3 TDE = 0 */
    while(USART_GetFlagStatus(USART3, USART_FLAG_TDE) == RESET)
    {
    }
    /* Wait while USART3 RDNE = 1 */
    if(USART_GetFlagStatus(USART3, USART_FLAG_RDNE) != RESET)
    {
      /* Judge whether Receive Data is 0x33 */
      if(USART_ReceiveData(USART3) == 0x33)
      {
        /* Toggle the LED*/
        AT32_LEDn_Toggle(LED2);
        AT32_LEDn_Toggle(LED3);
        AT32_LEDn_Toggle(LED4);
        Delay_ms(500);
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
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOD | RCC_APB2PERIPH_GPIOC | RCC_APB2PERIPH_AFIO, ENABLE);

  /* Enable USART2 Clock */
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_USART2, ENABLE); 

  /* Enable USART3 Clock */
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

  /* Enable the USART3 Pins Software Remapping */
  GPIO_PinsRemapConfig(GPIO_PartialRemap_USART3, ENABLE);
  
  /* Enable the USART2 Pins Software Remapping */
  GPIO_PinsRemapConfig(GPIO_Remap_USART2, ENABLE);  

  /* Configure USART2 Rx as input floating */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /* Configure USART3 Rx as input floating */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_11;
  GPIO_Init(GPIOC, &GPIO_InitStructure);  
  
  /* Configure USART2 Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_5;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* Configure USART3 Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_10;
  GPIO_Init(GPIOC, &GPIO_InitStructure);  
}

/**
  * @brief  Configures Wakeup and Key Buttons GPIO and EXTI Line.
  * @param  None
  * @retval None
  */
void EXTI_Wakeup_Key_Init(void)
{
  GPIO_InitType GPIO_InitStructure;
  EXTI_InitType EXTI_InitStructure;
  NVIC_InitType NVIC_InitStructure;

  /* Enable the BUTTON Clock */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA | RCC_APB2PERIPH_GPIOC | RCC_APB2PERIPH_AFIO, ENABLE);

  /* Configure Button pin as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_0;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* Configure Button pin as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_13;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Connect Button EXTI Line to Button GPIO Pin */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinsSource0);

  /* Connect Button EXTI Line to Button GPIO Pin */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinsSource13);
  
  /* Configure Wake-up EXTI line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
  EXTI_InitStructure.EXTI_LineEnable = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
 
  /* Configure key EXTI line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line13;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
  EXTI_InitStructure.EXTI_LineEnable = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set Button EXTI Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 
  
  /* Enable and set Button EXTI Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 
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

