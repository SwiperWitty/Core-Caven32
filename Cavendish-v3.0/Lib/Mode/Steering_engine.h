#ifndef	__STEERING_ENGINE_H_
#define	__STEERING_ENGINE_H_

#include "PWM.h"
#include "sys_time.h"

#define	Arr_T4	1999
#define Por_T4	720

//	T = (arr + 1 * por + 1) / 72MHz = 20MS

void Init_Steering_Engine_T4(void);
void Steering_Engine_Angle(char Channel,float Angle);
void Steering_Engine_360_Sport(char Channel,char Rotation,char Speed,int Time);

#endif
