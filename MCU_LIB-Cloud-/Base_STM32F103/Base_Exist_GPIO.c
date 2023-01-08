#include "Base_Exist_GPIO.h"

void LCD_GPIO_Init(int Set)
{
#ifdef Exist_LCD
    GPIO_InitTypeDef  gpio_init_struct;
    GPIO_StructInit(&gpio_init_struct);
    if (Set)
    {
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA, ENABLE);       //时钟

        gpio_init_struct.GPIO_Pin  = GPIO_Pin_10;
        gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
        gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOA, &gpio_init_struct);
    }
    else
    {
        gpio_init_struct.GPIO_Pin  = GPIO_Pin_10;
        gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
        gpio_init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOA, &gpio_init_struct);
    }
#endif
}

void LED_GPIO_Init(int Set)
{
#ifdef Exist_LED
    GPIO_InitTypeDef gpio_init_struct;
    GPIO_StructInit(&gpio_init_struct);
    if (Set) 
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
        GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);    //支持 SWD，禁用 JTAG，PA15/PB3/PB4 可作GPIO

        RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOB, ENABLE);       //时钟
        gpio_init_struct.GPIO_Pin  = GPIO_Pin_5 | GPIO_Pin_4;
        gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
        gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOB, &gpio_init_struct);

    }
    else                                                    //标志取消GPIO
    {
        gpio_init_struct.GPIO_Pin  = GPIO_Pin_4;
        gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
        gpio_init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOA, &gpio_init_struct);
    }
#endif
}

void BZZ_GPIO_Init(int Set)
{
#ifdef Exist_BZZ
    GPIO_InitTypeDef gpio_init_struct;
    GPIO_StructInit(&gpio_init_struct);
    if (Set) 
    {
        crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK,TRUE);

        gpio_init_struct.GPIO_Pin = GPIO_Pin_5;
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        GPIO_Init(GPIOB, &gpio_init_struct);
    }
    else                                                    //标志取消GPIO
    {
        gpio_init_struct.GPIO_Pin = GPIO_Pin_5;
        gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        GPIO_Init(GPIOB, &gpio_init_struct);
    }
#endif
}

void KEY_GPIO_Init(int Set)
{
#ifdef Exist_KEY
    GPIO_InitTypeDef gpio_init_struct;
    GPIO_StructInit(&gpio_init_struct);
    if (Set) {
        crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK,TRUE);

        gpio_init_struct.GPIO_Pin = GPIO_Pin_13;
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;                       //输入
        gpio_init_struct.gpio_pull = GPIO_PULL_UP;
        GPIO_Init(GPIOC, &gpio_init_struct);
    }
    else 
    {
        gpio_init_struct.GPIO_Pin = GPIO_Pin_13;
        gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        GPIO_Init(GPIOC, &gpio_init_struct);
    }
#endif
}

void HC138_GPIO_Init(int Set)
{
#ifdef Exist_HC138
    GPIO_InitTypeDef gpio_init_struct;
    GPIO_StructInit(&gpio_init_struct);
    if (Set) 
    {
        crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);       //时钟

        gpio_init_struct.GPIO_Pin = HC595_D1 | HC595_D2 | HC595_D3;
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        GPIO_Init(GPIOC, &gpio_init_struct);
    }
    else                                                    //标志取消GPIO
    {
        gpio_init_struct.GPIO_Pin = HC595_D1 | HC595_D2 | HC595_D3;
        gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        GPIO_Init(GPIOC, &gpio_init_struct);
    }
#endif
}

void HC595_GPIO_Init(int Set)
{
#ifdef Exist_HC595
    GPIO_InitTypeDef gpio_init_struct;
    GPIO_StructInit(&gpio_init_struct);
    if (Set) 
    {
        crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);       //时钟

        gpio_init_struct.GPIO_Pin = LATCH_CLOCK | SHIFT_CLOCK | HC595_Data;
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        GPIO_Init(GPIOC, &gpio_init_struct);
    }
    else                                                    //标志取消GPIO
    {
        gpio_init_struct.GPIO_Pin = LATCH_CLOCK | SHIFT_CLOCK | HC595_Data;
        gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        GPIO_Init(GPIOC, &gpio_init_struct);
    }
#endif
}

void DS18B20_GPIO_Init(int Set)
{
#ifdef Exist_DS18B20
    GPIO_InitTypeDef gpio_init_struct;
    GPIO_StructInit(&gpio_init_struct);
    if (Set) 
    {
        crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);       //时钟(切换GPIO记得看一眼)

        gpio_init_struct.GPIO_Pin = DS18B20_IO;
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        GPIO_Init(DS18B20_Clock, &gpio_init_struct);
    }
    else                                                    //标志取消GPIO
    {
        gpio_init_struct.GPIO_Pin = DS18B20_IO;
        gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        GPIO_Init(DS18B20_Clock, &gpio_init_struct);
    }
#endif
}

void DS18B20_IO_Config(int Set)
{
#ifdef Exist_DS18B20
    GPIO_InitTypeDef gpio_init_struct;
    GPIO_StructInit(&gpio_init_struct);
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);       //时钟(切换GPIO记得看一眼)

    gpio_init_struct.GPIO_Pin = DS18B20_IO;
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
    GPIO_Init(DS18B20_Clock, &gpio_init_struct);
#endif
}
