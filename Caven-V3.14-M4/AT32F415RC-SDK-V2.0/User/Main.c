#include "Mode.h"
#include "API.h"
#include "pic.h"

/*
            软件文件夹->AT32文件夹->Keil工程
项目文件->  
            云端库文件夹...
*/

int temp = 0;
U8 temp_1 = 0;
int temp_2 = 0;
int temp_3 = 0;
U8 temp_String[300];
U8 temp1_String[300];
char *temp_po;
U16 *temp_list;
char temp_data[0x2000];
U16 temp1_data[1000];
float temp_f;
int i;

void Main_Init(void);
int main (void)
{
    for(int a = 0;a < 124;a ++)
    {
        temp_String[a] = 0x88;
        temp_data[a] = a;
    }
    Main_Init();
    
    while(1)
    {
        Mode_User.Sys_Clock.Get_TIME();
        
        if(temp != SYS_Time.Watch.second)       //
        {
            temp = SYS_Time.Watch.second;
//            printf("Time - %2d:%2d:%2d \r\n",SYS_Time.Watch.hour,SYS_Time.Watch.minutes,SYS_Time.Watch.second);

//            printf("temp : %5.2f \r\n",ADC_Get_Temperature());
            Mode_User.USER_ADC.WAY_Get_List();
//            printf("str[A0]: %d\r\n str[A1]: %d\r\n str[A2]: %d\r\n \r\n",temp_list[0],temp_list[1],temp_list[2]);
//            printf("str[B0]: %5.2f\r\n str[B1]: %5.2f\r\n \r\n",Mode_User.USER_ADC.WAY_Conversion_Vol(temp_list[3]),Mode_User.USER_ADC.WAY_Conversion_Vol(temp_list[4]));
            
        }
        if(Mode_User.UART.DATA_UART[1]->Rxd_Received)
        {
            if(Mode_User.UART.DATA_UART[1]->DATA.Poit_U8[0] == '1')
            {
                Mode_User.Delay.Delay_ms(10);
                Mode_User.UART.WAY_Send_Data(1,Mode_User.UART.DATA_UART[1]->UART_RxdBuff,Mode_User.UART.DATA_UART[1]->DATA.Length);

            }
            
            Destroy(Mode_User.UART.DATA_UART[1],sizeof(*Mode_User.UART.DATA_UART[1]));
        }
        
        if(SYS_Time.Watch.second % 2)
        {
            Mode_User.LED.LED_SET(2,DISABLE);
        }
        else
        {
            Mode_User.LED.LED_SET(2,ENABLE);
        }
        
        temp_1 ++;
        if(temp_1 > 100) 
        {temp_1 = 0;}
        
        SPI_CS_Set(1,TRUE);

        for(int a = 0;a < 100;a ++)
        {
            
            SPI_Send_DATA(a);
        }
        SPI_CS_Set(1,0);
        //SPI_Send_String(temp1_String,200);
        
        Mode_User.Delay.Delay_ms(10);
//        SPI_SET_Addr_SendData(1,0xf8,0xaa);
		
//		
		TIM4_PWMx_SetValue(1,14);
		Mode_User.Delay.Delay_ms(10);
		
		TIM4_PWMx_SetValue(1,0);
		Mode_User.Delay.Delay_ms(100);

    }
}

void Main_Init(void)
{
    system_clock_config();
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
    Mode_Index();
    API_Index();
    
    Mode_Init.Sys_Clock(ENABLE);
    Mode_Init.LCD(ENABLE);
    Mode_Init.LED(ENABLE);
    Mode_Init.UART(1,115200,ENABLE);
    Mode_Init.User_ADC(ENABLE);
    Mode_Init.USB();
    
	TIM4_PWM_Start_Init(27,1440,ENABLE);		//25 144
//	TIM4_PWMx_SetValue(1,125);
	
//    Mode_Init.Steering_Engine(ENABLE);
    Mode_Init.UART(2,115200,ENABLE);
//    Mode_Init.UART(3,115200,ENABLE);   
    if(Mode_Init.DS18B20(ENABLE))
    {
        Mode_User.UART.WAY_Send_String(1,"s: Secc DS\r\n");
    }
    else
    {
        Mode_User.UART.WAY_Send_String(1,"s: Error DS\r\n");
    }
    
    char TEMP_ARRAY[200] = {"{see you ! }\r\n"};
    if(Fast_R08_Flash(FLASH_DATA_START) == '{')
    {
        printf("p: flash secc !\r\n");
        printf("p: %s \n",(char *)FLASH_DATA_START);
    }
    else
    {
        printf("p: flash null !\r\n");
        Flash_Save_Area (TEMP_ARRAY,GET_Addr_Area(FLASH_DATA_START),50);       //存
        printf("p: %s \n",(char *)FLASH_DATA_START);
        
    }
//    keyboard_send_string((u8 *)FLASH_DATA_START, strlen((char *)FLASH_DATA_START));       //会while

//    for(int i = 0;i <= 10;i++)
//    {
//        for(int j = 0;j <= 0x0f;j++)
//        {
//            printf("%02x ",temp_po[i*16 + j]);
//        }
//        printf(" --%d-- \r\n",i);       //发初始数据
//    }

    Mode_User.LCD.Show_Picture(0,0,60,60,gImage_am_60);     //Photo
    Mode_User.LED.LED_SET(1,DISABLE);
    Mode_User.LED.LED_SET(2,DISABLE);
//    Mode_User.Steering_Engine.Set_Angle(1,90);
    Mode_User.Delay.Delay_ms(100);

    Mode_User.Sys_Clock.Set_TIME(SYS_Time.Watch);
    
    printf("system_core_clock: %d \r\n",SystemCoreClock);
    
//    Mode_User.UART.WAY_Send_String(1,"s: hello world !\r\n");
//    printf("p: hello world !\r\n");
    temp_list = Mode_User.USER_ADC.DATA_List;

}
