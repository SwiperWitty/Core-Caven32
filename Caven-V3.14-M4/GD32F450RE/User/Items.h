#ifndef _ITEMS__H_
#define _ITEMS__H_

/*      
#include "Items.h"
*/
#include "gd32f4xx.h"
#include "system_gd32f4xx.h"     //软件配置的 基于内部晶振

#include "string.h"
#include "stdio.h"
#include "math.h"

/*
    SDK->Items->GPIO(Exist_GPIO)->BASE->
                                         \\
                                          -->[XXX]->MODE
                                         //
                    C(Lib)->Caven->API->

*/
#define TURE   TRUE
#define DEBUG_OUT   3           //Debug 通道(Caved 3.14是串口3)->MODE
#define MCU_SYS_Freq SystemCoreClock        //刚启动是xM，经过配置文件之后就是144（system_clock_config()之后）
#define DEFAULT_BAUD_RATE   115200
/*1字节帧头+2字节协议控制字+1字节485地址+2字节长度+1024字节数据+2字节CRC = 1032字节*/
#define UARTRXMAX 136
#define UARTTXMAX 512

#define DMA_BUF_LEN 64

#define BOARD_NV_BASEADDR  0x08000000
#define BOARD_NV_SIZE      0x00040000

#define BOARD_RAM_BASEADDR 0x20000000
#define BOARD_RAM_SIZE     0x00008000

#if (TARGET == GXRFID2008)
  #define  TARGET_GX_RFID_2008 GXRFID2008
  #define  TARGET_STRING "GXRFID2008"
#else
  #warning "Platform Target not defined!"
#endif

#define RF6004 1
#define RF8004 2
#define RF8008 3
#define RF6002 4
#define RF6001 5
#define RF6101 6
#define RF6008 7
#define RF6016 8
#define DESK   9
#define RF6201 10
#define XTM600 11
#define RF8208 12
#define RF8204 13
#define RF6601 14      //二代二版
#define RF2105_V5 15   //二代二版
#define RF2108 16   //二代二版  

#define GXRFID2008  1

#define HARWARE_ARCHITECTURE_VERSION_1 1
#define HARWARE_ARCHITECTURE_VERSION_2 2

#define GX_API 1
#define HL_API 2
#define YWG_API 3
#define GWDL_API 4

#define HW_MODBUS 1
#define XZ_MODBUS 2

#define BOARDTYPE   RF6001

//*             *//
                                             /*  基本外设就能实现的功能    */
#define Exist_SYS_TIME
//#define Exist_PWM
//#define Exist_ENCODE

//#define Exist_KEY
//#define Exist_LED
//#define Exist_BZZ

#define Exist_ADC
//#define Exist_DAC

#define Exist_UART
//#define Exist_IIC
#define Exist_SPI
#define Exist_USB
//#define Exist_CAN

#define Exist_FLASH
    
                                                    /*  只需要加上逻辑才能的功能    */
//#define Exist_LCD


#define	NOP()		__nop()

#define Reg_IO_H    scr
#define Reg_IO_L    clr

#define u8      uint8_t
#define u16     uint16_t
#define u32     uint32_t 

#define vu32    (__IO uint32_t)
#define vu16    (__IO uint16_t)
#define vu8     (__IO uint8_t)

#define GPIO_Pins_0  GPIO_PIN_0  
#define GPIO_Pins_1  GPIO_PIN_1  
#define GPIO_Pins_2  GPIO_PIN_2  
#define GPIO_Pins_3  GPIO_PIN_3  
#define GPIO_Pins_4  GPIO_PIN_4  
#define GPIO_Pins_5  GPIO_PIN_5  
#define GPIO_Pins_6  GPIO_PIN_6  
#define GPIO_Pins_7  GPIO_PIN_7  
#define GPIO_Pins_8  GPIO_PIN_8  
#define GPIO_Pins_9  GPIO_PIN_9  
#define GPIO_Pins_10 GPIO_PIN_10 
#define GPIO_Pins_11 GPIO_PIN_11 
#define GPIO_Pins_12 GPIO_PIN_12 
#define GPIO_Pins_13 GPIO_PIN_13
#define GPIO_Pins_14 GPIO_PIN_14 
#define GPIO_Pins_15 GPIO_PIN_15 
#define GPIO_Pin_ALL GPIO_PIN_ALL


#endif
