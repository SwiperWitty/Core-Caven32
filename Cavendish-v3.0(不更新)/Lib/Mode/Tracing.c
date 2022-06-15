#include "Tracing.h"

char Tracing_Array[6];

void Tracing_Init (void)
{
	GPIO_InitTypeDef GPIO_InitStrue;
	GPIO_InitStrue.GPIO_Mode = GPIO_Mode_IPU;
	RCC_APB2PeriphClockCmd(Trac_1_GPIO_TIM | Trac_2_GPIO_TIM | Trac_3_GPIO_TIM | Trac_4_GPIO_TIM | Trac_5_GPIO_TIM, ENABLE);
	
	GPIO_InitStrue.GPIO_Pin = Trac_1;
	GPIO_Init(Trac_1_GPIO, &GPIO_InitStrue);
	GPIO_InitStrue.GPIO_Pin = Trac_2;
	GPIO_Init(Trac_2_GPIO, &GPIO_InitStrue);
	GPIO_InitStrue.GPIO_Pin = Trac_3;
	GPIO_Init(Trac_3_GPIO, &GPIO_InitStrue);
	GPIO_InitStrue.GPIO_Pin = Trac_4;
	GPIO_Init(Trac_4_GPIO, &GPIO_InitStrue);
	GPIO_InitStrue.GPIO_Pin = Trac_5;
	GPIO_Init(Trac_5_GPIO, &GPIO_InitStrue);
}

char * Tracing_Find (void)
{
	*(Tracing_Array) = 0;
	if(GPIO_ReadInputDataBit(Trac_1_GPIO, Trac_1) == 1)	*(Tracing_Array+1) = '1';
	else *(Tracing_Array+1) = '0';
	if(GPIO_ReadInputDataBit(Trac_2_GPIO, Trac_2) == 1) *(Tracing_Array+2) = '1';
	else *(Tracing_Array+2) = '0';
	if(GPIO_ReadInputDataBit(Trac_3_GPIO, Trac_3) == 1) *(Tracing_Array+3) = '1';
	else *(Tracing_Array+3) = '0';
	if(GPIO_ReadInputDataBit(Trac_4_GPIO, Trac_4) == 1) *(Tracing_Array+4) = '1';
	else *(Tracing_Array+4) = '0';
	if(GPIO_ReadInputDataBit(Trac_5_GPIO, Trac_5) == 1) *(Tracing_Array+5) = '1';
	else *(Tracing_Array+5) = '0';
	*(Tracing_Array) = ' ';
	return Tracing_Array;
}
