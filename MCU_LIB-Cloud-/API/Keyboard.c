/*
 * Keyboard.c
 *
 *  Created on: 2022年7月22日
 *      Author: gxwl
 */
#include "keyboard.h"

int ASCII_TO_Keyboard(const char *Source,char *Target)
{
    int Length = strlen(Source);
//    printf("length : %d\n",Length);
    if (Length > 50)
        return (-1);
    char str[50];
    memcpy(str,Source,Length);  //  提取
    Target[Length] = '\0';
    for (int i = 0;i < Length; i++)
    {
        if (str[i] >= 'A' && str[i] <= 'Z')       //全部变成小写
        {
            str[i] += 32;

        }
//        printf("s : %d i: %d \n",str[i],i);       //debug
        if (str[i] >= 'a' && str[i] <= 'z')
        {
            Target[i] = str[i] - 93;         //97 - 04(a - z)
        }
        else if (str[i] >= '1' && str[i] <= '9')
        {
            Target[i] = str[i] - 19;         //49 - 30(1 - 9)
        }
        else if (str[i] == '0')
        {
            Target[i] = str[i] - 9;          //48 - 39(0)
        }
        else if (str[i] == '\n')     //Enter
        {
            Target[i] = str[i] -(-30);       //10 - 40( )
        }
        else if (str[i] == 27)       //ESC
        {
            Target[i] = str[i] -(-14);       //27 - 41()
        }
        else if (str[i] == 8)        //BACKSPACE
        {
            Target[i] = str[i] -(-34);       //08 - 42
        }
        else if (str[i] == 11)     //TAB
        {
            Target[i] = str[i] -(-32);       // 11- 43
        }
        else if (str[i] == ' ')     //SPACEBAR ' '
        {
            Target[i] = str[i] -(-12);       // 32- 44
        }
        else if (str[i] == '-')     //UNDERSCORE    -/_
        {
            Target[i] = str[i] -(0);        // 45- 45
        }
        else if (str[i] == '+')     //PLUS  =/+
        {
            Target[i] = str[i] -(-3);        // 43- 46
        }
        else if (str[i] == '{')     //OPEN_BRACKET [/{
        {
            Target[i] = str[i] - 76;        // 123- 47
        }
        else if (str[i] == '}')     //CLOSE_BRACKET ]/}
        {
            Target[i] = str[i] - 77;        // 125- 48
        }
        else if(str[i] == '~')       //TILDE ~
        {
            Target[i] = str[i] - 73;        // 126- 53
        }
        else if(str[i] == ',')       //TILDE ,/<
        {
            Target[i] = str[i] - (-10);        // 44- 54
        }
        else if(str[i] == '.')       //TILDE ./>
        {
            Target[i] = str[i] - (-9);        // 46- 55
        }
        else if (str[i] == 127)     //DEL
        {
            Target[i] = str[i] - 51;        // 127- 76
        }
        else {
            return 0;
        }
    }
    return Length;
}

void Keyboard_Send (char *Source,struct USB_FUN_ *FUN,char *Target)
{
    int i;
    for (i = 0; i < strlen(Source); i++)
    {
        FUN->Send(Source[i],Target);
        FUN->Delay(3);
    }
}
