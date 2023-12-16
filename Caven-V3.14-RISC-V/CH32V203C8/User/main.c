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
#include "modbus_RFID.h"

u32 run_num = 0;
int u2_rxd_num = 0;
u8 array[64];
u8 send_array[64];

void send_modbus (void *data,int length)
{
    Mode_Use.UART.Send_Data_pFun(UART_SYS,(u8 *)data,length);
}

int main(void)
{
    Main_Init();

    Caven_Watch_Type now_time;
    now_time = Mode_Use.TIME.Get_Watch_pFun();

    Task_Overtime_Type LED_Task = {
            .Switch = 1,
            .Begin_time = now_time,
            .Set_time.second = 1,
            .Set_time.time_us = 500000,
    };

    array[0] = 0;
    array[1] = 6;
    modbus_Type test_data = {
            .addr = 0x01,
            .cmd = 0x03,
            .p_data = array,
    };

    run_num = Modbus_rtu_info_Split_packet_Fun (test_data,send_array);
    Mode_Use.UART.Send_Data_pFun(UART_SYS,send_array,run_num);
    run_num = 0;
    while(1)
    {
        now_time = Mode_Use.TIME.Get_Watch_pFun();
//        printf("sys time: %d : %d : %d , %d (us)\n",now_time.hour,now_time.minutes,now_time.second,now_time.time_us);

        API_Task_Timer (&LED_Task,now_time);        // LED任务
        if (LED_Task.Trigger_Flag)
        {
            run_num++;
            if (run_num % 2) {
                Mode_Use.LED.SET_pFun(1,ENABLE);
            }
            else {
                Mode_Use.LED.SET_pFun(1,DISABLE);
            }
        }


        if(Center_State_machine(now_time))          // 状态机入口
        {
            break;          // 状态机退出,程序重启
        }

    }
    SYS_RESET();
}


void Main_Init(void)
{
//    SystemInit() 由系统自启完成
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Mode_Index();
    Mode_Init.TIME(ENABLE);

    system_init();
    Center_Init();

//    printf("SystemClk:%d \r\n", MCU_SYS_FREQ);
//    Mode_Use.TIME.Delay_Ms(500);
}





