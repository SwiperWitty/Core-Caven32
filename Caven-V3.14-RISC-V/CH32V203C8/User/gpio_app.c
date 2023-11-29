/*
 * gpio_app.c
 *
 *  Created on: 2023年11月29日
 *      Author: gxwl
 */

#include "gpio_app.h"

Caven_info_packet_Type LED_Status_Order (Caven_info_packet_Type data)
{
    Caven_info_packet_Type retval = data;
    u8 temp_buff[20];
    int run_num = 0;
    u8 Open_ID;

    DESTROY_DATA(temp_buff,sizeof(temp_buff));
    if (data.dSize > 0)
    {
        Open_ID = temp_buff[run_num];
        switch (Open_ID)
        {
            case 1:
                if (temp_buff[run_num + 1]) {
                    Mode_Use.LED.SET_pFun(1,ENABLE);
                }
                else {
                    Mode_Use.LED.SET_pFun(1,DISABLE);
                }
                retval.Result = 0;
                break;
            default:
                retval.Result = 9;
                break;
        }
    }
    return retval;
}

Caven_info_packet_Type BZZ_Status_Order (Caven_info_packet_Type data)
{
    Caven_info_packet_Type retval = data;
    u8 temp_buff[20];
    int run_num = 0;
    u8 Open_ID;

    DESTROY_DATA(temp_buff,sizeof(temp_buff));
    if (data.dSize > 0)
    {
        memcpy(temp_buff,data.p_Data,data.dSize);
        Open_ID = temp_buff[run_num];

        switch (Open_ID)
        {
            case 1:
                if (temp_buff[run_num + 1]) {
                    Mode_Use.BZZ.SET_pFun(ENABLE);
                }
                else {
                    Mode_Use.BZZ.SET_pFun(DISABLE);
                }
                retval.Result = 0;
                break;
            default:
                retval.Result = 9;
                break;
        }
    }
    return retval;
}

Caven_info_packet_Type gpio_handle(Caven_info_packet_Type data)
{
    Caven_info_packet_Type retval;

    switch (data.Cmd_sub)
    {
        case m_GPIO_TEST_Order:
            retval = data;
            break;
        case m_GPO_SET_Order:
            break;
        case m_GPI_GET_Order:
            break;
        case m_LED_Status_Order:
            retval = LED_Status_Order(data);
            break;
        case m_BZZ_Status_Order:
            retval = BZZ_Status_Order(data);
            break;
        default:
            retval.Result = 9;
            break;
    }

    return retval;
}

