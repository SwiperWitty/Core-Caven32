/*
 * gpio_app.c
 *
 *  Created on: 2023年11月29日
 */

#include "gpio_app.h"

static u8 buff_array[BUFF_MAX]; // buff缓冲区

Caven_info_packet_Type GPO_SET_Order(Caven_info_packet_Type data)
{
    Caven_info_packet_Type retval = data;
    int run_num = 0;
    u8 Open_ID;

    DESTROY_DATA(buff_array, sizeof(buff_array));
    if (data.dSize > 0)
    {
        Open_ID = buff_array[run_num];
        switch (Open_ID)
        {
        case 1:
            if (buff_array[run_num + 1])
            {
                Mode_Use.LED.SET_pFun(1, ENABLE);
            }
            else
            {
                Mode_Use.LED.SET_pFun(1, DISABLE);
            }
            retval.Result = RESULT_DEFAULT;
            break;
        default:
            retval.dSize = 0;
            retval.Result = 9;
            break;
        }
    }
    return retval;
}

Caven_info_packet_Type LED_Status_Order(Caven_info_packet_Type data)
{
    Caven_info_packet_Type retval = data;
    int run_num = 0;
    u8 Open_ID;
    DESTROY_DATA(buff_array, sizeof(buff_array));
    if (data.dSize > 0)
    {
        Open_ID = buff_array[run_num];
        switch (Open_ID)
        {
        case 1:
            if (buff_array[run_num + 1])
            {
                Mode_Use.LED.SET_pFun(1, ENABLE);
            }
            else
            {
                Mode_Use.LED.SET_pFun(1, DISABLE);
            }
            retval.Result = RESULT_DEFAULT;
            break;
        default:
            retval.dSize = 0;
            retval.Result = 9;
            break;
        }
    }
    return retval;
}

Caven_info_packet_Type BZZ_Status_Order(Caven_info_packet_Type data)
{
    Caven_info_packet_Type retval = data;
    int run_num = 0;
    u8 Open_ID;

    DESTROY_DATA(buff_array, sizeof(buff_array));
    if (data.dSize > 0)
    {
        memcpy(buff_array, data.p_Data, data.dSize);
        Open_ID = buff_array[run_num];

        switch (Open_ID)
        {
        case 1:
            if (buff_array[run_num + 1])
            {
                Mode_Use.BZZ.SET_pFun(ENABLE);
            }
            else
            {
                Mode_Use.BZZ.SET_pFun(DISABLE);
            }
            retval.Result = RESULT_DEFAULT;
            break;
        default:
            retval.dSize = 0;
            retval.Result = 9;
            break;
        }
    }
    return retval;
}

Caven_info_packet_Type gpio_handle(Caven_info_packet_Type data)
{
    Caven_info_packet_Type retval = data;

    switch (data.Cmd_sub)
    {
    case m_GPIO_TEST_Order:
        //            retval = data;
        retval.Result = 0;
        break;
    case m_GPO_SET_Order:
        retval = GPO_SET_Order(data);
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
        retval.dSize = 0;
        retval.Result = 5;
        break;
    }

    return retval;
}
