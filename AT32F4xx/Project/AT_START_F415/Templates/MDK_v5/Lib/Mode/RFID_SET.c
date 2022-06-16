#include "RFID_SET.h"

void SKY_13418(char CTR_x, char channel)
{
    if (channel >= 8)
        return;
    int RF_X = channel;
    GPIOA->BSRE = GPIO_Pins_11 | GPIO_Pins_12; 
    GPIOA->BRE = GPIO_Pins_3 | GPIO_Pins_4 | GPIO_Pins_5 | GPIO_Pins_8 | GPIO_Pins_9 | GPIO_Pins_10;
    
    if (CTR_x == 1)
    {
        GPIOA->BSRE = RF_X << 8;
        GPIOA->BRE = GPIO_Pins_11; 
    }
    else if (CTR_x == 2)
    {
        GPIOA->BSRE = RF_X << 3;
        GPIOA->BRE = GPIO_Pins_12; 
    }
    
}

char Switch_RF(char RF_X)
{
    if (RF_X > 16)
        return 0;
    switch (RF_X)
    {
    case 1:
        RF_01();
        break;
    case 2:
        RF_02();
        break;
    case 3:
        RF_03();
        break;
    case 4:
        RF_04();
        break;
    case 5:
        RF_05();
        break;
    case 6:
        RF_06();
        break;
    case 7:
        RF_07();
        break;
    case 8:
        RF_08();
        break;
    case 9:
        RF_09();
        break;
    case 10:
        RF_10();
        break;
    case 11:
        RF_11();
        break;
    case 12:
        RF_12();
        break;
    case 13:
        RF_13();
        break;
    case 14:
        RF_14();
        break;
    case 15:
        RF_15();
        break;
    case 16:
        RF_16();
        break;

    default:
        break;
    }
    return RF_X;
}

void RF_GPIO_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO, ENABLE);
    GPIO_PinsRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);
    GPIO_InitType GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_3 | GPIO_Pins_4 | GPIO_Pins_5 | GPIO_Pins_8 | GPIO_Pins_9 | GPIO_Pins_10 | GPIO_Pins_11 | GPIO_Pins_12;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIOA->BSRE = GPIO_InitStructure.GPIO_Pins;
}
