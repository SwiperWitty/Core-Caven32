#include "MP3.h"

char Array_mp3[8];

void Voice_Init(void)
{
	UARTx_Init (UART_2,Voice_Baud);
}

void Voice(char Model,char Num)
{
	if(Model == JQ8900)
	{
		Array_mp3[0] = 0xAA;
		Array_mp3[1] = 0x07;
		Array_mp3[2] = 0x02;
		Array_mp3[3] = 0x00;
		Array_mp3[4] = Num;
		Array_mp3[5] = 0xB3+Num;
		UARTx_Send_Data(UART_2,Array_mp3,6);
	}
	else if(Model == JQ6500)
	{
		Array_mp3[0] = 0x7E;
		Array_mp3[1] = 0x04;
		Array_mp3[2] = 0x03;
		Array_mp3[3] = 0x00;
		Array_mp3[4] = Num;
		Array_mp3[5] = 0xEF;
		UARTx_Send_Data(UART_2,Array_mp3,6);
	}
	else
		return;
}
