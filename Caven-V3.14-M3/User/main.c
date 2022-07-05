#ifdef Cavendish
    #include "base.h"
    #include "Mode.h"
    #include "API.h"
    #include "pic.h" //图片

    #ifdef ST
        #define MCU 1 // STM32
    #else
        #define MCU 0 // GD 32
    #endif
#endif

//  未完成  ！！！！ //
//要写入到STM32 FLASH的字符串数组
#define SIZE sizeof(Free_Array)	   //数组长度
#define FLASH_SAVE_ADDR 0X08012000 // FLASH_SAVE_ADDR >= 本代码所占用FLASH的大小(16进制) + 0X08000000 + 0X800)

float far, temp, vol;
char Free_Show[100];

int abd,debug_time_data;
void Mian_Init(void);
int main(void)
{
	Mian_Init();
	struct _Over_time Debug_OVTime = {
        .Now_Time = &SYS_Time.Watch,
        .Now_data = &debug_time_data,
        .Set_Time.second = 1
    };
	while (1)
	{
        API.DATA_Time.Over_Time(&Debug_OVTime);
        if (Debug_OVTime.Flag) {
            sprintf(Free_Show,"-- Time: %d:%d:%d - %d --\r\n",SYS_Time.Watch.hour,SYS_Time.Watch.minutes,SYS_Time.Watch.second,SYS_Time.Watch.time_num);
            Base_User.UART.Send_String(USART3,Free_Show);
        }
		sprintf(Free_Show,"-> TIME: %2d:%2d:%2d S  ",SYS_Time.Watch.hour,SYS_Time.Watch.minutes,SYS_Time.Watch.second);
		Mode_User.LCD.Show_String(0, 3, Free_Show, GBLUE, BLACK, 16);
        Destroy(Free_Show,Free_Show);
		abd = KEY_Touch_IN();
		if(abd == 0)
		{
			BZZ_Control (BZZ_ON);
			Base_User.UART.Send_String(USART3,"KEY -L !\r\n");
			Mode_User.LCD.Show_String(21, 3, "0 ", GBLUE, BLACK, 16);
		}
		else
		{
			BZZ_Control (BZZ_OF);
			Mode_User.LCD.Show_String(21, 3, "1 ", GBLUE, BLACK, 16);
		}
		if(CV_UART[3].Rxd_Received)
        {
            Base_User.UART.Send_Data(USART3,CV_UART[3].DATA.Buff,CV_UART[3].DATA.Length);
            Destroy(&CV_UART[3],CV_UART[3]);
        }
          
	}
}

void Mian_Init(void)
{
	Base_Index();
    API_Index();
	Mode_Index();

	Base_Init.SYS_Time_Init(ENABLE);
	Base_Init.Uart(3, 115200, ENABLE);

	Base_Init.ADC_x_Init(MCU_Temp, ENABLE);

	Mode_Init.LCD_Init(ENABLE);
	Mode_User.LCD.Show_Picture(180, 0, 60, 60, gImage_am_60);

	if (MCU)
		Mode_User.LCD.Show_String(22, 0, "ST", BLUE, BLACK, 16);
	else
		Mode_User.LCD.Show_String(22, 0, "GD", MAGENTA, BLACK, 16); // X = 21、Y = 0 为起点 显示 "GD  "	字体为 BLUE，背景色为 BLACK，字体16
	Mode_User.LCD.Show_String(5, 0, "Caven Pro", RED, BLACK, 24);
	Mode_User.LCD.Show_String(29, 14, "1", RED, BLACK, 16); //这里是16字 的极限 0-29（30行） 0-14(15列)
	memset(Free_Show,'-',26);				//分割区
	Mode_User.LCD.Show_String(0, 2, "-> MCU : 26.10 C ", BLUE, BLACK, 16);
	Mode_User.LCD.Show_String(0, 3, "-> TIME: 00:00:00 S ", GBLUE, BLACK, 16);
	Mode_User.LCD.Show_String(2, 4, Free_Show, WHITE, BLACK, 16);
	memset(Free_Show,0,sizeof(Free_Show));				//分割区

	KEY_Init(ENABLE);
	BZZ_Init(ENABLE);
	Base_User.Delay.Delay_ms(10);
	Base_User.UART.Send_String(USART3, "{Ready !}\r\n");
}
