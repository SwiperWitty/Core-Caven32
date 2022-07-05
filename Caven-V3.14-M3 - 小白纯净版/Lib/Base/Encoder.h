#ifndef _ENCODER_H_
#define _ENCODER_H_

//	可以跨芯片移植	需要大改底层
/*

        ENCODE(编码器)使用特定IO不可修改IO相关参数（重映射/复用除外）
	底层
*/
#include "stm32f10x.h"
#include "Caven.h"

#define ENCODER_TIM_PERIOD (u16)(65535)   //最大就是 65535

void ENCODEx_Init(char TIME_x);
int Read_Encoder(char TIME_x);
	
#endif
