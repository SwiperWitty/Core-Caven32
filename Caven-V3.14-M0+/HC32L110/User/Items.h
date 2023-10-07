#ifndef _ITEMS__H_
#define _ITEMS__H_

#include "hc32l110.h"
#include "ddl.h"
#include "system_hc32l110.h"	//软件配置的 40Mhz
#include "stdio.h"

/*
    SDK->Items->GPIO(Exist_GPIO)->BASE->
                                         \\
                                          -->[XXX]->MODE
                                         //
                    C(Lib)->Caven->API->

*/
#define TURE   TRUE
#define DEBUG_OUT   1           //Debug 通道(Caved 3.14是串口3)->MODE
#define MCU_SYS_Freq SystemCoreClock        //刚启动是xM，经过配置文件之后就是144（system_clock_config()之后）


                                             /*  基本外设就能实现的功能    */
#ifndef Exist_SYS_TIME
    #define Exist_SYS_TIME      //一定存在
#endif

#define Exist_KEY
#define Exist_LED
//#define Exist_BZZ

//#define Exist_ADC
//#define Exist_DAC

//#define Exist_UART

#define Exist_FLASH


#define	NOP()		__nop()

#define Reg_IO_H    scr
#define Reg_IO_L    clr

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
