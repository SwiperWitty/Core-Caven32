#include "Base_Exist_GPIO.h"

void LCD_GPIO_Init(int SET)
{
#ifdef Exist_LCD
    gpio_init_type GPIO_InitStructure;
    if (SET)
    {
        crm_periph_reset(CRM_GPIOA_PERIPH_RESET, TRUE);   //使能A端口时钟

        GPIO_InitStructure.gpio_pins = GPIO_PINS_10;      //LCD_DC
        GPIO_InitStructure.gpio_mode = GPIO_MODE_OUTPUT;         //推挽输出
        GPIO_InitStructure.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
        GPIO_InitStructure.gpio_pull = GPIO_PULL_NONE;
        GPIO_InitStructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;   
    }
    else
    {
        GPIO_InitStructure.gpio_pins = GPIO_PINS_10;
        GPIO_InitStructure.gpio_mode = GPIO_MODE_INPUT;
    }
    gpio_init(GPIOA, &GPIO_InitStructure);    //初始化GPIOA
#endif
}

void LED_GPIO_Init(int SET)
{
#ifdef Exist_LED
    GPIO_InitTypeDef GPIO_InitStructure;
    if (SET) 
    {
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;       //输出
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽    OD开漏（有阻）
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;    //上下拉
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //高速
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    }
    else                                                    //标志取消GPIO
    {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;        //模拟输入
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;    //上下拉
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    }
#endif
}

void BZZ_GPIO_Init(int SET)
{
#ifdef Exist_BZZ
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    if (SET) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    }
    else {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    }
#endif
}

void KEY_GPIO_Init(int SET)
{
#ifdef Exist_KEY
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    if (SET) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
        PWR_BackupAccessCmd(ENABLE);
//        RCC_LSEConfig(RCC_LSE_OFF);
        BKP_TamperPinCmd(DISABLE);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOC, &GPIO_InitStructure);
    }
    else {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOC, &GPIO_InitStructure);
    }
    PWR_BackupAccessCmd(DISABLE);/* 禁止修改RTC和后备寄存器*/

#endif
}

