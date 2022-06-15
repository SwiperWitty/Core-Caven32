/**
  ******************************************************************************
  * File   : FLASH/operate_SPIM/SPIM/SPIM_test.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file contains all the macros used in SPIM_test.c
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPIM_TEST_H
#define __SPIM_TEST_H

/* Define External SPI flash type, define only one ---------------------------*/
#define FLASH_SPIM_TYPE1  1
#define FLASH_SPIM_TYPE2  2

/* Define External SPI flash size --------------------------------------------*/
#define SPIM_PAGE_SIZE    4096

/* Define External SPI flash unlock key --------------------------------------*/
#define FLASH_KEY1                      ((UINT32)0x45670123)
#define FLASH_KEY2                      ((UINT32)0xCDEF89AB)

/* Define External SPI flash testing address ---------------------------------*/
#define SPIM_TEST_ADDR   0x8400000

/* Exported functions ------------------------------------------------------- */
void SPIM_test(void);

#endif /* __SPIM_TEST_H */

