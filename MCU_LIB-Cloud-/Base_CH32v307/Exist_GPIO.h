#ifndef __Exist_GPIO__H_
#define __Exist_GPIO__H_

#include "Items.h"

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

#ifdef Exist_LCD
    #define LCD_DC_Clr() GPIOA->BCR = GPIO_Pin_10    //DC_GPIO
    #define LCD_DC_Set() GPIOA->BSHR = GPIO_Pin_10   //PA10
//    #define LCD_RES_Clr()                                     //RES               //Caven 使用硬件复位
//    #define LCD_RES_Set()
#endif

#ifdef Exist_OLED

    #define OLED_DC_Clr() GPIOA->BCR = GPIO_Pin_10    //DC_GPIO
    #define OLED_DC_Set() GPIOA->BSHR = GPIO_Pin_10   //PA10

//    #define LCD_RES_Clr()                                     //RES               //Caven 使用硬件复位
//    #define LCD_RES_Set()
#endif

#ifdef Exist_LED
    #define LED_Clr() GPIOB->BCR = GPIO_Pin_11    //LED
    #define LED_Set() GPIOB->BSHR = GPIO_Pin_11   //PB011
    #define LEDR_Clr() GPIOA->BCR = GPIO_Pin_8    //LED_Red
    #define LEDR_Set() GPIOA->BSHR = GPIO_Pin_8   //PA008
#endif
#ifdef Exist_BZZ
    #define BZZ_Clr() GPIOB->BCR = GPIO_Pin_5    //BZZ
    #define BZZ_Set() GPIOB->BSHR = GPIO_Pin_5   //PB05
#endif

#ifdef Exist_KEY
    #define KEY_IN() GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)     //KEY,读
#endif

    #define Reset_L() GPIOB->BCR = GPIO_Pin_14
    #define Reset_H() GPIOB->BSHR = GPIO_Pin_14


/*  Init-Function    */

void LCD_GPIO_Init(int SET);

void LED_GPIO_Init(int SET);
void BZZ_GPIO_Init(int SET);

void KEY_GPIO_Init(int SET);
void GXIC_GPIO_Init(int SET);

#endif
