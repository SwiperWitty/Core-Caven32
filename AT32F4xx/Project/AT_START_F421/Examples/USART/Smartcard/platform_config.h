/**
  ******************************************************************************
  * File   : USART/Smartcard/platform_config.h 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : specific configuration file.
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PLATFORM_CONFIG_H
#define __PLATFORM_CONFIG_H

/* Includes ------------------------------------------------------------------*/

/* Smartcard Inteface USART pins */
#define SC_USART                 USART1
#define SC_USART_GPIO            GPIOA
#define SC_USART_CLK             RCC_APB2PERIPH_USART1
#define SC_USART_GPIO_CLK        RCC_AHBPERIPH_GPIOA
#define SC_USART_TxPin           GPIO_Pins_9
#define SC_USART_ClkPin          GPIO_Pins_8
#define SC_USART_IRQn            USART1_IRQn 
#define SC_USART_IRQHandler      USART1_IRQHandler    
/* Smartcard Inteface GPIO pins */
#define SC_3_5V                  GPIO_Pins_0  /* GPIOB Pin 0 */
#define SC_RESET                 GPIO_Pins_11 /* GPIOB Pin 11 */
#define SC_CMDVCC                GPIO_Pins_6  /* GPIOB Pin 6  */
#define SC_OFF                   GPIO_Pins_7  /* GPIOB Pin 7 */ 
#define GPIO_3_5V                GPIOB
#define GPIO_RESET               GPIOB
#define GPIO_CMDVCC              GPIOB
#define GPIO_OFF                 GPIOB
#define RCC_AHBPeriph_3_5V       RCC_AHBPERIPH_GPIOB
#define RCC_AHBPeriph_RESET      RCC_AHBPERIPH_GPIOB
#define RCC_AHBPeriph_CMDVCC     RCC_AHBPERIPH_GPIOB
#define RCC_AHBPeriph_OFF        RCC_AHBPERIPH_GPIOB
#define SC_EXTI                  EXTI_Line7
#define SC_PortSource            GPIO_PortSourceGPIOC
#define SC_PinSource             GPIO_PinsSource7
#define SC_EXTI_IRQ              EXTI15_4_IRQn


/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __PLATFORM_CONFIG_H */


