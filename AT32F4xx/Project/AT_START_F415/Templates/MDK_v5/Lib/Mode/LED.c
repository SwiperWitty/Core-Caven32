#include "LED.h"


void LED_GPIO_Init(void)
{
    #ifdef LED_6016
        GPIO_PinsRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB | RCC_APB2PERIPH_GPIOC, ENABLE);

        GPIO_InitType GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
        GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
        GPIO_InitStructure.GPIO_Pins = GPIO_Pins_All;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        GPIOB->BRE = GPIO_InitStructure.GPIO_Pins; // L

        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
        GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
        GPIO_InitStructure.GPIO_Pins = GPIO_Pins_6 | GPIO_Pins_7 | GPIO_Pins_8;
        GPIO_Init(GPIOC, &GPIO_InitStructure);
        GPIOC->BRE = GPIO_InitStructure.GPIO_Pins;
    #elif
        GPIO_PinsRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    #endif
}

char LED_X(char Channel)
{
    uint16_t temp = 0x0001;
    if (Channel > 16)
        return 0;
    char LED = Channel;
    
    if (LED <= 14)
    {
        if (LED >= 3)
            LED += 1;
        temp = temp << LED;

        GPIOB->BRE = GPIO_Pins_All;                 //关闭不需要的
        GPIOC->BRE = GPIO_Pins_6 | GPIO_Pins_7;     //关闭不需要的
        GPIOB->BSRE = temp;
    }
    else if (LED <= 16)
    {
        temp = temp << (LED - 9);
        GPIOC->BRE = GPIO_Pins_6 | GPIO_Pins_7;     //关闭不需要的
        GPIOB->BRE = GPIO_Pins_All;     
        GPIOC->BSRE = temp;
    }
    return Channel;
}

