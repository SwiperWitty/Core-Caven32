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


u32 run_num = 0;
int u2_rxd_num = 0;


void test_fun (int *num)
{
    int p = *num;
    printf("test %d \n",p);
    if (p == 0) {
        p = 1;
    }
    else {
        p = 0;
    }

    *num = p;
}

Task_Overtime_Type LED_Task = {
        .Switch = 1,
};

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

    while(1)
    {
        now_time = Mode_Use.TIME.Get_Watch_pFun();
//        printf("sys time: %d : %d : %d , %d (us)\n",now_time.hour,now_time.minutes,now_time.second,now_time.time_us);

        API_Task_Timer (&LED_Task,now_time);
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


        if(center_State_machine(now_time))
        {
            break;
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
    center_Init();

//    printf("SystemClk:%d \r\n", MCU_SYS_FREQ);

//    Mode_Use.TIME.Delay_Ms(500);
}





