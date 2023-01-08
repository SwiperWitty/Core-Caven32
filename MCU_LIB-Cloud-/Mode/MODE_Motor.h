#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "Base.h"
#include "Caven.h"

//	可以跨芯片移植	需要大改底层
/*
	V1.0				n多年前
	V1.1	(end)		2021.9.18

	V3.14	(Pro),依赖于HC595(方向设置) and TIM8-pwm(调速)
	当然，如果你只是用DRV8833，调速就会失效。
						2022.04.02
*/

#define Motor_1 1
#define Motor_2 2
#define Motor_3 3
#define Motor_4 4

#define Motor_PWM 8
#define Arr_T8 1000
#define Por_T8 72

//	T = (arr * por) / 72MHz = 1MS

/*

		Motorx_Control(char Motor_x, int Speed); 去除此函数方向传参，由Speed的 +/-来判别方向

											2021.12.05
*/



#endif
