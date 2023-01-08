#ifndef _Ultrasonic__H_
#define _Ultrasonic__H_

//	可以跨芯片移植	需要大改底层
/* 
	基于STM32F10X的GPIO、SYS_TIME系统时钟
	增加无设备不死机功能（基于SYS_TIM）
								2021.10.27
	Caven 3.14版本以上
	加入屏蔽下载相关线程序
								2022.04.02

*/

#include "Base.h"
#include "Caven.h"

// Trig	PB04
// Echo	PB05


struct Ultrasonic_
{
    float (*Distance)(void);
};

void Ultrasonic_Init(int SET);//初始化
float Distance(void);//距离

#endif
