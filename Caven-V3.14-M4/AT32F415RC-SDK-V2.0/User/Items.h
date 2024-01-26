#ifndef _ITEMS__H_
#define _ITEMS__H_

#include "at32f415.h"
// 软件配置的 基于内部48Mhz晶振  (48 / 6 / 2) * 36      APB 144M、APB1 72M、APB2 144M、（TIM All 144M）（ADC All 18M）（UART All 18M）
#include "at32f415_clock.h"
// print的库文件 
#include "stdio.h"

/*
 *          SDK->Items
 *                      \\
 *                      -->Base     -->Mode
 *                      //          //
 *  C(Lib)->Caven_Type->        API
 *  2.0     2023.11.16
*/

/*  基本外设就能实现的功能     */

#define OPEN_0001   0x01    // 设备0开启
#define OPEN_0010   0x02    // 设备1开启
#define OPEN_0011   0x03    // 设备1、0开启
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
#define OPEN_10000  0x10
#define OPEN_11110  0x1E
#define OPEN_11111  0x1f

#ifndef Exist_SYS_TIME
    #define Exist_SYS_TIME      // 一定存在
#endif
#define Exist_PWM
//#define Exist_ENCODE

#define Exist_BUTTON
#define Exist_LED       OPEN_0001  // LED0
#define Exist_BZZ
#define Exist_CustomIO

// #define Exist_ADC       OPEN_0001
//#define Exist_DAC

#define Exist_UART      OPEN_11110  // 串口1、2、3
//#define Exist_IIC
//#define Exist_SPI     OPEN_0100   // SPI 2
//#define Exist_USB
//#define Exist_CAN

//#define Exist_FLASH

/***    需要加上逻辑才能的功能     ***/
#define Exist_LCD
//#define Exist_OLED

//#define Exist_HC138
//#define Exist_HC595           OPEN_0001
//#define Exist_DS18B20

//#define Exist_Ultrasonic                  //超声波测距
//#define Exist_FindLine                    //循迹

//#define Exist_Voice                       //语音播报(MP3)

//#define Exist_Motor           OPEN_1111   // 电机
//#define Exist_Steering_Engine OPEN_1111   // 舵机
//#define Exist_STEP_Motor

//#define Exist_MLX90614                    //红外测温
//#define Exist_RTC8564                     //时钟


/****   进一步的逻辑关系    ****/
#ifdef Exist_LCD
    #ifndef Exist_SPI
        #define Exist_SPI
    #endif
#endif

#ifdef Exist_OLED
    #ifndef Exist_SPI
        #define Exist_SPI
    #endif
#endif

#ifdef Exist_Voice
    #ifndef Exist_UART
        #define Exist_UART
    #endif
#endif

#ifdef Exist_Motor
    #ifndef Exist_PWM
        #define Exist_PWM
    #endif
    #ifndef Exist_ENCODE
        #define Exist_ENCODE
    #endif
#endif

#ifdef Exist_Steering_Engine        //舵机基于TIM4-PWM
    #ifndef Exist_PWM
        #define Exist_PWM
    #endif
#endif

#ifdef Exist_MLX90614
    #ifndef Exist_IIC
        #define Exist_IIC
    #endif
#endif

/*****  冲突      *****/
#if DEBUG_OUT == 1
    #ifdef Exist_USB
        #warning (UART1 And USB Clash !!!)
    #endif
#endif

#ifdef Exist_LCD
    #ifdef Exist_OLED
        #warning (LCD And OLED Have A Clash !!!)
        #undef Exist_OLED
    #endif
#endif

#ifdef Exist_ADC
    #ifdef Exist_DAC
        #warning (ADC And DAC Have A Clash !!!)
        #undef Exist_DAC
    #endif
#endif


/*  MCU指令   */

#define MCU_SYS_FREQ    SystemCoreClock // 刚启动是xM，经过配置文件之后就是144（system_clock_config()之后）

#ifndef NOP
	#define NOP()    __NOP()
#endif

#ifndef SYS_RESET
	#define SYS_RESET() NVIC_SystemReset()
#endif

// boot
#define GO_TO_APP() do{     \
                            \
}while(0);

#define REG_IO_H    scr
#define REG_IO_L    clr

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


#endif
