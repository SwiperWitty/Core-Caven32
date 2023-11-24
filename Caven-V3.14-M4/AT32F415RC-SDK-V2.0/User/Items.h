#ifndef _ITEMS__H_
#define _ITEMS__H_

#include "at32f415.h"
#include "at32f415_clock.h"     //软件配置的 基于内部48Mhz晶振  (48 / 6 / 2) * 36      APB 144M、APB1 72M、APB2 144M、（TIM All 144M）（ADC All 18M）（UART All 18M）
#include "stdio.h"

/*
    SDK->Items->GPIO(Exist_GPIO)->BASE->
                                         \\
                                          -->[XXX]->MODE
                                         //
                    C(Lib)->Caven->API->

*/
#define DEBUG_OUT   3           //Debug 通道(Caved 3.14是串口3)->MODE
#define MCU_SYS_Freq SystemCoreClock        //刚启动是xM，经过配置文件之后就是144（system_clock_config()之后）


/*  基本外设就能实现的功能    */
#ifndef Exist_SYS_TIME
    #define Exist_SYS_TIME      //一定存在
#endif
//#define Exist_PWM
//#define Exist_ENCODE

#define Exist_KEY
#define Exist_LED
//#define Exist_BZZ

//#define Exist_ADC
//#define Exist_DAC

#define Exist_UART
#define Exist_IIC
//#define Exist_SPI
//#define Exist_USB
//#define Exist_CAN

#define Exist_FLASH

                                                    /*  只需要加上逻辑才能的功能    */
#define Exist_LCD
//#define Exist_OLED            //一般这两个是二选一（占用的都是SPI）

//#define Exist_HC138
//#define Exist_HC595
//#define Exist_DS18B20

//#define Exist_Ultrasonic          //超声波测距
//#define Exist_FindLine            //循迹

//#define Exist_Voice               //语音播报(MP3)

//#define Exist_Motor               //电机
//#define Exist_Steering_Engine     //舵机
#define Exist_STEP_Motor

//#define Exist_MLX90614            //红外测温
#define Exist_RTC8564               //时钟


/*  mcu 指令  */

#define GO_TO_APP() do{     \
                            \
}while(0);

#define Reg_IO_H    scr
#define Reg_IO_L    clr

#ifndef NOP
#define NOP()       __nop()
#endif

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

/*  进一步的逻辑关系    */
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

/*  冲突  */
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

#endif
