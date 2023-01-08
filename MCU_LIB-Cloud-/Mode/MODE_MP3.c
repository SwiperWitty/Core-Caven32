#include "MODE_MP3.h"


void Voice_Init(int Set)
{
#ifdef Exist_Voice
	Uart2_Init(Voice_Baud,Set);
#endif
}

void Voice(char Model,char Num)
{
#ifdef Exist_Voice
    char Array_mp3[8];
	if(Model == JQ8900)
	{
		Array_mp3[0] = 0xAA;
		Array_mp3[1] = 0x07;
		Array_mp3[2] = 0x02;
		Array_mp3[3] = 0x00;
		Array_mp3[4] = Num;
		Array_mp3[5] = 0xB3+Num;
	}
	else if(Model == JQ6500)
	{
		Array_mp3[0] = 0x7E;
		Array_mp3[1] = 0x04;
		Array_mp3[2] = 0x03;
		Array_mp3[3] = 0x00;
		Array_mp3[4] = Num;
		Array_mp3[5] = 0xEF;
	}
	else
		return;

    for(int n = 0;n < 6;n++)
    {
        UART_TXD_Send(2,Array_mp3[n]);
    }
#endif
}
