/**
  **************************************************************************
  * File   : at32_board.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Header file for AT-START board
  *          1. Set of firmware functions to manage Leds, push-button and COM ports.
  *          2. initialize Delay Function and USB
  */

#ifndef __AT32_BOARD_H
#define __AT32_BOARD_H	 
#include <at32f4xx.h>
#include "at32f4xx_gpio.h"
#include "at32f4xx_rcc.h"
/*
* This header include define support list:
* 	1. AT-START-F403 V1.2 Board
* 	2. AT-START-F413 V1.0 Board
* 	3. AT-START-F415 V1.0 Board
* 	4. AT-START-F403A V1.0 Board
* 	5. AT-START-F407 V1.0 Board
* 	6. AT-START-F421 V1.0 Board
* if define AT_START_F403_V1_2, the header file support AT-START-F403 V1.2 Board
* if define AT_START_F413_V1_0, the header file support AT-START-F413 V1.0 Board
* if define AT_START_F415_V1_0, the header file support AT-START-F415 V1.0 Board
* if define AT_START_F403A_V1_0, the header file support AT-START-F403A V1.0 Board
* if define AT_START_F407_V1_0, the header file support AT-START-F407 V1.0 Board
* if define AT_START_F421_V1_0, the header file support AT-START-F421 V1.0 Board
*/
#if !defined (AT_START_F403_V1_2) && !defined (AT_START_F413_V1_0) && !defined (AT_START_F415_V1_0) && \
    !defined (AT_START_F403A_V1_0)&& !defined (AT_START_F407_V1_0) && !defined (AT_START_F421_V1_0)
#error "Please select first the board AT-START device used in your application (in at32_board.h file)"
#endif

/*define usb pin*/
#define USB_DP_PIN          GPIO_Pins_12
#define USB_DM_PIN          GPIO_Pins_11

#define USB_GPIO            GPIOA
#if defined (AT32F421xx)
#define USB_GPIO_RCC_CLK    RCC_AHBPERIPH_GPIOA
#else
#define USB_GPIO_RCC_CLK    RCC_APB2PERIPH_GPIOA
#endif

/*AT_START LED*/
typedef enum
{
  LED1 = 0,
  LED2,
  LED3,
  LED4
}LED_Type;    

#define LED_NUM     4

#if defined (AT_START_F413_V1_0) || defined (AT_START_F415_V1_0)
/*have no LED1*/
#define LED1_PIN                0
#define LED1_GPIO               NULL
#define LED1_GPIO_RCC_CLK       0

#define LED2_PIN                GPIO_Pins_2
#define LED2_GPIO               GPIOC
#define LED2_GPIO_RCC_CLK       RCC_APB2PERIPH_GPIOC

#define LED3_PIN                GPIO_Pins_3
#define LED3_GPIO               GPIOC
#define LED3_GPIO_RCC_CLK       RCC_APB2PERIPH_GPIOC

#define LED4_PIN                GPIO_Pins_5
#define LED4_GPIO               GPIOC
#define LED4_GPIO_RCC_CLK       RCC_APB2PERIPH_GPIOC

#elif defined (AT_START_F403_V1_2) || defined (AT_START_F403A_V1_0) || \
    defined (AT_START_F407_V1_0)
/*have no LED1*/
#define LED1_PIN                0
#define LED1_GPIO               NULL
#define LED1_GPIO_RCC_CLK       0

#define LED2_PIN                GPIO_Pins_13
#define LED2_GPIO               GPIOD
#define LED2_GPIO_RCC_CLK       RCC_APB2PERIPH_GPIOD

#define LED3_PIN                GPIO_Pins_14
#define LED3_GPIO               GPIOD
#define LED3_GPIO_RCC_CLK       RCC_APB2PERIPH_GPIOD

#define LED4_PIN                GPIO_Pins_15
#define LED4_GPIO               GPIOD
#define LED4_GPIO_RCC_CLK       RCC_APB2PERIPH_GPIOD

#elif defined (AT_START_F421_V1_0)
/*have no LED1*/
#define LED1_PIN                0
#define LED1_GPIO               NULL
#define LED1_GPIO_RCC_CLK       0

#define LED2_PIN                GPIO_Pins_6
#define LED2_GPIO               GPIOF
#define LED2_GPIO_RCC_CLK       RCC_AHBPERIPH_GPIOF

#define LED3_PIN                GPIO_Pins_7
#define LED3_GPIO               GPIOF
#define LED3_GPIO_RCC_CLK       RCC_AHBPERIPH_GPIOF

#define LED4_PIN                GPIO_Pins_11
#define LED4_GPIO               GPIOB
#define LED4_GPIO_RCC_CLK       RCC_AHBPERIPH_GPIOB 
#endif
/*End LED define*/


/*define button*/
typedef enum
{
  BUTTON_WAKEUP = 0,
  BUTTON_USER_KEY,
  BUTTON_KEY1_DOWN,
  BUTTON_KEY2_LEFT,   
  NO_BUTTON    
}BUTTON_Type;

#define BUTTON_NUM  2

#define BUTTON_WAKEUP_PIN              GPIO_Pins_0
#define BUTTON_WAKEUP_GPIO             GPIOA
#if defined (AT32F421xx)
#define BUTTON_WAKEUP_RCC_CLK          RCC_AHBPERIPH_GPIOA
#else
#define BUTTON_WAKEUP_RCC_CLK          RCC_APB2PERIPH_GPIOA
#endif

#define BUTTON_USER_KEY_PIN            GPIO_Pins_13
#define BUTTON_USER_KEY_GPIO           GPIOC
#if defined (AT32F421xx)
#define BUTTON_USER_KEY_RCC_CLK        RCC_AHBPERIPH_GPIOC
#else
#define BUTTON_USER_KEY_RCC_CLK        RCC_APB2PERIPH_GPIOC
#endif

/*end define button*/
#ifdef AT_START_F403_V1_2
/*Audio DAC OUTPUT GPIO Pin*/
#define F403_AUDIO_DAC_OUTPIN      GPIO_Pins_4
#endif

/**************** UART printf ****************/
#define AT32_PRINT_UART                USART1
#define USARTx_IRQn                    USART1_IRQn
#define USARTx_IRQ_Handler             USART1_IRQHandler
#define AT32_PRINT_UART_RCC            RCC_APB2PERIPH_USART1

/*Tx*/
#define AT32_PRINT_UARTTX_PIN          GPIO_Pins_9 
#define AT32_PRINT_UARTTX_GPIO         GPIOA
#if defined (AT32F421xx)
#define AT32_PRINT_UARTTX_GPIO_RCC     RCC_AHBPERIPH_GPIOA
#else
#define AT32_PRINT_UARTTX_GPIO_RCC     RCC_APB2PERIPH_GPIOA
#endif

/*Rx*/
#define AT32_PRINT_UARTRX_PIN          GPIO_Pins_10
#define AT32_PRINT_UARTRX_GPIO         GPIOA
#if defined (AT32F421xx)
#define AT32_PRINT_UARTRX_GPIO_RCC     RCC_AHBPERIPH_GPIOA
#else
#define AT32_PRINT_UARTRX_GPIO_RCC     RCC_APB2PERIPH_GPIOA
#endif

/**************** End UART printf ****************/

void AT32_Board_Init(void);
void AT32_USB_GPIO_init(void);

/*Led Operation function*/
void AT32_LEDn_Init(LED_Type led);
void AT32_LEDn_ON(LED_Type led);
void AT32_LEDn_OFF(LED_Type led);
void AT32_LEDn_Toggle(LED_Type led);

/*Button Operation function*/
void AT32_BUTTON_Init(BUTTON_Type button);
BUTTON_Type AT32_BUTTON_Press(void);
uint8_t AT32_BUTTON_State(BUTTON_Type button);

void UART_Print_Init(uint32_t bound);

/*Delay function*/
void Delay_init(void);
void Delay_us(u32 nus);
void Delay_ms(u16 nms);
void Delay_sec(u16 sec);
#endif

