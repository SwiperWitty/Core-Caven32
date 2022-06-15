#include "BZZ.h"


void BZZ_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStrue;
	GPIO_InitStrue.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStrue.GPIO_Speed = GPIO_Speed_50MHz;
	
	RCC_APB2PeriphClockCmd(BZZ_GPIO_TIM, ENABLE);
	GPIO_InitStrue.GPIO_Pin = BZZ;
	GPIO_Init(BZZ_GPIO, &GPIO_InitStrue);
	
	BZZ_Control (BZZ_OF);
}

void BZZ_Control (char OUT)
{
	if(OUT) GPIO_ResetBits(BZZ_GPIO, BZZ);
	else GPIO_SetBits(BZZ_GPIO, BZZ);
}


