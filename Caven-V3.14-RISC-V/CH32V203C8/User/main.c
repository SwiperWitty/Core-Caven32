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

#include "main.h"

int main(void)
{
    Main_Init();
    while(1)
    {

    }
}

void Main_Init(void)
{
//    SystemInit() 由系统自启完成
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    LED_GPIO_Init(ENABLE);
    LED_Clr();
//    Mode_Init.TIME(ENABLE);
//    Mode_Init.UART(m_UART_CH1,115200,ENABLE);
//    printf("SystemClk:%d \r\n", MCU_SYS_FREQ);
//    Mode_Use.TIME.Delay_Ms(500);
}
