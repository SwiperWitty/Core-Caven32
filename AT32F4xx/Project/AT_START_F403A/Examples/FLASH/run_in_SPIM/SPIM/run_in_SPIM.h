/**
  ******************************************************************************
  * File   : FLASH/run_in_SPIM/SPIM/run_in_SPIM.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file contains all the macros used in run_in_SPIM.c
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RUN_IN_SPIM_H
#define __RUN_IN_SPIM_H

/* Define External SPI flash type, define only one ---------------------------*/
#define FLASH_BANK3_TYPE1  1
#define FLASH_BANK3_TYPE2  2

/* Define External SPI flash unlock key --------------------------------------*/
#define FLASH_KEY1                      ((UINT32)0x45670123)
#define FLASH_KEY2                      ((UINT32)0xCDEF89AB)

/* Exported functions ------------------------------------------------------- */
void delay_init(void);
void delay_ms(unsigned int nms);
void LED_Init(void);
void SPIM_run(void);

#endif /* __RUN_IN_SPIM_H */

