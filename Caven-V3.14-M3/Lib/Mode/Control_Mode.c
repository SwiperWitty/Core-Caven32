#include "Control_Mode.h"


void Control_Mode_Init (void)
{
	TIM8_PWM_Init(1000, 72);//T = (arr * por) / 72MHz
	
	TIM_SetCompare3(TIM8, 0);
	TIM_SetCompare4(TIM8, 0);
}

void Control_Mode_OUT(char OUTX,int PWM)
{
	if(OUTX == 1)
	{
		TIM_SetCompare3(TIM8, PWM);
	}
	else if(OUTX == 2)
	{
		TIM_SetCompare4(TIM8, PWM);
	}
	else
	{
		TIM_SetCompare3(TIM8, 0);
		TIM_SetCompare4(TIM8, 0);
	}
	
}


