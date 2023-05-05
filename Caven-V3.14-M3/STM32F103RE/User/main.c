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

void Delay(int i)
{
    do
    {
        int j = 1000000;
        do
        {
            
        }while(j--);
    }while(i--);
}

int main(void)
{
	Main_Init();
	struct _Over_time LED = {
	   .Now_Time = &SYS_Time.Watch,
	   .Now_data = &led,
	   .Set_Time.second = 1,
	   .Set_Time.time_us = 0,
	};

	while (1)
	{
//		LED_L();
//		Delay(10);
		
		Delay(10);
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
