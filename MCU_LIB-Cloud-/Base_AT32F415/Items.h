#ifndef _ITEMS__H_
#define _ITEMS__H_

#include "at32f415.h"
#include "at32f415_clock.h"     //软件配置的 基于内部48Mhz晶振  (48 / 6 / 2) * 36      APB 144M、APB1 72M、APB2 144M、（TIM All 144M）（ADC All 18M）（UART All 18M）

/*
    SDK->Items->GPIO(Exist_GPIO)->BASE->
                                         \\
                                          -->[XXX]->MODE
                                         //
                    C(Lib)->Caven->API->

*/
#define TURE   TRUE
#define DEBUG_OUT   1           //Debug 通道(目前是串口1)->MODE
#define MCU_SYS_Freq SystemCoreClock        //刚启动是8M，经过配置文件之后就是144（system_clock_config()之后）
#define	NOP()		__nop()
                                                    /*  基本外设就能实现的功能    */
#define Exist_SYS_TIME
//#define Exist_PWM
//#define Exist_ENCODE

//#define Exist_KEY
#define Exist_LED
//#define Exist_BZZ

#define Exist_ADC
//#define Exist_DAC

#define Exist_UART
#define Exist_IIC
#define Exist_SPI
//#define Exist_USB
//#define Exist_CAN

#define Exist_FLASH
    
                                                    /*  只需要加上逻辑才能的功能    */
//#define Exist_LCD
//#define Exist_OLED            //一般这两个是二选一（占用的都是SPI）

//#define Exist_HC138
//#define Exist_HC595
//#define Exist_DS18B20

//#define Exist_Ultrasonic          //超声波测距
//#define Exist_FindLine            //循迹

//#define Exist_Voice               //语音播报(MP3)

//#define Exist_Motor               //电机
//#define Exist_Steering_Engine       //舵机

//#define Exist_MLX90614            //红外测温


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

#endif
