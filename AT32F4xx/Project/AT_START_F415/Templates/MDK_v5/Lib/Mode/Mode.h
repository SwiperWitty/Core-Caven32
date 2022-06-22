#ifndef _MODE_H__
#define _MODE_H__

#include "LED.h"
#include "RFID_SET.h"

/*
    本MODE文件依赖于 AT32 的SDK文件，它是一个管理函数的文件，本身不具备功能
    移植需要修改大量底层寄存器
    不支持跨平台
    结构体数据 (_xxx_xx)
    结构体函数 (xxx_xx_)
*/



struct Mode_Init_
{
    void (*LED_GPIO)(void);
    void (*RF_GPIO)(void);
};

struct Mode_User_
{
    struct LED_ LED;
    struct RFID_ RFID;

};

extern struct Mode_Init_ Mode_Init;         //板载硬件资源
extern struct Mode_User_ Mode_User;         //板载硬件资源

void Mode_Index(void);

#endif
