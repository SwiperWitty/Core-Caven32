/**
  ******************************************************************************
  * File   : ADC/ADC1_DMA/platform_config.h 
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
#define SC_USART                 USART3
#define SC_USART_GPIO            GPIOB
#define SC_USART_CLK             RCC_APB1PERIPH_USART3
#define SC_USART_GPIO_CLK        RCC_APB2PERIPH_GPIOB
#define SC_USART_TxPin           GPIO_Pins_10
#define SC_USART_ClkPin          GPIO_Pins_12
#define SC_USART_IRQn            USART3_IRQn 
#define SC_USART_IRQHandler      USART3_IRQHandler    
/* Smartcard Inteface GPIO pins */
#define SC_3_5V                  GPIO_Pins_0  /* GPIOB Pin 0 */
#define SC_RESET                 GPIO_Pins_11 /* GPIOB Pin 11 */
#define SC_CMDVCC                GPIO_Pins_6  /* GPIOC Pin 6  */
#define SC_OFF                   GPIO_Pins_7  /* GPIOC Pin 7 */ 
#define GPIO_3_5V                GPIOB
#define GPIO_RESET               GPIOB
#define GPIO_CMDVCC              GPIOC
#define GPIO_OFF                 GPIOC
#define RCC_APB2Periph_3_5V      RCC_APB2PERIPH_GPIOB
#define RCC_APB2Periph_RESET     RCC_APB2PERIPH_GPIOB
#define RCC_APB2Periph_CMDVCC    RCC_APB2PERIPH_GPIOC
#define RCC_APB2Periph_OFF       RCC_APB2PERIPH_GPIOC
#define SC_EXTI                  EXTI_Line7
#define SC_PortSource            GPIO_PortSourceGPIOC
#define SC_PinSource             GPIO_PinsSource7
#define SC_EXTI_IRQ              EXTI9_5_IRQn


/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __PLATFORM_CONFIG_H */


