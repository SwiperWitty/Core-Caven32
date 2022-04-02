#ifndef _DAC_H_
#define _DAC_H_

//	可以跨芯片移植	需要大改底层
/*
        DACx_Set_Vol 设置好电压会return输入的电压
        DAC使用特定IO不可修改IO相关参数（重映射/复用除外）
	底层
*/

#include "stm32f10x.h"

#define DAC_1			DAC_Channel_1
#define DAC_2			DAC_Channel_2

void DACx_Init(int DAC_X);
float DACx_Set_Vol(int DAC_X,float vol);

#endif
