#include "MODE.h"
#include "API.h"

int led;
char Free_array[100];
void Main_Init(void);
int main (void)
{
    Main_Init();
    struct _Over_time LED = {
       .Now_Time = &SYS_Time.Watch,
       .Now_data = &led,
       .Set_Time.second = 1,
       .Set_Time.time_us = 100000,
    };
    
    while(1)
    {
		SYS_Time.Watch = Mode_User.Sys_Clock.Get_TIME();
        API.Time.Over_Time(&LED);
        Mode_User.LED.LED_SET(1,LED.Flip);
        
        if(LED.Flag)
        {
			sprintf(Free_array,"->%2d:%2d:%2d \r\n", SYS_Time.Watch.hour,SYS_Time.Watch.minutes,SYS_Time.Watch.second);
            Debug_Out(Free_array);
//            printf("temp - %5.2f c \r\n ",Get_MCU_Temp());
            
		}
        
        if(CV_UART[1]->Rxd_Received == 1)
        {
            Mode_User.UART.WAY_Send_Data(1,CV_UART[1]->DATA.Poit_U8,CV_UART[1]->DATA.Length);
            Mode_User.UART.WAY_Send_String(1," \r\n");
            Destroy(&CV_UART[1],sizeof(CV_UART[1]));
        }
        SYS_Delay_ms (5);
//        SPI_SET_Addr_SendData(1,led++,0XAA);
        memset(Free_array,0xf1,10);
        IIC_Send_DATA(0x54,Free_array,0,10,12);
    }
}

void Main_Init(void)
{
    Mode_Index();
    API_Index();

    Mode_Init.Sys_Clock(ENABLE);
    Mode_Init.LED(ENABLE);
    Mode_Init.UART(1,115200,ENABLE);
    SPI_Start_Init(ENABLE);

    
    Mode_User.Delay.Delay_ms(100);
    Mode_User.UART.WAY_Send_String(1,"hello world !\r\n");
    Debug_Out("hello world !\r\n");
	
	sprintf(Free_array,"->%2d:%2d:%2d \r\n", SYS_Time.Watch.hour,SYS_Time.Watch.minutes,SYS_Time.Watch.second);
	Debug_Out(Free_array);
	
	Mode_User.Sys_Clock.Set_TIME(SYS_Time.Watch);
	Mode_User.Delay.Delay_ms(1000);
    
    IIC_Start_Init(ENABLE);
}
