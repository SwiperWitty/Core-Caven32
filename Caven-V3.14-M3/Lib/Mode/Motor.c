#include "motor.h"
#include "sys_time.h"

void Motorx_Init(FunctionalState Set)
{
	if (Set)
	{
		PWM_x_Init(Motor_PWM, Arr_T8, Por_T8, ENABLE); // T = (arr * por) / 72MHz
		TIM_SetCompare1(TIM8, 0);
		TIM_SetCompare2(TIM8, 0);
		TIM_SetCompare3(TIM8, 0);
		TIM_SetCompare4(TIM8, 0);
	}
	else
	{
	}
}

void Motorx_Control(char Motor_x, int Speed)
{
	int temp = Speed;
	char Rotation;

	temp = abs(temp);

	if (temp > 1000)
		temp = 1000;
	if (Speed > 0)
		Rotation = 'R';
	else if (Speed < 0)
		Rotation = 'L';
	else
		Rotation = ' ';

	switch (Motor_x)
	{
	case 1:
	{
		TIM_SetCompare1(TIM8, 0);
		if (Rotation == 'R') // Right
		{
		}
		else if (Rotation == 'L') // Left
		{
		}
		else
		{

		} // Stop
		TIM_SetCompare1(TIM8, temp);
		break;
	}
	case 2:
	{
		TIM_SetCompare2(TIM8, 0);
		if (Rotation == 'R')
		{
		}
		else if (Rotation == 'L')
		{
		}
		else
		{

		} // Stop
		TIM_SetCompare2(TIM8, temp);
		break;
	}
	case 3:
	{
		TIM_SetCompare3(TIM8, 0);
		if (Rotation == 'R') // Right
		{
		}
		else if (Rotation == 'L') // Left
		{
		}
		else
		{

		} // Stop
		TIM_SetCompare3(TIM8, temp);
		break;
	}
	case 4:
	{
		TIM_SetCompare4(TIM8, 0);
		if (Rotation == 'R') // Right
		{
		}
		else if (Rotation == 'L') // Left
		{
		}
		else
		{

		} // Stop
		TIM_SetCompare4(TIM8, temp);
		break;
	}
	default:
		break;
	}
}
