#include "mode.h"

struct _Mode_Init Mode_Init;
struct _Mode_User Mode_User;    //结构体实体


void Mode_User_index(void)		//索引 功能函数 本体
{
    Mode_User.LCD.Fill = LCD_Fill;

    Mode_User.LCD.Draw_Point = LCD_Draw_Point;
    Mode_User.LCD.Draw_Line = LCD_Draw_Line;
    Mode_User.LCD.Draw_Circle = LCD_Draw_Circle;
    Mode_User.LCD.Draw_Rectangle = LCD_Draw_Rectangle;
    
    Mode_User.LCD.Show_String = LCD_Show_String;
    Mode_User.LCD.Show_Chinese = LCD_Show_Chinese;
    Mode_User.LCD.Show_Picture = LCD_Show_Picture;
}

void Mode_Index(void)
{
    Mode_Init.LCD_Init = LCD_Init;

    Mode_User_index();
}
