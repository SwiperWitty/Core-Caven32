#ifndef _BASE_H__
#define _BASE_H__

#include "items.h"

#include "Exist_GPIO.h"     //LED、BZZ、KEY...

#ifdef Exist_UART
#include "uart.h"
#endif

#ifdef Exist_SPI
#include "spi.h"
#endif


#ifdef Exist_IIC
#include "Base_IIC.h"
#endif


#ifdef Exist_ADC
#include "adc.h"
#endif


#ifdef Exist_SYS_TIME
#include "sys_time.h"
#endif

#ifdef Exist_PWM
#include "PWM.h"
#endif


/*
    SDK->Items->GPIO(Exist_GPIO)->BASE->
                                         \\
                                          -->[XXX]->MODE
                                         //
                    C(Lib)->Caven->API->
*/
/*
    底层（Base）中有不少东西让人迷惑，他们大致分布在 system_xx32xxx.c 以及 xx32xxxx_it.h 和 xx32xxxx.h
    如果查阅这里，问题都没有解决，那么这就是一个不该由你解决的问题。
  Base层只能和 MCU SDK有关
*/


#endif
