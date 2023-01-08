#include "Steering_engine.h"

#define Angle_coefficient	1.11


void Steering_Engine_Init (int Set)
{
#ifdef Exist_Steering_Engine
    TIM4_PWM_Start_Init(Arr_T4,Por_T4,Set);
    Steering_Engine_Angle(1,0);
    Steering_Engine_Angle(2,0);
    Steering_Engine_Angle(3,0);
    Steering_Engine_Angle(4,0);     //默认0°
#endif

}

/*
舵机角度计算
x = Angle_coefficient
90° ->  (-90 * x) + 150 = 50  
0°  ->  (0 * x) + 150   = 150 <set angle calculator>    
90° ->  (90 * x) + 150  = 250  
解得 Angle_coefficient = 1.11
*/
int Set_Angle_Cal_180(float Angle)	
{
    int Time_PWM = 150;
#ifdef Exist_Steering_Engine
	
	if(Angle <= 90 || Angle >= -90) 
    {
        Time_PWM = (Angle * Angle_coefficient) + 150;       //符合范围
    }
#endif
    return Time_PWM;
}

/*  通道、设置角度  */
void Steering_Engine_Angle(char Channel,float Angle)
{
#ifdef Exist_Steering_Engine

    TIM4_PWMx_SetValue(Channel,Set_Angle_Cal_180(Angle));

#endif
}

/*  通道、旋转方向、旋转的速度、保持的时间  */
void Steering_Engine_360_Sport(char Channel,char Rotation,char Speed,int Time)
{
#ifdef Exist_Steering_Engine
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
    TIM4_PWMx_SetValue(Channel,Time_PWM);
#endif
}
