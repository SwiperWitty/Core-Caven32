#include "Mode.h"
#include "API.h"
#include "pic.h"

/*
            软件文件夹->AT32文件夹->Keil工程
项目文件->  
            云端库文件夹...
*/
#define Photo1 gImage_lan
#define Photo2 gImage_dogen

int temp = 0;
//char *temp_po;
//U16 *temp_list;

//float temp_f;
int i;

void Main_Init(void);
int main (void)
{
    Main_Init();
    
    while(1)
    {
        Mode_User.Sys_Clock.Get_TIME();
		if(Mode_User.KEY.KEY_State(1) == 0)
		{
			if(Mode_User.KEY.KEY_State(1) == 0)
			{
				i++;
				if(i > 100)
				{i = 1;}
				printf("Key num : %d",i);
			}
			do{
				Mode_User.Delay.Delay_ms(5);
				
			}while(Mode_User.KEY.KEY_State(1) == 0);
		}
		if(i%2 == 1 && temp != i)
		{
			temp = i;
			Mode_User.LCD.Show_Picture(0,0,240,240,Photo2);     //Photo
		}
		else if(i%2 == 0 && temp != i)
		{
			temp = i;
			Mode_User.LCD.Show_Picture(0,0,240,240,Photo1);     //Photo
		}
		
//        if(temp != SYS_Time.Watch.second)       //
//        {
//            temp = SYS_Time.Watch.second;
//            printf("Time - %2d:%2d:%2d \r\n",SYS_Time.Watch.hour,SYS_Time.Watch.minutes,SYS_Time.Watch.second);

//            printf("temp : %5.2f \r\n",ADC_Get_Temperature());
//            Mode_User.USER_ADC.WAY_Get_List();
//            printf("str[A0]: %d\r\n str[A1]: %d\r\n str[A2]: %d\r\n \r\n",temp_list[0],temp_list[1],temp_list[2]);
//            printf("str[B0]: %5.2f\r\n str[B1]: %5.2f\r\n \r\n",Mode_User.USER_ADC.WAY_Conversion_Vol(temp_list[3]),Mode_User.USER_ADC.WAY_Conversion_Vol(temp_list[4]));
            
//        }
//        if(Mode_User.UART.DATA_UART[1]->Rxd_Received)
//        {
//            if(Mode_User.UART.DATA_UART[1]->DATA.Poit_U8[0] == '1')
//            {
//                Mode_User.Delay.Delay_ms(10);
//                Mode_User.UART.WAY_Send_Data(1,Mode_User.UART.DATA_UART[1]->UART_RxdBuff,Mode_User.UART.DATA_UART[1]->DATA.Length);

//            }
//            
//            Destroy(Mode_User.UART.DATA_UART[1],sizeof(*Mode_User.UART.DATA_UART[1]));
//        }
        

		
//		
//		TIM4_PWMx_SetValue(1,14);
//		Mode_User.Delay.Delay_ms(10);
//		
//		TIM4_PWMx_SetValue(1,0);
//		Mode_User.Delay.Delay_ms(100);

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
	Mode_Init.UART(DEBUG_OUT,115200,ENABLE);
    Mode_Init.KEY(1,ENABLE);
//    Mode_Init.User_ADC(ENABLE);

//    Mode_Init.USB();
    
//	TIM4_PWM_Start_Init(27,1440,ENABLE);		//25 144
//    Mode_Init.Steering_Engine(ENABLE);
    
//    if(Fast_R08_Flash(FLASH_DATA_START) == '{')
//    {
//        printf("p: flash secc !\r\n");
//        printf("p: %s \n",(char *)FLASH_DATA_START);
//    }
//    else
//    {
//        printf("p: flash null !\r\n");
//        Flash_Save_Area (TEMP_ARRAY,GET_Addr_Area(FLASH_DATA_START),50);       //存
//        printf("p: %s \n",(char *)FLASH_DATA_START);
//        
//    }
//    keyboard_send_string((u8 *)FLASH_DATA_START, strlen((char *)FLASH_DATA_START));       //会while

    
	
//    Mode_User.LED.LED_SET(1,DISABLE);

//    Mode_User.Steering_Engine.Set_Angle(1,90);
//    Mode_User.Delay.Delay_ms(100);

//    Mode_User.Sys_Clock.Set_TIME(SYS_Time.Watch);
	Mode_User.LCD.Show_Picture(0,0,240,240,Photo1);     //Photo
    printf("system_core_clock: %d \r\n",SystemCoreClock);

//    temp_list = Mode_User.USER_ADC.DATA_List;

}
