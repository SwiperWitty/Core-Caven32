/**
  **************************************************************************
  * File   : delay.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : none
  **************************************************************************
  */
 
#ifndef __DELAY_H
#define __DELAY_H 			   
#include "sys.h"  

	 
void delay_init(void);
void delay_ms(u16 nms);
void delay_us(u32 nus);

#endif





























