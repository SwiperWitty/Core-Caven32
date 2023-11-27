/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
/*
 * 主频使用96mhz，这样操作flash不需要降频，同时可以使用USB。
 *
 */
#include "main.h"

u32 temp_num;
int u2_rxd_num = 0;
int main(void)
{
    Main_Init();
    temp_num = 0;
    Caven_Watch_Type now_time;
    now_time = Mode_Use.TIME.Get_Watch_pFun();

    Caven_Watch_Type last_time = {
            .hour = 8,
            .minutes = 30,
            .second = 40,
    };
    Caven_Watch_Type diff_time;
    diff_time = Mode_Use.TIME.Get_differ_pFun(last_time,now_time);
    printf("diff time: %d : %d : %d , %d (us)\n",diff_time.hour,diff_time.minutes,diff_time.second,diff_time.time_us);
    while(1)
    {
        Mode_Use.LED.SET_pFun(1,ENABLE);
        Mode_Use.BZZ.SET_pFun(DISABLE);
        Mode_Use.TIME.Delay_Ms(500);

        Mode_Use.LED.SET_pFun(1,DISABLE);
        Mode_Use.BZZ.SET_pFun(ENABLE);
        Mode_Use.TIME.Delay_Ms(500);

        printf("test num : %d \n",temp_num++);

        now_time = Mode_Use.TIME.Get_Watch_pFun();
//        printf("sys time: %d : %d : %d , %d (us)\n",now_time.hour,now_time.minutes,now_time.second,now_time.time_us);

        if (u2_rxd_num) {
            Mode_Use.TIME.Delay_Ms(5);
            Mode_Use.UART.Send_String_pFun(m_UART_CH2,"yes ! \n");
            u2_rxd_num = 0;
        }

    }
}

void UART1_Getrx_Fun (u8 data);
void UART2_Getrx_Fun (u8 data);
void UART3_Getrx_Fun (u8 data);

void Main_Init(void)
{
//    SystemInit() 由系统自启完成
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Mode_Index();
    Mode_Init.TIME(ENABLE);

    Mode_Init.LED(ENABLE);
    Mode_Init.BZZ(ENABLE);

    Mode_Init.UART(DEBUG_OUT,115200,ENABLE);
    Mode_Use.UART.Send_String_pFun(DEBUG_OUT,"u1:hello world ! \n");
    Mode_Use.UART.Receive_Bind_pFun(m_UART_CH1,UART1_Getrx_Fun);

    Mode_Init.UART(m_UART_CH2,115200,ENABLE);
    Mode_Use.UART.Send_String_pFun(m_UART_CH2,"u2:hello world ! \n");
    Mode_Use.UART.Receive_Bind_pFun(m_UART_CH2,UART2_Getrx_Fun);

    printf("SystemClk:%d \r\n", MCU_SYS_FREQ);

    Caven_Watch_Type set_time = {
            .hour = 8,
            .minutes = 58,
            .second = 10,
            .time_us = 5000,
    };
    Caven_Date_Type set_date = {
            .Days = 5,
    };
    Mode_Use.TIME.Set_Date_pFun(set_date);
    Mode_Use.TIME.Set_Watch_pFun(set_time);

//    Mode_Use.TIME.Delay_Ms(500);
}

void UART1_Getrx_Fun (u8 data)
{

}
void UART2_Getrx_Fun (u8 data)
{
    Base_UART_Send_Byte(m_UART_CH1,data);
    u2_rxd_num++;
}
void UART3_Getrx_Fun (u8 data)
{
    Base_UART_Send_Byte(m_UART_CH1,data);
    u2_rxd_num++;
}
