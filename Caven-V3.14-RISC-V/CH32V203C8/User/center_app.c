/*
 * center_app.c
 *
 *  Created on: 2023年11月29日
 */

#include "center_app.h"


//volatile u64 test_cen_a,test_cen_b,test_cen_c,test_cen_d;
/*
 *  1.如果一条消息要发给RFID和ESP32，那么就要考虑是否会有消息返回，如果同时有消息返回就会有中断被占用。
 *
 */

Caven_BaseTIME_Type center_time;
int Center_State_machine(Caven_BaseTIME_Type time)
{
    int retval = 0;
    int temp_num;

    center_time = time;


    return retval;
}

