#ifndef _PWM_H_
#define _PWM_H_

/*
	TIM4
	TIM8(高级)
	
	Period = (ARR + 1)*(PSC + 1)/CLK
	ARR   PSC 
	
							舵机
	20ms pwm	1.5ms(0)	0.5ms(-90)	2.5ms(+90)	
	arr = 1999	psc = 719	CLK = 72MHz	Period = 0.02s
	TIM4_PWM_Init(72, 100);
    TIM_SetCompare1(TIM4,20);
	
							电机
	1ms	arr = 1000	psc = 7	CLK = 72MHz	Period = 1ms
	
	GD和ST在模式上好像有差异，特别是PWM
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	这样才通用											--2021.11.21
	PWM使用特定IO不可修改IO相关参数（重映射/复用除外）
	底层
*/

#include "Items.h"

#define PWM_4	4
#define PWM_8	8

void TIM4_PWM_Init(u16 arr, u16 psc);

//void PWM_x_Init(char PWM_x, u16 arr, u16 psc, FunctionalState SET);

#endif
