#include "LED.h"


void LED_Init(int SET)
{
#ifdef Exist_LED
    LED_GPIO_Init(SET);
    LED_SET(1,DISABLE);
    
#endif
}

void LED_SET(char Channel,int SET)
{
#ifdef Exist_LED
    switch (Channel) {
        case 1:
            if(SET)
                LED_L();
            else
                LED_H();
            break;
        case 2:
            if(SET)
                LEDR_L();
            else
                LEDR_H();
            break;
        default:
            break;
    }

#endif
}

void WS2812_Delay(int time)
{
    volatile int var;
    for (var = time;var > 0;var--)
    {

    }
}

void WS2812_Reset (void)
{
#ifdef Exist_LED
	LED_L();
	WS2812_Delay(60);
	LED_H();
#endif
}

void WS2812_write_byte(char data)
{
#ifdef Exist_LED
	char temp = data;
	for(char i = 0; i < 8; i++)
	{
	    if((temp << i) & 0x80)
	    {
	        LED_H();
	        WS2812_Delay(7);
	        LED_L();
	        WS2812_Delay(3);
	    }
	    else
	    {
            LED_H();
            WS2812_Delay(3);
            LED_L();
	        WS2812_Delay(6);
        }
	}
	LED_L();
#endif
}

void LED_REG(struct Caven_Color Color,int SET)
{
//    NVIC_SETPRIMASK();
	if(SET)
	{
		WS2812_write_byte(0xaa);
		WS2812_write_byte(0x01);
		WS2812_write_byte(0x00);
	}
	else
	{
		WS2812_write_byte(0);
		WS2812_write_byte(0);
		WS2812_write_byte(0);
	}
//	NVIC_RESETPRIMASK();
}

