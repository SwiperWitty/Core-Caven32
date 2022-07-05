#include "Steering_engine.h"

#define Angle_Num	1.11

int Set_Angle_Cal_180(float Angle)	//舵机角度计算	0C = (0 * Angle_Num) + 150 = 150 <set angle calculator>
{
	int Time_PWM = 150;
	if(Angle <= 90 || Angle >= -90) Time_PWM = (Angle * Angle_Num) + 150;
	return Time_PWM;
}

void Init_Steering_Engine_T4 (void)
{
	SysTick_Config(SystemCoreClock/100000);					//360 Steering_Engine_360_Sport	Init
	PWM_x_Init(Steering_PWM, Arr_T4, Por_T4, ENABLE);							//20ms pwm  20ms 1.5ms (0) 0.5ms (-90) 2.5ms (+90)
	TIM_SetCompare1(TIM4, Set_Angle_Cal_180(0));
	TIM_SetCompare2(TIM4, Set_Angle_Cal_180(0));
	TIM_SetCompare3(TIM4, Set_Angle_Cal_180(0));
	TIM_SetCompare4(TIM4, Set_Angle_Cal_180(0));
}

void Steering_Engine_360_Sport(char Channel,char Rotation,char Speed,int Time)
{
	int Time_PWM = 150;
	if(Speed > 100)	Speed = 100;
	if(Rotation == 'r' || Rotation == 'R')
	{
		Time_PWM = 150 - (Speed * 0.5);
	}
	else if(Rotation == 'l' || Rotation == 'L')
	{
		Time_PWM = 150 + (Speed * 0.5);
	}
	switch(Channel)
	{
		case(1):
		{
			TIM_SetCompare1(TIM4, Time_PWM);Delay_ms(Time);TIM_SetCompare1(TIM4, 150);
			break;
		}
		case(2):
		{			
			TIM_SetCompare2(TIM4, Time_PWM);Delay_ms(Time);TIM_SetCompare2(TIM4, 150);
			break;
		}
		case(3):
		{			
			TIM_SetCompare3(TIM4, Time_PWM);Delay_ms(Time);TIM_SetCompare3(TIM4, 150);
			break;
		}
		case(4):
		{			
			TIM_SetCompare4(TIM4, Time_PWM);Delay_ms(Time);TIM_SetCompare4(TIM4, 150);
			break;
		}
		default:
			break;
	}
}

void Steering_Engine_Angle(char Channel,float Angle)
{
	switch(Channel)
	{
		case(1):
		{
			TIM_SetCompare1(TIM4, Set_Angle_Cal_180(Angle));
			break;
		}
		case(2):
		{			
			TIM_SetCompare2(TIM4, Set_Angle_Cal_180(Angle));
			break;
		}
		case(3):
		{			
			TIM_SetCompare3(TIM4, Set_Angle_Cal_180(Angle));
			break;
		}
		case(4):
		{			
			TIM_SetCompare4(TIM4, Set_Angle_Cal_180(Angle));
			break;
		}
		default:
			break;
	}
}
