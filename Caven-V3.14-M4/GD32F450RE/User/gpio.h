/*******************************************************************************


       层定义： 底层驱动
       文件名： gpio.h
       版本号： V1.0
       功能描述：
           本文件为GPIO接口的头文件，主要相关操作函数。


        修订记录：
            V1.0    2007-11-2   郭述强  在AT91SAM7X256上实现
            V2.0    2023-07-6   yty     兼容升级

*******************************************************************************/

#ifndef _GPIO_H
#define _GPIO_H

#include "Items.h"

//数字电路控制

/*  A   */
#define VCO_CLK         GPIO_Pins_3             /*gpioG*/
#define VCO_DATA        GPIO_Pins_4             /*gpioG*/
#define PLL_LE          GPIO_Pins_2             /*gpioG*/
#define PLL_CE          GPIO_Pins_15            /*gpioD*/
#define PLL_LOCK        GPIO_Pins_14            /*gpioD*/

/*  B   */
#define VACP            GPIO_Pins_4             /*gpioA*/
#define RF_AMP_EN       GPIO_Pins_5             /*gpioA*/
#define SHUTEN          GPIO_Pins_11            /*gpioD*/
#define AZEN            GPIO_Pins_12            /*gpioD*/
#define nPWREN          GPIO_Pins_13            /*gpioD*/
#define RF1_POWER       GPIO_Pins_12            /*gpioE*/
#define RF2_POWER       GPIO_Pins_13            /*gpioE*/
#define RFID_EN         GPIO_Pins_10            /*gpioH*/
#define chip_rst        GPIO_Pins_11            /*gpioC*/

/*  C   */
#define BEEP            GPIO_Pins_9             /*gpioE*/
#define LED_WORK        GPIO_Pins_7             /*gpioE*/       //GPIO_Pins_7
#define LED1_WORK        GPIO_Pins_0             /*gpioC*/       //GPIO_Pins_7

/*  A   */
//High
#define VCO_CLK_H()     GPIO_BOP(GPIOG) = VCO_CLK       //PG3
#define VCO_DATA_H()    GPIO_BOP(GPIOG) = VCO_DATA      //PG4
#define PLL_LE_H()      GPIO_BOP(GPIOG) = PLL_LE        //PG2
#define PLL_CE_H()      GPIO_BOP(GPIOD) = PLL_CE   
//Low
#define VCO_CLK_L()     GPIO_BC(GPIOG) = VCO_CLK       //PG3
#define VCO_DATA_L()    GPIO_BC(GPIOG) = VCO_DATA      //PG4
#define PLL_LE_L()      GPIO_BC(GPIOG) = PLL_LE        //PG2
#define PLL_CE_L()      GPIO_BC(GPIOD) = PLL_CE   
//in
#define PLL_LOCK_R() gpio_input_bit_get(GPIOD, PLL_LOCK)

/*  B   */
//High
#define VACP_H()        GPIO_BOP(GPIOA) = VACP
#define SHUTEN_H()      GPIO_BOP(GPIOD) = SHUTEN
#define AZEN_H()        GPIO_BOP(GPIOD) = AZEN  

#define RF_AMP_EN_H()   GPIO_BOP(GPIOA) = RF_AMP_EN
#define nPWREN_H()      GPIO_BOP(GPIOD) = nPWREN
#define RF1_POWER_H()   GPIO_BOP(GPIOE) = RF1_POWER
#define RF2_POWER_H()   GPIO_BOP(GPIOE) = RF2_POWER
#define RFID_EN_H()     GPIO_BOP(GPIOH) = RFID_EN

#define chip_rst_H()    GPIO_BOP(GPIOC) = chip_rst
//Low
#define VACP_L()        GPIO_BC(GPIOA) = VACP
#define SHUTEN_L()      GPIO_BC(GPIOD) = SHUTEN
#define AZEN_L()        GPIO_BC(GPIOD) = AZEN  
#define nPWREN_L()      GPIO_BC(GPIOD) = nPWREN
#define chip_rst_L()    GPIO_BC(GPIOC) = chip_rst

/*  C   */
//High
#define BEEP_H()        GPIO_BOP(GPIOE) = BEEP
#define LED_WORK_OF()   GPIO_BOP(GPIOE) = LED_WORK              //
#define LED1_WORK_OF()   GPIO_BOP(GPIOC) = LED1_WORK              //GPIO_BOP(GPIOE) = LED_WORK
//Low
#define BEEP_L()        GPIO_BC(GPIOE) = BEEP
#define LED_WORK_ON()   GPIO_BC(GPIOE) = LED_WORK              //
#define LED1_WORK_ON()   GPIO_BC(GPIOC) = LED1_WORK              //GPIO_BOP(GPIOE) = LED_WORK

/*  other   */
#define POWERSAVE 0

/********************/
/*  handle  */
void BEEP_ON(void);
void BEEP_OFF(void);
void gpio_init(void);
unsigned char SwitchAntenna(unsigned char input_channel);
void goodread_led(void);
void closeread_led(int);
void LED_rf_pwron(unsigned char antid);
void LED_rf_pwroff(void);
unsigned char set_gpo_state(unsigned char port, unsigned char state);
unsigned char get_gpi_state(unsigned char *pstatebuf, unsigned short *plen_buf);
#endif

