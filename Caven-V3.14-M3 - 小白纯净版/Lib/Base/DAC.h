#ifndef _DAC_H_
#define _DAC_H_

//	可以跨芯片移植	需要大改底层
/*
        DAC_x_Set_Vol 设置好电压会return输入的电压
        DAC使用特定IO不可修改IO相关参数（重映射/复用除外）
	底层
*/

#include "stm32f10x.h"

#define DAC_1			DAC_Channel_1
#define DAC_2			DAC_Channel_2

struct DAC_
{
	float (*DAC_x_Set_Vol)(int DAC_x,float vol);
};

void DAC_x_Init(char DAC_x,FunctionalState SET);
float DAC_x_Set_Vol(int DAC_x,float vol);

#endif
