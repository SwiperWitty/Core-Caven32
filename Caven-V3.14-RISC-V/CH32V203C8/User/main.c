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
        Mode_Use.TIME.Delay_Ms(500);

        Mode_Use.LED.SET_pFun(1,DISABLE);
        Mode_Use.TIME.Delay_Ms(500);

//        printf("test num : %d \n",temp_num++);

        now_time = Mode_Use.TIME.Get_Watch_pFun();
//        printf("sys time: %d : %d : %d , %d (us)\n",now_time.hour,now_time.minutes,now_time.second,now_time.time_us);

        if(center_State_machine())
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

    printf("SystemClk:%d \r\n", MCU_SYS_FREQ);

//    Mode_Use.TIME.Delay_Ms(500);
}





