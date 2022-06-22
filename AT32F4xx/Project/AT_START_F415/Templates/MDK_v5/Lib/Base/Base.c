#include "base.h"

/*
    结构体实体+索引（初始化索引+功能索引）
*/

struct Base_Init_ Base_Init;
struct Base_User_ Base_User;

void Base_User_Index(void)
{
    Base_User.Sys_Time.Delay_10us = Delay_10us;
    Base_User.Sys_Time.Delay_ms = Delay_ms;
    Base_User.Sys_Time.Delay_S = Delay_S;

    Base_User.Uart.Send_Data = UARTx_Send_Data;
    Base_User.Uart.Send_String = UARTx_Send_String;

}

void Base_Index(void)
{
    Base_Init.Sys_time = Sys_time_Init;
    Base_Init.UARTx = UARTx_Init;

    Base_User_Index();
}
