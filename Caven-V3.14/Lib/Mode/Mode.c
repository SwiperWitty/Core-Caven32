#include "Mode.h"

struct _Mode_Init Mode_Init;
struct _Mode_User Mode_User;

extern struct _LCD LCD;

void Mode_User_index(void)		//索引 功能函数 本体
{
    Mode_User.LCD = &LCD;
}

void Mode_Init_index(void)
{
    Mode_Init.LCD_Init = LCD_Init;

    Mode_User_index();
}
