#include "key.h"

void KEY_Init(FunctionalState SET)
{
	if(SET)
	{
		KEY_GPIO_Init();
	}
	else
	{
		KEY_GPIO_Exit();
	}
}

