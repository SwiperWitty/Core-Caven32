#ifndef __Ultrasonic_H
#define __Ultrasonic_H

//	可以跨芯片移植	需要大改底层
/* 
	基于STM32F10X的GPIO、SYS_TIME系统时钟
	增加无设备不死机功能（基于SYS_TIM）
								2021.10.27
	Caven 3.14版本以上
	加入屏蔽下载相关线程序
								2022.04.02

*/

#include "stm32f10x.h"
#include "sys_time.h"

#define Trig	GPIO_Pin_4
#define Echo	GPIO_Pin_5

#define CSB_T_GPIO	GPIOB
#define CSB_E_GPIO	GPIOB

#define CSB_T_GPIO_TIM	RCC_APB2Periph_GPIOB
#define CSB_E_GPIO_TIM	RCC_APB2Periph_GPIOB

void Ultrasonic_Init(void);//初始化
float Distance(void);//距离


#endif
