/*
 * gpio_app.c
 *
 *  Created on: 2023年11月29日
 */

#include "gpio_app.h"

static u8 buff_array[BUFF_MAX]; // buff缓冲区
static GPIO_cfg_Type s_GPIO_Config = {
        .GPO_num = 2,
        .GPI_num = 2,

};


Caven_info_packet_Type GPO_SET_Order(Caven_info_packet_Type data)
{
    Caven_info_packet_Type retval = data;
    int run_num;
    int GPO_data,GPO_state,GPO_Time;
    DESTROY_DATA(buff_array, sizeof(buff_array));
    if (data.dSize > 0 && data.dSize < sizeof(buff_array))
    {
        memcpy(buff_array, retval.p_Data, retval.dSize);
        retval.Result = RESULT_DEFAULT;

        for (run_num = 0;run_num < data.dSize;run_num += 2)
        {
            GPO_data = buff_array[run_num] & 0xFF;
            GPO_state = buff_array[run_num + 1];

            switch (GPO_data)
            {
            case 1:
                if (GPO_state)  {GPO1_H();}
                else        {GPO1_L();}
                break;
            case 2:
                if (GPO_state)  {GPO2_H();}
                else        {GPO2_L();}
                break;
            case (0xFF):
                GPO_Time = buff_array[run_num + 1];
                GPO_Time <<= 8;
                GPO_Time += buff_array[run_num + 2];

                s_GPIO_Config.GPO_Keep_Time = GPO_Time;
                run_num ++;
                break;
            default:
                break;
            }
            if (GPO_data > s_GPIO_Config.GPO_num)
            {
                return retval;
            }
        }
    }
    return retval;
}

Caven_info_packet_Type GPI_GET_Order(Caven_info_packet_Type data)
{
    Caven_info_packet_Type retval = data;
    int run_num = 0;
    int GPI_state;
    DESTROY_DATA(buff_array, sizeof(buff_array));
    retval.Result = m_Result_Back_Succ;     // back

    for (int i = 1;i <= s_GPIO_Config.GPI_num;i ++)
    {
        GPI_state = 0;
        switch (i) {
            case 1:
                GPI_state = GPI1_STATE();
                break;
            case 2:
                GPI_state = GPI2_STATE();
                break;
            case 3:
                break;
            case 4:
                break;
            default:
                break;
        }
        buff_array[run_num++] = i;
        buff_array[run_num++] = GPI_state & 0xFF;
    }
    retval.dSize = run_num;
    memcpy(retval.p_Data,buff_array,retval.dSize);      // 修改了传参[data]的p_Data /

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

    }
    return retval;
}

void LED_BULE_event_task_Fun (void * data)
{
    int temp_data = *(int *)data;
    if (temp_data) {
        LEDBLUE_L();
    }
    else {
        LEDBLUE_H();
    }
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
            retval.Result = m_Result_Back_Other;
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
        retval.Result = m_Result_Back_Succ;
        break;
    case m_GPO_SET_Order:
        retval = GPO_SET_Order(data);
        break;
    case m_GPI_GET_Order:
        retval = GPI_GET_Order(data);
        break;
    case m_LED_Status_Order:
        retval = LED_Status_Order(data);
        break;
    case m_BZZ_Status_Order:
        retval = BZZ_Status_Order(data);
        break;

    default:
        retval.dSize = 0;
        retval.Result = m_Result_Back_CMDS;
        break;
    }

    return retval;
}
