#include "motor.h"
#include "sys_time.h"

void Motorx_Init(char Motor_x)
{
	GPIO_InitTypeDef GPIO_InitStrue;
	GPIO_InitStrue.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStrue.GPIO_Speed = GPIO_Speed_50MHz;

	TIM8_PWM_Init(Arr_T8, Por_T8);//T = (arr * por) / 72MHz
	TIM_SetCompare1(TIM8, 0);
	TIM_SetCompare2(TIM8, 0);
	TIM_SetCompare3(TIM8, 0);
	TIM_SetCompare4(TIM8, 0);
	switch (Motor_x)
	{
		case 1:
		{
			RCC_APB2PeriphClockCmd(M1_GPIO_TIM1 | M1_GPIO_TIM2, ENABLE);

			GPIO_InitStrue.GPIO_Pin = M1_N1;
			GPIO_Init(M1_GPIO1, &GPIO_InitStrue);
			GPIO_InitStrue.GPIO_Pin = M1_N2;
			GPIO_Init(M1_GPIO2, &GPIO_InitStrue);

			GPIO_SetBits(M1_GPIO1, M1_N1);
			GPIO_SetBits(M1_GPIO2, M1_N2);
			break;
		}
		case 2:
		{
			RCC_APB2PeriphClockCmd(M2_GPIO_TIM1 | M2_GPIO_TIM2, ENABLE);

			GPIO_InitStrue.GPIO_Pin = M2_N1;
			GPIO_Init(M2_GPIO1, &GPIO_InitStrue);
			GPIO_InitStrue.GPIO_Pin = M2_N2;
			GPIO_Init(M2_GPIO2, &GPIO_InitStrue);

			GPIO_SetBits(M2_GPIO1, M2_N1);
			GPIO_SetBits(M2_GPIO2, M2_N2);
			break;
		}
		case 3:
		{
			RCC_APB2PeriphClockCmd(M3_GPIO_TIM1 | M3_GPIO_TIM2, ENABLE);

			GPIO_InitStrue.GPIO_Pin = M3_N1;
			GPIO_Init(M3_GPIO1, &GPIO_InitStrue);
			GPIO_InitStrue.GPIO_Pin = M3_N2;
			GPIO_Init(M3_GPIO2, &GPIO_InitStrue);

			GPIO_SetBits(M3_GPIO1, M3_N1);
			GPIO_SetBits(M3_GPIO2, M3_N2);
			break;
		}
		case 4:
		{
			RCC_APB2PeriphClockCmd(M4_GPIO_TIM1 | M4_GPIO_TIM2, ENABLE);

			GPIO_InitStrue.GPIO_Pin = M4_N1;
			GPIO_Init(M4_GPIO1, &GPIO_InitStrue);
			GPIO_InitStrue.GPIO_Pin = M4_N2;
			GPIO_Init(M4_GPIO2, &GPIO_InitStrue);

			GPIO_SetBits(M4_GPIO1, M4_N1);
			GPIO_SetBits(M4_GPIO2, M4_N2);
			break;
		}
		default:
			break;
	}
}

void Motorx_Control(char Motor_x, int Speed)
{
	int temp = Speed;
	char Rotation;
	
	temp = abs(temp);
	
	if(temp > 1000) temp = 1000;
	if(Speed > 0) Rotation = 'R';
	else if(Speed < 0) Rotation = 'L';
	else Rotation = ' ';
	
	switch (Motor_x)
	{
		case 1:
		{
			TIM_SetCompare1(TIM8, 0);
			if (Rotation == 'R') //Right
			{
				GPIO_ResetBits(M1_GPIO1, M1_N1);
				GPIO_SetBits(M1_GPIO2, M1_N2);
			}
			else if (Rotation == 'L') //Left
			{
				GPIO_SetBits(M1_GPIO1, M1_N1);
				GPIO_ResetBits(M1_GPIO2, M1_N2);
			}
			else
			{
				GPIO_ResetBits(M1_GPIO1, M1_N1);
				GPIO_ResetBits(M1_GPIO2, M1_N2);
			} //Stop
			TIM_SetCompare1(TIM8, temp);
			break;
		}
		case 2:
		{
			TIM_SetCompare2(TIM8, 0);
			if (Rotation == 'R')
			{
				GPIO_ResetBits(M2_GPIO1, M2_N1);
				GPIO_SetBits(M2_GPIO2, M2_N2);
			}
			else if (Rotation == 'L')
			{
				GPIO_SetBits(M2_GPIO1, M2_N1);
				GPIO_ResetBits(M2_GPIO2, M2_N2);
			}
			else
			{
				GPIO_ResetBits(M2_GPIO1, M2_N1);
				GPIO_ResetBits(M2_GPIO2, M2_N2);
			} //Stop
			TIM_SetCompare2(TIM8, temp);
			break;
		}
		case 3:
		{
			TIM_SetCompare3(TIM8, 0);
			if (Rotation == 'R') //Right
			{
				GPIO_ResetBits(M3_GPIO1, M3_N1);
				GPIO_SetBits(M3_GPIO2, M3_N2);
			}
			else if (Rotation == 'L') //Left
			{
				GPIO_SetBits(M3_GPIO1, M3_N1);
				GPIO_ResetBits(M3_GPIO2, M3_N2);
			}
			else
			{
				GPIO_ResetBits(M3_GPIO1, M3_N1);
				GPIO_ResetBits(M3_GPIO2, M3_N2);
			} //Stop
			TIM_SetCompare3(TIM8, temp);
			break;
		}
		case 4:
		{
			TIM_SetCompare4(TIM8, 0);
			if (Rotation == 'R') //Right
			{
				GPIO_ResetBits(M4_GPIO1, M4_N1);
				GPIO_SetBits(M4_GPIO2, M4_N2);
			}
			else if (Rotation == 'L') //Left
			{
				GPIO_SetBits(M4_GPIO1, M4_N1);
				GPIO_ResetBits(M4_GPIO2, M4_N2);
			}
			else
			{
				GPIO_ResetBits(M4_GPIO1, M4_N1);
				GPIO_ResetBits(M4_GPIO2, M4_N2);
			} //Stop
			TIM_SetCompare4(TIM8, temp);
			break;
		}
		default:
			break;
	}
}
