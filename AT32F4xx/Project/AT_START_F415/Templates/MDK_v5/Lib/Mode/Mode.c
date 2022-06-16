#include "Mode.h"

struct _Mode_Init Mode_Init;
struct _Mode_User Mode_User;

void Mode_User_Index(void)
{
    Mode_User.LED.Switch = LED_X;
    Mode_User.RFID.Switch = Switch_RF;
    
}


void Mode_Index(void)
{
    Mode_Init.RF_GPIO = RF_GPIO_Init;
    Mode_Init.LED_GPIO = LED_GPIO_Init;
    
    Mode_User_Index();
}
