#include "Mode.h"
#include "API.h"
#include "pic.h" //图片

#ifdef ST
    #define MCU 1 // STM32
#else
    #define MCU 0 // GD 32
#endif


int led;
int num = 0;
float Frame = 0;
char Free_array[100];
void Main_Init(void);


int main(void)
{
	Main_Init();

	while (1)
	{

        
	}
}

void Main_Init(void)
{
	API_Index();
	Mode_Index();
	
	LED_GPIO_Init(1);
    LED_H();
	LEDR_H();
}
