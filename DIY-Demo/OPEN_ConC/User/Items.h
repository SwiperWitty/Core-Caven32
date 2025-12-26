#ifndef _ITEMS__H_
#define _ITEMS__H_
#if defined (AT32F415RCT7) || defined (AT32F415CBT7)
#include "at32f415.h"
// 软件配置的 基于内部48Mhz晶振  (48 / 6 / 2) * 36      APB 144M、APB1 72M、APB2 72M、（TIM All 144M）（ADC All 18M）（UART All 18M）
#include "at32f415_clock.h"
#include "stdio.h"
#endif
#if defined (STM32F10X_MD) || defined (STM32F10X_HD)
#include "stm32f10x.h"
// 软件配置的 基于xx晶振  
#include "system_stm32f10x.h"
#include "stdio.h"
#endif
/*
 *          SDK->Items
 *                      \\
 *                      -->Base     -->Mode
 *                      //          //
 *  C(Lib)->Caven_Type->        API
 *  2.0     2023.11.16
*/

/*  基本外设就能实现的功能     */

#define OPEN_NULL   0x00    // 不存在设备
#define OPEN_0000   0x00    // 默认MAP
#define OPEN_0001   0x01    // 存在设备0
#define OPEN_0010   0x02    // 存在设备1
#define OPEN_0011   0x03    // 存在设备1、0
#define OPEN_0100   0x04	// ...
#define OPEN_0101   0x05
#define OPEN_0110   0x06
#define OPEN_0111   0x07
#define OPEN_1000   0x08
#define OPEN_1001   0x09
#define OPEN_1010   0x0a
#define OPEN_1011   0x0b
#define OPEN_1100   0x0c
#define OPEN_1101   0x0d
#define OPEN_1110   0x0e
#define OPEN_1111   0x0f

#define OPEN_00001  0x01
#define OPEN_00010  0x02
#define OPEN_00011  0x03
#define OPEN_00100  0x04
#define OPEN_00101  0x05
#define OPEN_00110  0x06
#define OPEN_00111  0x07
#define OPEN_01000  0x08
#define OPEN_01001  0x09
#define OPEN_01010  0x0a
#define OPEN_01011  0x0b
#define OPEN_01100  0x0c
#define OPEN_01101  0x0d
#define OPEN_01110  0x0e
#define OPEN_01111  0x0f
#define OPEN_10000  0x10
#define OPEN_11110  0x1E
#define OPEN_11111  0x1f

#define DEBUG_CH	3   		// Debug 通道(Caved 3.14是串口3)

#ifndef Exist_SYS_TIME
    #define Exist_SYS_TIME  OPEN_0001 // 一定存在
#endif
#define Exist_PWM       OPEN_NULL
#define Exist_CAPTURE   OPEN_NULL

#define Exist_BUTTON    OPEN_NULL
#define Exist_LED       OPEN_NULL
#define Exist_BZZ       OPEN_NULL

#define Exist_ADC		OPEN_NULL
#define Exist_DAC       OPEN_NULL

#define Exist_UART      OPEN_11110
#define Exist_IIC       OPEN_NULL
#define Exist_SPI       OPEN_NULL
#define Exist_USB       OPEN_0001
#define Exist_CAN       OPEN_NULL

#define Exist_FLASH     OPEN_0001

#define GUI_LVGL        OPEN_NULL

/***    需要加上逻辑才能的功能     ***/
#define Exist_LCD	            OPEN_NULL
#define Exist_OLED              OPEN_NULL

#define Exist_HC138             OPEN_NULL
#define Exist_HC595             OPEN_NULL
#define Exist_DS18B20           OPEN_NULL
#define Exist_MLX90614          OPEN_NULL
#define Exist_RTC8564           OPEN_NULL   // 时钟

#define Exist_Ultrasonic        OPEN_NULL   // 超声波测距

#define Exist_Voice             OPEN_NULL   // 语音播报(MP3)

#define Exist_Motor_Engine      OPEN_NULL   // 电机
#define Exist_Steering_Engine   OPEN_NULL   // 舵机
#define Exist_STEP_Motor        OPEN_NULL
#define Exist_Motor_BYJ         OPEN_NULL


/****   进一步的逻辑关系    ****/
#if Exist_UART
    #define UART1_REMAP OPEN_0000		// OPEN_0000:PA9 10,OPEN_0001:PB6 7
    #define UART2_REMAP OPEN_0000		// OPEN_0000:PA2 3
    #define UART3_REMAP OPEN_0001       // OPEN_0000:PB10 11,OPEN_0001:PC10 11
    #define UART4_REMAP OPEN_0000		// OPEN_0000:PC10 11
#endif
#if Exist_USB
    #define USB_MODE	OPEN_0000		// OPEN_0000:HID OPEN_0001:CDC OPEN_0010:HID+KB
#endif
#if Exist_CAPTURE
    #define TIM1_REMAP  OPEN_0000
    #define TIM2_REMAP  OPEN_0001       // OPEN_0000:PA0-3,OPEN_0001:PA15、PB3、PB10、PB11
    #define TIM3_REMAP  OPEN_0000        
    #define TIM4_REMAP  OPEN_0000
#endif

#if Exist_LCD
    #if Exist_SPI == OPEN_NULL
        #undef Exist_SPI
        #define Exist_SPI	OPEN_0100
    #endif
#endif

#if Exist_OLED || Exist_MLX90614 || Exist_RTC8564
    #if Exist_IIC == OPEN_NULL
        #undef Exist_IIC
        #define Exist_IIC   OPEN_0001
    #endif
#endif

#if Exist_Voice
    #if Exist_UART == OPEN_NULL
        #undef Exist_UART
        #define Exist_UART  OPEN_0100
    #endif
#endif

#if Exist_Motor_Engine          // 电机基于TIM8-PWM
    #if Exist_PWM == OPEN_NULL
        #undef Exist_PWM
        #define Exist_PWM   OPEN_0001
    #endif
#endif

#if Exist_Steering_Engine       // 舵机基于TIM4-PWM
    #if Exist_PWM == OPEN_NULL
        #undef Exist_PWM
        #define Exist_PWM   OPEN_0001
    #endif
#endif

/*****  冲突      *****/

#if Exist_LCD
    #if Exist_OLED
        #warning (LCD And OLED Have A Clash !!!)
        #undef Exist_OLED
        #define Exist_OLED  OPEN_0000
    #endif
#endif

/*  MCU指令   */

#define MCU_SYS_FREQ    SystemCoreClock // 刚启动是xMhz，经过配置文件之后就是xxxMhz（system_clock_config()之后）

#ifndef NOP
	#define NOP()    __NOP()
#endif

#ifndef SYS_RESET
	#define SYS_RESET() NVIC_SystemReset()
#endif

// boot
typedef void (*pFunction)(void); /* 跳转函数类型声明 */
#define GO_TO_APP(addr) do{     \
	pFunction jump_to_app;						\
	nvic_irq_disable(SysTick_IRQn);			\
	__NVIC_ClearPendingIRQ(SysTick_IRQn);	\
	jump_to_app = (pFunction)*(uint32_t*)(addr + 4);	\
	__set_MSP(*(uint32_t*)addr);			\
	jump_to_app();							\
}while(0);

#define NVIC_VECTOR_SET(addr)	nvic_vector_table_set(NVIC_VECTTAB_FLASH,addr)

#define IO_H_REG    scr
#define IO_L_REG    clr

#ifdef GPIO_PINS_0
	#define GPIO_Pin_0  GPIO_PINS_0
	#define GPIO_Pin_1  GPIO_PINS_1
	#define GPIO_Pin_2  GPIO_PINS_2
	#define GPIO_Pin_3  GPIO_PINS_3
	#define GPIO_Pin_4  GPIO_PINS_4
	#define GPIO_Pin_5  GPIO_PINS_5
	#define GPIO_Pin_6  GPIO_PINS_6
	#define GPIO_Pin_7  GPIO_PINS_7
	#define GPIO_Pin_8  GPIO_PINS_8
	#define GPIO_Pin_9  GPIO_PINS_9
	#define GPIO_Pin_10 GPIO_PINS_10
	#define GPIO_Pin_11 GPIO_PINS_11
	#define GPIO_Pin_12 GPIO_PINS_12
	#define GPIO_Pin_13 GPIO_PINS_13
	#define GPIO_Pin_14 GPIO_PINS_14
	#define GPIO_Pin_15 GPIO_PINS_15
#endif
#ifdef GPIO_PIN_0
    #define GPIO_Pin_0  GPIO_PIN_0
    #define GPIO_Pin_1  GPIO_PIN_1
    #define GPIO_Pin_2  GPIO_PIN_2
    #define GPIO_Pin_3  GPIO_PIN_3
    #define GPIO_Pin_4  GPIO_PIN_4
    #define GPIO_Pin_5  GPIO_PIN_5
    #define GPIO_Pin_6  GPIO_PIN_6
    #define GPIO_Pin_7  GPIO_PIN_7
    #define GPIO_Pin_8  GPIO_PIN_8
    #define GPIO_Pin_9  GPIO_PIN_9
    #define GPIO_Pin_10 GPIO_PIN_10
    #define GPIO_Pin_11 GPIO_PIN_11
    #define GPIO_Pin_12 GPIO_PIN_12
    #define GPIO_Pin_13 GPIO_PIN_13
    #define GPIO_Pin_14 GPIO_PIN_14
    #define GPIO_Pin_15 GPIO_PIN_15
#endif

#ifndef MAX
    #define MAX(a,b)    ((a)>(b))?(a):(b)               // 比较函数返回最大值，防止过小
    #define MIN(a,b)    ((a)<(b))?(a):(b)               // 比较函数返回最小值，防止过大
#endif

#endif

