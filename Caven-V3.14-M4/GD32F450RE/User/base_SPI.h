#ifndef _BASE__SPI_H
#define _BASE__SPI_H

/*base_SPI.h*/

#include "Items.h"

/*  DEFINE  */
#define TRANSFER_FLAG (1 << 15)
#define WRITE_FLAG    ((1 << 14) | TRANSFER_FLAG)
#define READ_FLAG     ((0 << 14) | TRANSFER_FLAG)

/*SPI1*/
#define SPI1_CS1    GPIO_PIN_9  /*gpioB*/

#define SPI1_CLK    GPIO_PIN_3  /*gpioD*/
#define SPI1_MISO   GPIO_PIN_2  /*  gpioI*/
#define SPI1_MOSI   GPIO_PIN_3  /*  gpioI*/

/*SPI2*/
#define SPI2_CS1    GPIO_PIN_15 /*gpioA*/
#define SPI2_CS2    GPIO_PIN_0  /*gpioD*/
#define SPI2_CS3    GPIO_PIN_1  /*gpioD*/

#define SPI2_CLK    GPIO_PIN_10 /*gpioC*/
#define SPI2_MISO   GPIO_PIN_4  /*gpioB*/       //empty
#define SPI2_MOSI   GPIO_PIN_5  /*gpioB*/

/*  NSS  */
#define SPI1_CS1_H()     GPIO_BOP(GPIOB) = SPI1_CS1   //High-GPIO_BC Low-GPIO_BOP 
#define SPI1_CS1_L()     GPIO_BC(GPIOB) = SPI1_CS1   //High-GPIO_BC Low-GPIO_BOP 

#define SPI2_CS1_H()     GPIO_BOP(GPIOA) = SPI2_CS1          
#define SPI2_CS2_H()     GPIO_BOP(GPIOD) = SPI2_CS2
#define SPI2_CS3_H()     GPIO_BOP(GPIOD) = SPI2_CS3
#define SPI2_CS1_L()     GPIO_BC(GPIOA) = SPI2_CS1          
#define SPI2_CS2_L()     GPIO_BC(GPIOD) = SPI2_CS2
#define SPI2_CS3_L()     GPIO_BC(GPIOD) = SPI2_CS3

/*  handle  */
void spi1_init(void);
void spi2_init(void);

int spi_write_fpga_reg(uint16_t addr, uint16_t data);
uint32_t spi_read_fpga_reg(uint16_t addr, uint16_t* data);
void spi2anachip(uint16_t data,uint16_t nss);

#endif

