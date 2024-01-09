/*
 * gpio_app.c
 *
 *  Created on: 2023年11月29日
 */

#include "gpio_app.h"

extern int Center_send_packet(Caven_info_packet_Type data);
static u8 buff_array[BUFF_MAX]; // buff缓冲区
static GPIO_cfg_Type s_GPIO_Config = {
        .GPO_num = 2,
        .GPI_num = 2,
        .BZZ_Keep_Time = 0,
        .GPO_Keep_Time = 0,
};

/*
 *
   * 自由GPIO上报
 */
int GPI_Change_Updata_Handle (void)
{
    int run_num = 0;
    int retval = 0;
    Caven_info_packet_Type temp_packet = {
        .Head = 0xFA8A,
        .Versions = 1,
        .Type = 3,
        .Addr = 0,
        .Cmd = m_CAVEN_FEATURE_Order,
        .Cmd_sub = m_GPI_GET_Order,
        .Comm_way = 0,           // 发给系统串口

        .Result = m_Result_Back_Empty,
    };
    for (int i = 1; i <= s_GPIO_Config.GPI_num; ++i)
    {
        switch (i)
        {
            case 1:
                s_GPIO_Config.GPI_Status_Get[i] = GPI1_STATE();
                break;
            case 2:
                s_GPIO_Config.GPI_Status_Get[i] = GPI2_STATE();
                break;
            default:
                break;
        }
        if (s_GPIO_Config.GPI_Status_Get[i] != s_GPIO_Config.GPI_Statls_Last[i])    // 发现差异
        {
            s_GPIO_Config.GPI_Statls_Last[i] = s_GPIO_Config.GPI_Status_Get[i];
            Caven_info_packet_index_Fun(&temp_packet,buff_array);
            buff_array[run_num ++] = i;
            buff_array[run_num ++] = s_GPIO_Config.GPI_Status_Get[i];
            temp_packet.dSize = run_num;
            Center_send_packet(temp_packet);
        }
    }

    return retval;
}

/*
 * 0XFF是无动作
 * 0X00是0
 * 0X01是开
 */
int GPO_Sync_Status (unsigned char *data,int len)
{
    int retval = -1;
    int GPO_state;
    if (data == NULL) {
        return retval;
    }
    for (int i = 1; i <= len; ++i)
    {
        GPO_state = data[i];
        switch (i)
        {
        case 1:
            if (GPO_state)  {GPO1_H();}
            else            {GPO1_L();}
            break;
        case 2:
            if (GPO_state)  {GPO2_H();}
            else            {GPO2_L();}
            break;
        default:
            // HC595,直接给int,长度，然后直接退出
            retval = 0;
            return retval;
        }
    }
    retval = 0;
    return retval;
}

Caven_info_packet_Type GPO_SET_Order(Caven_info_packet_Type data)
{
    Caven_info_packet_Type retval = data;
    int run_num;
    int GPO_data,GPO_state,GPO_Time;
    DESTROY_DATA(buff_array, sizeof(buff_array));
    if (data.dSize > 0 && data.dSize < sizeof(buff_array))
    {
        memcpy(buff_array, retval.p_Data, retval.dSize);
        // 保存上一次的状态
        memcpy(s_GPIO_Config.GPO_Status_Last, s_GPIO_Config.GPO_Status_Set, sizeof(s_GPIO_Config.GPO_Status_Last));
//        retval.Result = RESULT_DEFAULT;

        for (run_num = 0;run_num < data.dSize;run_num += 2)
        {
            GPO_data = buff_array[run_num] & 0xFF;
            GPO_state = buff_array[run_num + 1];

            switch (GPO_data)
            {
            case (0xFF):
                GPO_Time = buff_array[run_num + 1];
                GPO_Time <<= 8;
                GPO_Time += buff_array[run_num + 2];

                s_GPIO_Config.GPO_Keep_Time = GPO_Time;
                Caven_trigger_event_Fun(&SYS_events,GPO_event_Handle,0x01);     //触发BZZ响一次事件
                run_num ++;
                return retval;
            default:
                s_GPIO_Config.GPO_Status_Set[GPO_data] = GPO_state;             //载入
                if (GPO_state) {                                                //对操作对象镜像
                    s_GPIO_Config.GPO_Status_Last[GPO_data] = 0;
                }
                else {
                    s_GPIO_Config.GPO_Status_Last[GPO_data] = 1;
                }

                break;
            }
        }
        GPO_Sync_Status (s_GPIO_Config.GPO_Status_Set,s_GPIO_Config.GPO_num);
    }
    return retval;
}

void GPO_event_task_Fun (void * data)
{
    int temp_data = *(int *)data;
    int set_time = 0;
    static int s_once_GPO = 0;
    static Task_Overtime_Type GPO_once_Task = {
        .Switch = 1,            // 任务开关
        .Begin_time = {0},
        .Set_time.second = 0,
    };
    if (temp_data && s_GPIO_Config.GPO_Keep_Time > 0)
    {
        if (s_once_GPO == 0) {
            s_once_GPO = 1;
            GPO_once_Task.Begin_time = Real_TIME.Watch;
            set_time = s_GPIO_Config.GPO_Keep_Time * 10;    // ms
            GPO_once_Task.Set_time.minutes = ((set_time / 1000) / 60) % 60;
            GPO_once_Task.Set_time.second = (set_time / 1000) % 60;
            GPO_once_Task.Set_time.time_us = (set_time % 1000) * 1000;
            GPO_Sync_Status (s_GPIO_Config.GPO_Status_Set,s_GPIO_Config.GPO_num);
        }
        else {
            API_Task_Timer(&GPO_once_Task, Real_TIME.Watch);
        }

        if (GPO_once_Task.Trigger_Flag)
        {
            GPO_once_Task.Trigger_Flag = 0;
            s_once_GPO = 0;
            // 来到上一个操作的相反态
            GPO_Sync_Status (s_GPIO_Config.GPO_Status_Last,s_GPIO_Config.GPO_num);
            memcpy(s_GPIO_Config.GPO_Status_Set, s_GPIO_Config.GPO_Status_Last, sizeof(s_GPIO_Config.GPO_Status_Last));
            *(int *)data = 0;
        }
    }
    else {
        *(int *)data = 0;
    }
}


Caven_info_packet_Type GPI_GET_Order(Caven_info_packet_Type data)
{
    Caven_info_packet_Type retval = data;
    int run_num = 0;
    int GPI_state;
    DESTROY_DATA(buff_array, sizeof(buff_array));
//    retval.Result = m_Result_Back_Succ;     // back

    for (int i = 1;i <= s_GPIO_Config.GPI_num;i ++)
    {
        GPI_state = s_GPIO_Config.GPI_Status_Get[i];

        buff_array[run_num++] = i;
        buff_array[run_num++] = GPI_state & 0xFF;
    }
    retval.dSize = run_num;
    memcpy(retval.p_Data,buff_array,retval.dSize);      // 修改了传参[data]的p_Data /

    return retval;
}

Caven_info_packet_Type LED_SET_Order(Caven_info_packet_Type data)
{
    Caven_info_packet_Type retval = data;
    int run_num;
    int GPO_data,GPO_state;
    DESTROY_DATA(buff_array, sizeof(buff_array));
    if (data.dSize > 0 && data.dSize < sizeof(buff_array))
    {
        memcpy(buff_array, retval.p_Data, retval.dSize);
//        retval.Result = RESULT_DEFAULT;

        for (run_num = 0;run_num < data.dSize;run_num += 2)
        {
            GPO_data = buff_array[run_num] & 0xFF;
            GPO_state = buff_array[run_num + 1];

            switch (GPO_data)
            {
            case 1:
                if (GPO_state)  {LEDBLUE_L();}
                else            {LEDBLUE_H();}
                break;
            case 2:
                if (GPO_state)  {LEDWIFI_L();}
                else            {LEDWIFI_H();}
                break;
            case 3:
                if (GPO_state)  {LED_L();}
                else            {LED_H();}
                break;
            default:
                return retval;
                break;
            }
        }
    }
    return retval;
}

void LED_event_task_Fun (void * data)
{
    int temp_data = *(int *)data;
    if (temp_data) {
        LEDBLUE_L();
        LEDWIFI_L();
    }
    else {
        LEDBLUE_H();
        LEDWIFI_H();
    }
    *(int *)data = 0;   /* 清除触发标记  */
}

Caven_info_packet_Type BZZ_Status_Order(Caven_info_packet_Type data)
{
    Caven_info_packet_Type retval = data;
    int run_num;
    int GPO_data,GPO_state,GPO_Time;
    DESTROY_DATA(buff_array, sizeof(buff_array));
    if (data.dSize > 0 && data.dSize < sizeof(buff_array))
    {
        memcpy(buff_array, retval.p_Data, retval.dSize);
//        retval.Result = RESULT_DEFAULT;

        for (run_num = 0;run_num < data.dSize;run_num += 2)
        {
            GPO_data = buff_array[run_num] & 0xFF;
            GPO_state = buff_array[run_num + 1];

            switch (GPO_data)
            {
            case 1:
                if (GPO_state)  {BZZ_H();}
                else            {BZZ_L();}
                break;
            case (0xFF):
                GPO_Time = buff_array[run_num + 1];
                GPO_Time <<= 8;
                GPO_Time += buff_array[run_num + 2];

                s_GPIO_Config.BZZ_Keep_Time = GPO_Time;
                if (s_GPIO_Config.BZZ_Keep_Time > 900) {
                    s_GPIO_Config.BZZ_Keep_Time = 900;
                }
                Caven_trigger_event_Fun(&SYS_events,BZZ_event_Handle,0x01);     //触发BZZ响一次事件
                run_num ++;
                break;
            default:
                break;
            }
            if (GPO_data > 1)
            {
                return retval;
            }
        }
    }
    return retval;
}

void BZZ_event_task_Fun (void * data)
{
    int temp_data = *(int *)data;
    static int s_once_bzz = 0;
    static Task_Overtime_Type BZZ_once_Task = {
        .Switch = 1,            // 任务开关
        .Begin_time = {0},
        .Set_time.second = 0,
    };
    if (temp_data && s_GPIO_Config.BZZ_Keep_Time > 0)
    {
        if (s_once_bzz == 0) {
            s_once_bzz = 1;
            BZZ_once_Task.Begin_time = Real_TIME.Watch;
            BZZ_once_Task.Set_time.time_us = s_GPIO_Config.BZZ_Keep_Time * 1000;
            BZZ_H();
        }
        else {
            API_Task_Timer(&BZZ_once_Task, Real_TIME.Watch);
        }

        if (BZZ_once_Task.Trigger_Flag) {
            BZZ_once_Task.Trigger_Flag = 0;
            BZZ_L();
            s_once_bzz = 0;
            *(int *)data = 0;
        }
    }
    else {
        *(int *)data = 0;
    }
}

/*
 * b消息的完全转发，将Caven_info_packet_Type包里面的数据内容丢出去（极快）
 */
Caven_info_packet_Type Data_TRANSPOND_Order(Caven_info_packet_Type data)
{
    Caven_info_packet_Type retval = data;
    u8 Open_ID;
    DESTROY_DATA(buff_array, sizeof(buff_array));

    if (data.dSize > 0)
    {
        memcpy(buff_array, data.p_Data, data.dSize);
        Open_ID = buff_array[0];
        switch (Open_ID)
        {
        case 0: // 自动
//            Mode_Use.UART.Send_Data_pFun(Sys_cfg.Last_Comm, &buff_array[1], (data.dSize - 1));
//            retval.Result = RESULT_DEFAULT;
            break;
        case 1: // RS232
//            Mode_Use.UART.Send_Data_pFun(UART_RS232, &buff_array[1], (data.dSize - 1));
            MODE_UART_DMA_Send_Data_Fun(3, &buff_array[1], (data.dSize - 1));
            retval.Result = RESULT_DEFAULT;
            break;
        case 2: // RS485
            Mode_Use.UART.Send_Data_pFun(2, &buff_array[1], (data.dSize - 1));
            retval.Result = RESULT_DEFAULT;
            break;
        case 3: // weigen

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
    case m_GPIO_TRANSPOND_Order:
        retval = Data_TRANSPOND_Order(data);
        break;
    case m_GPO_SET_Order:
        retval = GPO_SET_Order(data);
        break;
    case m_GPI_GET_Order:
        retval = GPI_GET_Order(data);
        break;
    case m_LED_Status_Order:
        retval = LED_SET_Order(data);
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
