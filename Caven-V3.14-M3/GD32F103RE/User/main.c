#ifdef Cavendish
    #include "Mode.h"
    #include "API.h"
    #include "pic.h" //图片

#endif

int led;
int num = 0;
float Frame = 0;
char Free_array[100];
void Main_Init(void);


int main(void)
{
	Main_Init();
    int a = 0;

	while (1)
	{
        Mode_User.Delay.Delay_ms(500);
        printf("num %d \r\n",a++);
        Mode_User.Delay.Delay_ms(500);

	}
}

void Main_Init(void)
{
    HSI_SetSysClock(RCC_PLLMul_16);
	API_Index();
	Mode_Index();
    LCD_Init(1);
    
    Mode_User.LCD.Show_String(0,0,"1234",WHITE,BACK_COLOR,16);
//    Mode_Init.Sys_Clock(ENABLE);
    Sys_Time_Init(ENABLE);
	Mode_Init.UART(3,115200,ENABLE);
        
    Mode_User.UART.WAY_Send_String(3,"ck3:hello !\r\n");

	LED_GPIO_Init(1);
    LED_H();
    printf("mcu system %d",MCU_SYS_Freq);
}


