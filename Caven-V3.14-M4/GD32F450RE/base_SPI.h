#ifndef _BASE__SPI_H
#define _BASE__SPI_H

/*base_SPI.h*/

#include "gd32f4xx.h"
#include <stdio.h>

/*SPI1*/
#define SPI1_CS1    GPIO_PIN_9  /*gpioB*/

#define SPI1_CLK    GPIO_PIN_3  /*gpioD*/
#define SPI1_MISO   GPIO_PIN_2  /*gpioC  gpioI*/
#define SPI1_MOSI   GPIO_PIN_3  /*gpioC  gpioI*/

/*SPI2*/
#define SPI2_CS1    GPIO_PIN_9  /*gpioB*/
#define SPI2_CS2    GPIO_PIN_9  /*gpioB*/
#define SPI2_CS3    GPIO_PIN_9  /*gpioB*/

#define SPI2_CLK    GPIO_PIN_3  /*gpioD*/
#define SPI2_MISO   GPIO_PIN_2  /*gpioC  gpioI*/
#define SPI2_MOSI   GPIO_PIN_3  /*gpioC  gpioI*/

/*  DEFINE  */
#define TRANSFER_FLAG (1 << 15)
#define WRITE_FLAG    ((1 << 14) | TRANSFER_FLAG)
#define READ_FLAG     ((0 << 14) | TRANSFER_FLAG)

/*  NSS  */
#define SPI1_CS1_H()     GPIO_BC(GPIOB) = SPI1_CS1   //High-GPIO_BC Low-GPIO_BOP 

#define SPI2_CS1_H()     GPIO_BC(GPIOB) = SPI2_CS1          
#define SPI2_CS2_H()     GPIO_BC(GPIOB) = SPI2_CS2
#define SPI2_CS3_H()     GPIO_BC(GPIOB) = SPI2_CS3

/*  handle  */
void spi1_init(void);

int spix_write_fpga_data(char spix,char nss,uint16_t add,uint16_t data);
int spix_read_fpga_data(char spix,char nss,uint16_t add,uint16_t *data);


#endif

