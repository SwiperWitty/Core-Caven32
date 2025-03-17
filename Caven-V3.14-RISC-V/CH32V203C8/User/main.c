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
 *    主频使用96mhz，这样操作flash不需要降频，同时可以使用USB。
 *  2v1
 */
#include "main.h"

u8 send_array[64];
int run_num;

int main(void)
{
    Main_Init();

    Caven_BaseTIME_Type now_time;
    now_time = Mode_Use.TIME.Get_BaseTIME_pFun();

    Task_Overtime_Type LED_Task = {
            .Switch = 1,
            .Begin_time = now_time,
            .Set_time.SYS_Sec = 1,
            .Set_time.SYS_Us = 5000,
            .Flip_falg = 1,
    };

    while(1)
    {
        now_time = Mode_Use.TIME.Get_BaseTIME_pFun();
//        printf("sys time: %d : %d : %d , %d (us)\n",now_time.hour,now_time.minutes,now_time.second,now_time.time_us);

        API_Task_Timer (&LED_Task,now_time);        // LED任务
        User_GPIO_set(2,4,!LED_Task.Flip_falg);
        if (LED_Task.Trigger_Flag) {
            RFID_LED_L();
        }

        if(Center_State_machine(now_time))          // 状态机入口
        {
            break;                                  // 状态机退出,程序重启
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
    Mode_Use.TIME.Delay_Ms(10);

    Center_Init();
    system_init();


//    printf("SystemClk:%d \r\n", MCU_SYS_FREQ);
}





