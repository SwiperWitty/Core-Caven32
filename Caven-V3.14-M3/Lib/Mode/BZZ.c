#include "BZZ.h"


void BZZ_Init(FunctionalState SET)
{
	if (SET)
	{
		BZZ_GPIO_Init();
		BZZ_Control (BZZ_OF);
	}
	else
	{
		BZZ_GPIO_Exit();
	}
	
	BZZ_Control (BZZ_OF);
}

void BZZ_Control (char OUT)
{
	if(OUT) BZZ_L();		//开
	else BZZ_H();			//关
}


