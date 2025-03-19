#include "GX_info_frame.h"

#include "Encrypt_crc.h"

/*
Caven_info_Make_packet_Fun
** make packet
** 将[data]数据转化成[packet]
传参
** standard ：标准要求(不需要指针索引)
** target   : 目标包(指针，这个包内的指针必须要有索引)
** data     ：数据来源（填入）
return   : retval
** 1.(-0x80 < retval < 0) 协议消息解析错误
** 2.retval = -0x80 包里存在协议消息没处理
** 3.retval = -0x8F 目标包的指针没有索引
** 4.retval & 0x50 >= 0 获取到协议消息,可以开始解析，同时(Result & 0x50 > 1)
** 5.retval = 其他   获取中(包含没开始retval = 0)
*/

int GX_info_return_Fun (uint8_t cmd,uint8_t MID,uint8_t addr,uint8_t *data,uint16_t len,uint8_t *array)
{
    int retval = 0;
    int temp_run = 0;
    int temp_data = 0;
    if (array == NULL || data == NULL) {
        retval = 1;
    }
    else {
        array[temp_run++] = 0X5A;

        array[temp_run++] = 0X00;
        array[temp_run++] = 0X01;
        if (addr != 0) {
            array[temp_run++] = cmd | 0x20;
        }
        else {
            array[temp_run++] = cmd;
        }
        array[temp_run++] = MID;

        if (addr != 0) {
            array[temp_run++] = addr;
        }

        array[temp_run++] = (len >> 8) & 0xff;
        array[temp_run++] = len & 0xff;
        memcpy(&array[temp_run],data,len);
        temp_run += len;

        temp_data = Encrypt_XMODEM_CRC16_Fun(&array[1], temp_run-1);
        array[temp_run++] = (temp_data >> 8) & 0xff;
        array[temp_run++] = temp_data & 0xff;

        retval = temp_run;
    }
    return retval;
}

int GX_info_Make_packet_Fun(GX_info_packet_Type const standard, GX_info_packet_Type *target, unsigned char data)
{
    int retval = 0;
    int temp = 0;
#ifdef BUFF_MAX
//    unsigned char array[BUFF_MAX];
#else
    unsigned char array[300];
#endif

    GX_info_packet_Type temp_packet = *target;
    unsigned char *tepm_pData = temp_packet.p_AllData;

    if (temp_packet.Result & 0x50) /* 目标有数据没处理 */
    {
        return (-0x80);
    }
    if (target == NULL || temp_packet.p_AllData == NULL)
    {
        return (-0x8F);
    }
    if (temp_packet.Run_status > 0 && temp_packet.Run_status < 8)   // 跳过头帧和crc校验帧
    {
        temp_packet.End_crc = CRC16_XMODEM_Table_Byte(data, temp_packet.End_crc);
    }
    switch (temp_packet.Run_status)
    {
    case 0: /* Head */
        temp_packet.Head = data;
        if (temp_packet.Head == standard.Head)
        {
            temp_packet.get_crc = 0;
            temp_packet.End_crc = 0;
            temp_packet.Get_num = 0;
            tepm_pData[temp_packet.Get_num++] = temp_packet.Head;

            temp_packet.Run_status++;
        }
        break;
    case 1: /* agreement Type */
        tepm_pData[temp_packet.Get_num++] = data;
        temp_packet.Prot_W_Type = data;
        temp_packet.Run_status++;

        break;
    case 2: /* Versions */
        tepm_pData[temp_packet.Get_num++] = data;
        temp_packet.Prot_W_Versions = data;
        if (temp_packet.Prot_W_Versions <= standard.Prot_W_Versions)
        {
            temp_packet.Run_status++;
        }
        else {
            temp_packet.Run_status = -temp_packet.Run_status;
        }
        break;
    case 3: /* 485_flag & data_flag & Class*/
        tepm_pData[temp_packet.Get_num++] = data;
        temp = data;
        temp_packet.Prot_W_485Type = (temp & (0x01 << 5)) >> 5;
        temp_packet.Prot_W_DFlag = (temp & (0x01 << 4)) >> 4;
        temp_packet.Prot_W_Class = temp & 0x0f;

        temp_packet.Run_status++;
        break;
    case 4: /* MID */
        tepm_pData[temp_packet.Get_num++] = data;
        temp_packet.Prot_W_MID = data;

        temp_packet.Run_status++;

        if (temp_packet.Prot_W_485Type == 0)
        {
            temp_packet.Addr = 0;
            temp_packet.Run_status++;
        }
        break;
    case 5: /* Addr */
        tepm_pData[temp_packet.Get_num++] = data;
        temp_packet.Addr = data;

        temp_packet.Run_status++;
        break;
    case 6: /* Size */
        tepm_pData[temp_packet.Get_num++] = data;
        temp_packet.dSize = (temp_packet.dSize << 8) + data;
        if (temp_packet.Prot_W_485Type == 0)
        {
            temp = 1 + 4 + 2;
        }
        else
        {
            temp = 1 + 4 + 1 + 2;
        }

        if (temp_packet.Get_num >= temp)
        {
            if (temp_packet.dSize > standard.dSize)
            {
                temp_packet.Run_status = -temp_packet.Run_status;
            }
            else if (temp_packet.dSize == 0)
            {
                temp_packet.Run_status += 2; /* 0个 p_AllData ，直接去 End_crc */
            }
            else
            {
                temp_packet.Run_status++;
            }
        }
        break;
    case 7: /* p_Data */
        tepm_pData[temp_packet.Get_num++] = data;
        if (temp_packet.Prot_W_485Type == 0)
        {
            temp = 1 + 4 + 2 + temp_packet.dSize;
        }
        else
        {
            temp = 1 + 4 + 1 + 2 + temp_packet.dSize;
        }

        if (temp_packet.Get_num >= temp)
        {
            temp_packet.Run_status++;
        }
        break;
    case 8: /* End_crc */
        tepm_pData[temp_packet.Get_num++] = data;
        temp_packet.get_crc = (temp_packet.get_crc << 8) + data;
        if (temp_packet.Prot_W_485Type == 0)
        {
            temp = 1 + 4 + 2 + temp_packet.dSize + 2;
        }
        else
        {
            temp = 1 + 4 + 1 + 2 + temp_packet.dSize + 2;
        }

        if (temp_packet.Get_num >= temp)
        {
            if (temp_packet.End_crc == temp_packet.get_crc)
            {
                temp_packet.Result |= 0x50; // crc successful
                temp_packet.Run_status = 0xff;
            }
            else
            {
                temp_packet.Run_status = -temp_packet.Run_status;
            }
        }
        break;
    default:
        break;
    }
    /*  结果    */
    if (temp_packet.Run_status < 0) // error
    {
        retval = temp_packet.Run_status;
        GX_info_packet_fast_clean_Fun(target);
    }
    else if (temp_packet.Run_status == 0xff) // Successful
    {
        if (temp_packet.Prot_W_485Type)
        {
            temp_packet.p_Data = temp_packet.p_AllData + 1 + 4 + 1 + 2;
        }
        else
        {
            temp_packet.p_Data = temp_packet.p_AllData + 1 + 4 + 2;
        }
        *target = temp_packet;
        retval = 0xFF;
    }
    else // doing
    {
        *target = temp_packet;
        retval = temp_packet.Run_status;
    }
    return retval;
}

/*
Caven_info_Split_packet_Fun
** split packet
** 将[packet]数据转化成[data]
传参
** source ：数据来源包(指针必须要有索引,如果里面的dSize == 0,p_Data可以为NULL)
** data     ：数据目标
return   : retval
** retval = 返回数据目标split出的长度
*/
int GX_info_rest_data_packet_Fun(GX_info_packet_Type *target, unsigned char *data,int Add_Num)
{
    int retval;
    int Offset_num;
    int temp;
    if (data == NULL || (target->p_AllData == NULL))
    {
        retval = (-1);
    }
    else
    {
        if (target->Prot_W_485Type) {
            Offset_num = 8;
        }
        else {
            Offset_num = 7;
        }
        memcpy(target->p_AllData + Offset_num,data,Add_Num);
        if (target->Get_num < 9) {
            target->Get_num = 9;
        }
        temp = target->Get_num - target->dSize;
        target->dSize = Add_Num;
        target->Get_num = temp + target->dSize;

        temp = target->dSize;
        target->p_AllData[Offset_num - 2] = ((temp >> 8) & 0xff);
        target->p_AllData[Offset_num - 1] = ((temp) & 0xff);

        Offset_num = target->Get_num - 1 - 2;
        temp = Encrypt_XMODEM_CRC16_Fun(target->p_AllData+1, Offset_num);
        Offset_num = target->Get_num;
        target->p_AllData[Offset_num - 2] = ((temp >> 8) & 0xff);
        target->p_AllData[Offset_num - 1] = ((temp) & 0xff);
        target->End_crc = temp;
    }
    return retval;
}


/*
 * 这个函数需要快速响应
 */
int GX_Circular_queue_input (GX_info_packet_Type *data,GX_info_packet_Type *Buff_data,char Start_Num,char Buff_Num)
{
    int retval = 0;

    GX_info_packet_Type temp_packet;
    for (int i = Start_Num;i < Buff_Num;i++)
    {
        temp_packet = Buff_data[i];
        if (temp_packet.Result & 0x50)
        {
            retval = (-1);
        }
        else
        {
            GX_packet_data_copy_Fun(&Buff_data[i],data);    // 载入数据到队列
//            GX_info_packet_fast_clean_Fun(data);
            retval = i;
            break;
        }
    }
    GX_info_packet_fast_clean_Fun(data);      // 无论是否能载入，都要清，否则影响下一帧接收
    return retval;
}

/*
 * retval = (-1):没有要处理的数据
 * retval = other:有
 *
 */
int GX_Circular_queue_output (GX_info_packet_Type *data,GX_info_packet_Type *Buff_data,char Buff_Num)
{
    int retval = 0;
    GX_info_packet_Type *temp_packet;
    for (int i = 0;i < Buff_Num;i++)
    {
        temp_packet = &Buff_data[i];

        if (temp_packet->Result & 0x50)
        {
            GX_packet_data_copy_Fun(data,temp_packet);    // 从队列提取数据
            GX_info_packet_fast_clean_Fun(&Buff_data[i]);
            retval = i;

            break;
        }
    }
    return retval;
}

/*
Caven_info_packet_index_Fun
** index 索引
** 将数据[data]绑定到[packet]的指针变量
传参
** target ：数据源包(这个包内有指针变量)
** data     ：要绑定的数据目标
return   : retval
** retval < 0 索引错误
** retval = 0 索引成功
*/
int GX_info_packet_index_Fun(GX_info_packet_Type *target, unsigned char *data)
{
    int retval = 0;
    if (target != NULL)
    {
        target->p_AllData = data;
    }
    return retval;
}

int GX_packet_data_copy_Fun(GX_info_packet_Type *source,GX_info_packet_Type *target)
{
    int retval = 0;
    GX_info_packet_Type temp_packet;

    if ((source != NULL) && (target != NULL))
    {
        temp_packet = *target;                  // 抽离数据
        temp_packet.p_AllData = source->p_AllData;    // 保留指针
        memcpy(temp_packet.p_AllData,target->p_AllData,target->Get_num);    // 复制指针内容,内容的长度依据是[Get_num]
        *source = temp_packet;                  // copy
    }
    return retval;
}

/*
Caven_info_packet_clean_Fun
** clean function
** 将数据[target]清除，但不解除指针绑定的数据
传参
** target ：数据源包(这个包内有指针变量)
return   : retval
** retval = 0 索引成功
*/
int GX_info_packet_clean_Fun(GX_info_packet_Type *target)
{
    int retval = 0;
    unsigned char *p_data;
    p_data = target->p_AllData;
    if (p_data != NULL && (target->Get_num > 0 && target->Get_num < BUFF_MAX))
    {
        memset(p_data, 0, target->Get_num);    // 清除指针内容,内容的长度依据是[Get_num]
    }
    GX_info_packet_fast_clean_Fun(target);
    // memset(target, 0, sizeof(GX_info_packet_Type));
    // target->p_AllData = p_data;
    return retval;
}
/*
 * 1此函数
 */
void GX_info_remove_addr (GX_info_packet_Type *target)
{
    uint8_t temp_data[500];
    int temp_dat;
    int temp_run;
    if (target->Addr != 0) {

        temp_run = 0;
        memcpy(temp_data,target->p_AllData,5);
        temp_run += 5;
        temp_data[3] &= ~0x20;      // 去标志位
        target->Prot_W_485Type = 0;
        memcpy(&temp_data[temp_run],target->p_AllData + 6,target->dSize + 2);     // 跳过地址
        temp_run += target->dSize + 2;

        temp_dat = Encrypt_XMODEM_CRC16_Fun(&temp_data[1], temp_run - 1);
        temp_data[temp_run++] = (temp_dat >> 8) & 0xff;
        temp_data[temp_run++] = temp_dat & 0xff;
        memcpy(target->p_AllData,temp_data,temp_run);
        target->Get_num = temp_run;
    }
}

void GX_info_add_addr (GX_info_packet_Type *target)
{
    uint8_t temp_data[500];
    int temp_dat;
    int temp_run;
    if (target->Addr != 0 && target->Prot_W_485Type == 0) {

        temp_run = 0;
        memcpy(temp_data,target->p_AllData,5);
        temp_run += 5;
        temp_data[3] |= 0x20;      // 标志位
        temp_data[temp_run++] = target->Addr;

        memcpy(&temp_data[temp_run],target->p_AllData + 5,target->dSize + 2);     // 跳过地址
        temp_run += target->dSize + 2;

        temp_dat = Encrypt_XMODEM_CRC16_Fun(&temp_data[1], temp_run - 1);
        temp_data[temp_run++] = (temp_dat >> 8) & 0xff;
        temp_data[temp_run++] = temp_dat & 0xff;
        memcpy(target->p_AllData,temp_data,temp_run);

        target->Get_num = temp_run;
    }
}

/*
 * fast 主要给中断，这样就不会循环套娃
 */
int GX_info_packet_fast_clean_Fun(GX_info_packet_Type *target)
{
    int retval = 0;
    target->Result = 0;
    target->Run_status = 0;
    target->dSize = 0;
    return retval;
}

