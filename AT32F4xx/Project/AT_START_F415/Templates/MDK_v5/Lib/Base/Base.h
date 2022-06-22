#ifndef _BASE_H__
#define _BASE_H__

#include "sys_time.h"
#include "uart_x.h"

/* 
    本文件是对功能的索引，自身并不具有功能属性
    Base这是底层基本功能，与芯片耦合度很高，移植工程量很大，但是框架基本一致
    结构体数据 (_xxx_xx)
    结构体函数 (xxx_xx_)
*/

//1.结构体内的函数指针的函数名比函数本体更简洁   --2022.06.16

struct Base_Init_
{
    void (*UARTx)(char Channel,int bound);
    void (*Sys_time)(FunctionalState Set);
};

struct Base_User_
{
    struct Sys_Time_ Sys_Time;
    struct uart_ Uart;
};


extern struct Base_Init_ Base_Init;
extern struct Base_User_ Base_User;         //在此声明，这样Main中就可以将此省略

void Base_Index(void);
void Base_User_Index(void);

#endif
