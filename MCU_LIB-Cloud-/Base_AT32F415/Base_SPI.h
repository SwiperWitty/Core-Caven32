#ifndef _BASE_SPI_H__
#define _BASE_SPI_H__

#ifdef DEFAULT
#include "items.h"              //默认功能
#endif

#ifndef DEFAULT
#include "User_items.h"         //自行设置功能，一般出现在本地文件的User中
#endif

/****************/

/*
            SCK    ->  
                        /----\
            MISO   ->   |SPI1|    <-    NSS/NSS2/NSS3  (Serial) 
                        \----/
            MOSI   ->
    
    分为硬件SPI/软件SPI
    软件SPI慢，但是它可以指定任意IO口(目前只发不收)。
    尽量使 SPI_GPIO 在同一组GPIO上（A/B/C/D）
    SPI是主动通信（主从），主机的收发逻辑不需要中断，但是从机需要（傻了吧，这是不全面的）。
    DMA和普通模式是可以一起用的
																					2022.02.26
    SPI的唯一要求————越快越好，目前软件模拟 461kHz(8bit)，硬件SPI参考-SPI_Speed-定义。
    软件模式 -- 4Mhz
    硬件普通模式 -- 36Mhz
    硬件DMA模式  -- 36Mhz
                                                                                    2022.07.26
    SPI的软件模拟方式为size-8bit,硬件SPI可以是size-8bit/size-16bit
    SPI软件方模拟方式为上升沿读(0,0)
    SPI一般只会使用一个（SPI1），但是硬件可能被占用于是选择SPI2（推荐只使用一个SPI）
                                                                                    2022.08.15
    SPI的硬件NSS就是一直拉低。狗都不用。
                                                                                    2022.08.19  
    SPI做主机：软件、硬件（包括DMA）完成，没有DMA接收
                                                                                    2022.10.14                                                                                                                                                                

*/

// 选择输出模式
#ifdef Exist_SPI
//    #define SPI_Software	        //屏蔽就是硬件模式
    #ifndef SPI_Software
//        #define SPI_DMA			    //屏蔽就是普通模式
    #endif
    #define HOST_MODE
    #define SPIx   2
#endif


#ifdef SPI_Software                                 //软件SPI
#define SPI_MODE_IN    GPIO_MODE_INPUT
#define SPI_MODE_OUT   GPIO_MODE_OUTPUT
#else                                               //硬件SPI
#define SPI_MODE_IN    GPIO_MODE_INPUT
#define SPI_MODE_OUT   GPIO_MODE_MUX
#define SPI_Speed   SPI_MCLK_DIV_8        //16-9MHZ   8-18MHZ     4-36MHZ     2-72MHZ
#define SPI_Size    SPI_FRAME_8BIT                  //8b   16b
#endif
#define SPI_MODE_NSS    GPIO_MODE_OUTPUT

//SPI1
#define SPI1_NSS        GPIO_PINS_4     //(CS)
#define SPI1_SCK        GPIO_PINS_5
#define SPI1_MISO       GPIO_PINS_6
#define SPI1_MOSI       GPIO_PINS_7
#define GPIO_SPI1       GPIOA

//SPI2
#define SPI2_NSS        GPIO_PINS_12    //(CS)
#define SPI2_SCK        GPIO_PINS_13
#define SPI2_MISO       GPIO_PINS_14
#define SPI2_MOSI       GPIO_PINS_15
#define GPIO_SPI2       GPIOB

void SPI_Start_Init(int Set);

//    驱动层      //

void SPI_CS_Set(char Serial,char State);

void SPI_Send_DATA(const uint16_t DATA);

//    调用层      //

void SPI_SET_Addr_SendData(char Serial,uint16_t Addr,uint16_t DATA);
uint16_t SPI_SET_Addr_ReadData(char Serial,uint16_t Addr);

void SPI_Send_String(const void * DATA,int num);



#if (SPIx == 1)
#define SPI_NSS_H()  GPIO_SPI1->scr = SPI1_NSS		//置高电平
#define SPI_NSS_L()  GPIO_SPI1->clr = SPI1_NSS 		//置低电平
#define SPI_SCK_H()  GPIO_SPI1->scr = SPI1_SCK
#define SPI_SCK_L()  GPIO_SPI1->clr = SPI1_SCK 
#define SPI_MOSI_H() GPIO_SPI1->scr = SPI1_MOSI
#define SPI_MOSI_L() GPIO_SPI1->clr = SPI1_MOSI

#define SPI_MISO_R() gpio_input_data_bit_read(GPIO_SPI1,SPI1_MISO)      //读取引脚电平

#elif (SPIx == 2)
#define SPI_NSS_H()  GPIO_SPI2->scr = SPI2_NSS		//置高电平
#define SPI_NSS_L()  GPIO_SPI2->clr = SPI2_NSS 		//置低电平
#define SPI_SCK_H()  GPIO_SPI2->scr = SPI2_SCK
#define SPI_SCK_L()  GPIO_SPI2->clr = SPI2_SCK 
#define SPI_MOSI_H() GPIO_SPI2->scr = SPI2_MOSI
#define SPI_MOSI_L() GPIO_SPI2->clr = SPI2_MOSI

#define SPI_MISO_R() gpio_input_data_bit_read(GPIO_SPI2,SPI2_MISO)      //读取引脚电平

#endif

#endif
