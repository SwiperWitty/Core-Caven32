#include "Base_Exist_GPIO.h"

void LCD_GPIO_Init(int Set)
{
#ifdef Exist_LCD
    gpio_init_type  gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    if (Set)
    {
        crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);       //时钟

        gpio_init_struct.gpio_pins = GPIO_PINS_10;            //LCD_DC
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOA, &gpio_init_struct);
    }
    else
    {
        gpio_init_struct.gpio_pins = GPIO_PINS_10;
        gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOA, &gpio_init_struct);
    }
#endif
}

void LED_GPIO_Init(int Set)
{
#ifdef Exist_LED
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    if (Set) 
    {
        crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK,TRUE);
        crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK,TRUE);
        
        gpio_init_struct.gpio_pins = GPIO_PINS_4;
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOA, &gpio_init_struct);
        
        gpio_init_struct.gpio_pins = GPIO_PINS_2;
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOC, &gpio_init_struct);
    }
    else                                                    //标志取消GPIO
    {
        gpio_init_struct.gpio_pins = GPIO_PINS_4;
        gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOA, &gpio_init_struct);
    }
#endif
}

void BZZ_GPIO_Init(int Set)
{
#ifdef Exist_BZZ
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    if (Set) 
    {
        crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK,TRUE);

        gpio_init_struct.gpio_pins = GPIO_PINS_5;
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOB, &gpio_init_struct);
    }
    else                                                    //标志取消GPIO
    {
        gpio_init_struct.gpio_pins = GPIO_PINS_5;
        gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOB, &gpio_init_struct);
    }
#endif
}

void KEY_GPIO_Init(int Set)
{
#ifdef Exist_KEY
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    if (Set) {
        crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK,TRUE);

        gpio_init_struct.gpio_pins = GPIO_PINS_13;
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;                       //输入
        gpio_init_struct.gpio_pull = GPIO_PULL_UP;
        gpio_init(GPIOC, &gpio_init_struct);
    }
    else 
    {
        gpio_init_struct.gpio_pins = GPIO_PINS_13;
        gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOC, &gpio_init_struct);
    }
#endif
}

void HC138_GPIO_Init(int Set)
{
#ifdef Exist_HC138
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    if (Set) 
    {
        crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);       //时钟

        gpio_init_struct.gpio_pins = HC595_D1 | HC595_D2 | HC595_D3;
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOC, &gpio_init_struct);
    }
    else                                                    //标志取消GPIO
    {
        gpio_init_struct.gpio_pins = HC595_D1 | HC595_D2 | HC595_D3;
        gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOC, &gpio_init_struct);
    }
#endif
}

void HC595_GPIO_Init(int Set)
{
#ifdef Exist_HC595
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    if (Set) 
    {
        crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);       //时钟

        gpio_init_struct.gpio_pins = LATCH_CLOCK | SHIFT_CLOCK | HC595_Data;
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOC, &gpio_init_struct);
    }
    else                                                    //标志取消GPIO
    {
        gpio_init_struct.gpio_pins = LATCH_CLOCK | SHIFT_CLOCK | HC595_Data;
        gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOC, &gpio_init_struct);
    }
#endif
}

void DS18B20_GPIO_Init(int Set)
{
#ifdef Exist_DS18B20
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    if (Set) 
    {
        crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);       //时钟(切换GPIO记得看一眼)

        gpio_init_struct.gpio_pins = DS18B20_IO;
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(DS18B20_Clock, &gpio_init_struct);
    }
    else                                                    //标志取消GPIO
    {
        gpio_init_struct.gpio_pins = DS18B20_IO;
        gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(DS18B20_Clock, &gpio_init_struct);
    }
#endif
}

void DS18B20_IO_Config(int Set)
{
#ifdef Exist_DS18B20
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);       //时钟(切换GPIO记得看一眼)

    gpio_init_struct.gpio_pins = DS18B20_IO;
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_OPEN_DRAIN;
    if (Set)                                                        //输出
    {
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;

    }
    else                                                            //输入
    {
        gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;

    }
    gpio_init(DS18B20_Clock, &gpio_init_struct);
#endif
}
