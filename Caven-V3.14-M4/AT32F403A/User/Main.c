#include "Mode.h"
#include "pic.h"

/*
            软件文件夹->AT32文件夹->Keil工程
项目文件->  
            云端库文件夹...
*/
#ifdef PICTURE
	#define Photo1 0
	#define Photo2 gImage_pai
#endif

int temp = 0;
char array_buff[300];
u8 spi_buff[10];
void Main_Init(void);
/*
	Power_app
	Steering_Engine_app
	Games_app
	boot_app
*/
int main (void)
{
	int retval = 0;
    Main_Init();
    sprintf(array_buff,"Caven Desk ");
//    Mode_Use.LCD.Show_String_pFun(5,0,array_buff,LCD_BLUE,LCD_Back_Color,24);
    for(int i = 0; i < 10;i++)
    {
        spi_buff[i] = i;
    }

    while(1)
    {
		
        Mode_Use.TIME.Delay_Ms(3);
		if(retval)
		{
			break;
		}
    }
}

void Main_Init(void)
{
    system_clock_config();
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
    crm_adc_clock_div_set(CRM_ADC_DIV_6);
    //
    Mode_Index();
    
    Mode_Init.TIME_Init_State = Mode_Init.TIME(ENABLE);
	Mode_Init.UART_Init_State = Mode_Init.UART(DEBUG_OUT,115200,ENABLE);
	
	Mode_Use.TIME.Delay_Ms(500);
	Mode_Init.LCD_Init_State = Mode_Init.LCD(ENABLE);
    
	Mode_Use.UART.Send_String_pFun(DEBUG_OUT,"Hello world ! \n");
    Base_UART_DMA_Send_Data(DEBUG_OUT,"debug dma send \n",strlen("debug dma send \n"));
    
        
#ifdef PICTURE
	Mode_Use.LCD.Show_Picture_pFun(0,0,240,240,Photo2);     //Photo
#endif

}
