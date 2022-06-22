#include "Mode.h"

struct Mode_Init_ Mode_Init;
struct Mode_User_ Mode_User;

void Mode_User_Index(void)
{
    Mode_User.LED.Switch = LED_X;
    Mode_User.LED.Power_LED = Power_LED;
    Mode_User.RFID.Switch = Switch_RF;
    
}


void Mode_Index(void)
{
    Mode_Init.RF_GPIO = RF_GPIO_Init;
    Mode_Init.LED_GPIO = LED_GPIO_Init;
    
    Mode_User_Index();
}
