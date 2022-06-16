#ifndef _MODE_H__
#define _MODE_H__

#include "LED.h"
#include "RFID_SET.h"

/*
    本MODE文件依赖于 UART文件，AT32的SDK文件
    移植需要修改大量底层寄存器
    不支持跨平台
*/



struct _Mode_Init
{
    void (*LED_GPIO)(void);
    void (*RF_GPIO)(void);
};

struct _Mode_User
{
    struct _LED LED;
    struct _RFID RFID;

};

extern struct _Mode_Init Mode_Init;         //板载硬件资源
extern struct _Mode_User Mode_User;         //板载硬件资源

void Mode_Index(void);

#endif
