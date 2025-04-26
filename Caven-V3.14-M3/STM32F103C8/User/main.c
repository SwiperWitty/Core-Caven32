#include "Mode.h"
#include "API.h"
#include "pic.h" //图片



char temp_array[100];
void Main_Init(void);


int main(void)
{
	Main_Init();
	User_GPIO_config(1,11,1);
	User_GPIO_config(3,13,1);
	while (1)
	{
		User_GPIO_set(1,11,0);
		User_GPIO_set(3,13,0);
		Mode_Use.TIME.Delay_Ms(500);
		User_GPIO_set(1,11,1);
		User_GPIO_set(3,13,1);
		Mode_Use.TIME.Delay_Ms(500);
        
	}
}

void Main_Init(void)
{
    int reverse = 0;
	// 直接使用外部时钟
	Mode_Index();
	Mode_Init.TIME(ENABLE);
	Mode_Init.UART(m_UART_CH2,15200,ENABLE);
	//
    Mode_Use.UART.Send_String_pFun(m_UART_CH2,"hello !\n");
}
