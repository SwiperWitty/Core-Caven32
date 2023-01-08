#ifndef __Exist_GPIO__H_
#define __Exist_GPIO__H_

#ifdef DEFAULT
#include "items.h"              //默认功能
#endif

#ifndef DEFAULT
#include "User_items.h"         //自行设置功能，一般出现在本地文件的User中
#endif

/****************/

/*
                GPIO_Init(Exist_GPIO)->
    SDK->Items->                       BASE->
                Peripheral(UART、ADC)->      \\
                                              -->[XXX]->MODE
                                             //
                        C(Lib)->Caven->API->
*/

/*
        本文件在上述关系图中属于 【GPIO_Init】
         这里存放的是【只使用GPIO外设】功能的初始化（KEY、LED、BZZ、超声波等）。
         其他功能的初始化（例如UART、IIC），除了先初始化【GPIO外设】还要【功能外设】，这种功能的初始化GPIO并不在本文件范围内。
         IIC有软件版本和硬件版本，所以它的GPIO跟本文件无关。
         但是LCD的驱动由SPI协议和DC（IO）接口组成，那么这个IO接口由本文件提供。

                                                                     ————202207.14
 */

#define READ_Config		0
#define WRITE_Config	1

//User io
#define User_hot_off() GPIOB->clr = GPIO_PINS_6  
#define User_hot_on() GPIOB->scr = GPIO_PINS_6   
#define User_09V() GPIOA->clr = GPIO_PINS_1      
#define User_12V() GPIOA->scr = GPIO_PINS_1   

#ifdef Exist_LCD
    #define LCD_DC_L() GPIOA->clr = GPIO_PINS_10    //DC_GPIO
    #define LCD_DC_H() GPIOA->scr = GPIO_PINS_10   //PA10
//    #define LCD_RES_L()                                     //RES               //Caven 使用硬件复位
//    #define LCD_RES_H()
#endif

#ifdef Exist_OLED

    #define OLED_DC_L() GPIOA->clr = GPIO_PINS_10    //DC_GPIO
    #define OLED_DC_H() GPIOA->scr = GPIO_PINS_10   //PA10

//    #define LCD_RES_L()                                     //RES               //Caven 使用硬件复位
//    #define LCD_RES_H()
#endif

#ifdef Exist_LED
    #define LED_L() GPIOA->clr = GPIO_PINS_4       //LED
    #define LED_H() GPIOA->scr = GPIO_PINS_4      //PA04
    #define LEDR_L() GPIOC->clr = GPIO_PINS_2      //PA04
    #define LEDR_H() GPIOC->scr = GPIO_PINS_2      //PA04
#endif

#ifdef Exist_BZZ
    #define BZZ_L() GPIOB->clr = GPIO_PINS_5    //BZZ
    #define BZZ_H() GPIOB->scr = GPIO_PINS_5   //PB05
#endif

#ifdef Exist_KEY
    #define KEY_R() gpio_input_data_bit_read(GPIOC,GPIO_PINS_13)     //KEY,读
#endif

#ifdef Exist_HC595
    #define LATCH_CLOCK     GPIO_PINS_1         //门阀时钟
    #define SHIFT_CLOCK     GPIO_PINS_2         //移动时钟
    #define HC595_Data      GPIO_PINS_3         //数据
    
    #define LATCH_CLOCK_L() GPIOC->clr = LATCH_CLOCK
    #define LATCH_CLOCK_H() GPIOC->scr = LATCH_CLOCK
    #define SHIFT_CLOCK_L() GPIOC->clr = SHIFT_CLOCK
    #define SHIFT_CLOCK_H() GPIOC->scr = SHIFT_CLOCK
    #define HC595_Data_L() GPIOC->clr = HC595_Data
    #define HC595_Data_H() GPIOC->scr = HC595_Data
    
#endif

#ifdef Exist_HC138
    #define HC595_D1    GPIO_PINS_1
    #define HC595_D2    GPIO_PINS_2
    #define HC595_D3    GPIO_PINS_3         //数据
    
    #define HC138_D1_H() GPIOC->scr = HC595_D1
    #define HC138_D1_L() GPIOC->clr = HC595_D1
    #define HC138_D2_H() GPIOC->scr = HC595_D2
    #define HC138_D2_L() GPIOC->clr = HC595_D2
    #define HC138_D3_H() GPIOC->scr = HC595_D3
    #define HC138_D3_L() GPIOC->clr = HC595_D3
    
#endif

#ifdef Exist_DS18B20
    #define DS18B20_IO    GPIO_PINS_1
    #define DS18B20_Clock    GPIOA

    #define DS18B20_IO_H() DS18B20_Clock->scr = DS18B20_IO
    #define DS18B20_IO_L() DS18B20_Clock->clr = DS18B20_IO

    #define DS18B20_IO_R() gpio_input_data_bit_read(DS18B20_Clock,DS18B20_IO)     // 读
#endif


/*  Init-Function    */

void LCD_GPIO_Init(int Set);

void LED_GPIO_Init(int Set);
void BZZ_GPIO_Init(int Set);

void HC138_GPIO_Init(int Set);
void HC595_GPIO_Init(int Set);

void DS18B20_GPIO_Init(int Set);

void KEY_GPIO_Init(int Set);

void User_GPIO_Init(int Set);	//!!!!

/*  other    */
void DS18B20_IO_Config(int Set);


#endif
