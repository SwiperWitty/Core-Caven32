#include "Mode.h"


/*
            软件文件夹->AT32文件夹->Keil工程
项目文件->  
            云端库文件夹...
*/


void Main_Init (void);

int main(void)
{
    Main_Init ();
    while(1)
    {
        GPIOA->clr = GPIO_PINS_0;  //Blue
    }
}

void Main_Init (void)
{
    system_clock_config();
    crm_adc_clock_div_set(CRM_ADC_DIV_8);   //ADC时钟设置为96/8 （12Mhz）
    gpio_init_type gpio_init_struct;

    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    gpio_default_para_init(&gpio_init_struct);
	
	Mode_Index();
	Mode_Init.UART(1,115200,ENABLE);
	Mode_User.UART.WAY_Send_String(1,"hello world ! \r\n");
	
    /* configure the led gpio */
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
    gpio_init_struct.gpio_pins = GPIO_PINS_2 | GPIO_PINS_1 | GPIO_PINS_0;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOA, &gpio_init_struct);
	
    GPIOA->scr = GPIO_PINS_2;       //1		//Blue
	GPIOA->scr = GPIO_PINS_1;       //1		//Green
	GPIOA->scr = GPIO_PINS_0;       //1		//red
		
}


