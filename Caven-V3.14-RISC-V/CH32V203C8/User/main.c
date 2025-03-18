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
 */
#include "main.h"

u8 send_array[64];
int run_num;

int main(void)
{
    Caven_BaseTIME_Type now_time;
    Main_Init();
    now_time.SYS_Sec = 1742299486;
    Mode_Use.TIME.Set_BaseTIME_pFun(now_time);
    struct tm date = Mode_Use.TIME.Get_Date_pFun();
    now_time = Mode_Use.TIME.Get_BaseTIME_pFun();

    Task_Overtime_Type LED_Task = {
            .Switch = 1,
            .Begin_time = now_time,
            .Set_time.SYS_Sec = 1,
            .Set_time.SYS_Us = 500000,
            .Flip_falg = 0,
    };
    User_GPIO_config(2,4,1);
    while(1)
    {
        now_time = Mode_Use.TIME.Get_BaseTIME_pFun();
        date = Mode_Use.TIME.Get_Date_pFun();
        date.tm_hour += 8;          // 加上 8 小时
        mktime(&date);              // 规范化时间（处理溢出，例如跨天）
        API_Task_Timer (&LED_Task,now_time);        // LED任务
        User_GPIO_set(2,4,LED_Task.Flip_falg);
        if (LED_Task.Trigger_Flag) {
            printf("utc [%d] date %d/%d/%d %02d:%02d:%02d  \n",now_time.SYS_Sec,date.tm_year,date.tm_mon,date.tm_mday,
                    date.tm_hour,date.tm_min,date.tm_sec);
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

    Mode_Init.UART(1,115200,ENABLE);
    printf("SystemClk:%d \r\n", MCU_SYS_FREQ);
}
