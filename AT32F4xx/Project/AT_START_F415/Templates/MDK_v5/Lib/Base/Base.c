#include "base.h"


struct _Base_Init Base_Init;
struct _Base_User Base_User;

void Base_User_Index(void)
{
    Base_User.Delay.Delay_10us = Delay_10us;
    Base_User.Delay.Delay_ms = Delay_ms;
    Base_User.Delay.Delay_S = Delay_S;

    Base_User.Uart.Send_Data = UARTx_Send_Data;
    Base_User.Uart.Send_String = UARTx_Send_String;

}

void Base_Index(void)
{
    Base_Init.Sys_time = Sys_time_Init;
    Base_Init.UARTx = UARTx_Init;

    Base_User_Index();
}
