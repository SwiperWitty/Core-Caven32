/*
 * keyboard.h
 *
 *  Created on: 2022年7月22日
 *      Author: gxwl
 *      键盘表
 */

#ifndef API_KEYBOARD_H_
#define API_KEYBOARD_H_

#include "Caven.h"

struct USB_FUN_
{
    void (*Send)(char Temp,char * DATA);
    void (*Delay)(int num);

};

int ASCII_TO_Keyboard(const char *Source,char *Target);
void Keyboard_Send (char *Source,struct USB_FUN_ *FUN,char *Target);

#endif /* API_KEYBOARD_H_ */
