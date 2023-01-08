#ifndef _ITEMS__H_
#define _ITEMS__H_

#include "at32f403a_407.h"

/*
    SDK->Items->GPIO(Exist_GPIO)->BASE->
                                         \\
                                          -->[XXX]->MODE
                                         //
                    C(Lib)->Caven->API->
*/

#define DEBUG_OUT   1           //Debug 通道(目前是串口1)->MODE

                                                    /*  基本外设就能实现的功能    */
//#define Exist_SYS_TIME
//#define Exist_PWM
//#define Exist_ENCODE

//#define Exist_KEY
//#define Exist_LED
//#define Exist_BZZ

//#define Exist_ADC
//#define Exist_DAC

//#define Exist_UART
//#define Exist_IIC
//#define Exist_SPI
//#define Exist_CAN

//#define Exist_FLASH
                                                    /*  只需要加上逻辑才能的功能    */
//#define Exist_LCD
//#define Exist_OLED    //一般这两个是二选一（占用的都是SPI）

//#define Exist_38
//#define Exist_595

//#define Exist_Ultrasonic
//#define Exist_FindLine

//#define Exist_Voice

//#define Exist_Motor
//#define Exist_Steering_Engine

//#define Exist_MLX90614


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

#ifdef Exist_Steering_Engine
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
