#ifndef	__STEERING_ENGINE_H_
#define	__STEERING_ENGINE_H_

#include "Base.h"
#include "Caven.h"

#define Steering_PWM 4
#define	Arr_T4	2000
#define Por_T4	SystemCoreClock / 100000      //144 000 000 / 100000 = 1440

//	T = (arr + 1 * por + 1) / 144MHz = 20MS

struct Steering_Engine_
{
    void (*Set_Angle)(char Channel,float Angle);
    void (*Set_360_Sport)(char Channel,char Rotation,char Speed,int Time);
};

void Steering_Engine_Init (int Set);
void Steering_Engine_Angle(char Channel,float Angle);
void Steering_Engine_360_Sport(char Channel,char Rotation,char Speed,int Time);

#endif
