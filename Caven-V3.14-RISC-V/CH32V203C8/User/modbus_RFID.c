#include "modbus_RFID.h"

//#include "driver/uart.h"        // 特色

// x16+x15+x2+1
static int ModBusCRC16(unsigned char *data, int len)
{
    int i, j, temp, CRC16;

    CRC16 = 0xFFFF;					/* CRC寄存器初始值 */
    for (i = 0; i < len; i++)
    {
        CRC16 ^= data[i];
        for (j = 0; j < 8; j++)
        {
            temp = (int)(CRC16 & 0x0001);
            CRC16 >>= 1;
            if (temp == 1)
            {
                CRC16 ^= 0xA001;	/* 异或多项式 */
            }
        }
    }
    return CRC16;
}

int Front_to_back_2byte (int data)
{
    int retval = 0;
    retval = ((data & 0x00ff) << 8);
    retval += ((data & 0xff00) >> 8);
    return retval;
}

/*
 * 合成modbus协议
 */
int Modbus_rtu_info_Make_packet_Fun(modbus_Type const standard, modbus_Type *target, unsigned char data)
{
    int retval = 0;
    int temp_num,temp;
    modbus_Type temp_packet;
    memcpy(&temp_packet,target,sizeof(temp_packet));
    unsigned char * tepm_pData = temp_packet.Data;

    if (temp_packet.Result & 0x50) /* 目标有数据没处理 */
    {
        return (-0x80);
    }
    if (target == NULL)
    {
        return (-0x8F);
    }

    switch (temp_packet.Run_status)
    {
        case 0:                             // id
            if (standard.id == data)
            {
                temp_packet.id = data;
                tepm_pData[temp_packet.Get_num ++] = data;
                temp_packet.Run_status = 1;
            }
            break;
        case 1:                             // cmd
            {
                temp_packet.cmd = data;
                tepm_pData[temp_packet.Get_num ++] = data;
                temp_packet.Run_status ++;
            }
            break;
        case 2:                             // addr
            {
                temp_packet.addr = (temp_packet.addr << 8) + data;
                tepm_pData[temp_packet.Get_num ++] = data;
                temp_num = 2 + 2;
                if (temp_packet.Get_num >= temp_num)
                {
                    temp_packet.Run_status ++;
                }
            }
            break;
        case 3:                             // dSize
            {
                temp_packet.dSize = (temp_packet.dSize << 8) + data;
                tepm_pData[temp_packet.Get_num ++] = data;
                temp_num = 2 + 2 + 2;
                if (temp_packet.Get_num >= temp_num)
                {
                    temp_packet.dSize *= 2;         //1个数据16bit
                    temp_packet.Run_status ++;
                }
            }
            break;
        case 4:                             // CRC(低位在前)
            {
                temp_packet.crc = (temp_packet.crc << 8) + data;
                tepm_pData[temp_packet.Get_num ++] = data;
                temp_num = 2 + 2 + 2 + 2;
                if (temp_packet.Get_num >= temp_num)
                {
                    temp = ModBusCRC16(tepm_pData, temp_num - 2);
                    temp_num = Front_to_back_2byte(temp);

//                    printf("crc  get : %x, %x \n",temp_packet.crc,temp_num);
                    if (temp_packet.crc == temp_num)
                    {
                        temp_packet.Result |= 0x50;
                        temp_packet.Run_status = 0xff;
                    }
                    else
                    {
                        temp_packet.Run_status = -temp_packet.Run_status;
                    }
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
        Modbus_info_packet_clean_Fun(target);
    }
    else if (temp_packet.Run_status == 0xff) // Successful
    {
        memcpy(target,&temp_packet,sizeof(temp_packet));

        retval = temp_packet.Run_status;
        //        printf("succ %x \n",retval);
    }
    else // doing
    {
        memcpy(target,&temp_packet,sizeof(temp_packet));
        target->Result &= 0x0F;
        retval = temp_packet.Run_status;
    }
    return retval;
}

/*
 * 分解modbus协议为数组
 */
int Modbus_rtu_info_Split_packet_Fun (modbus_Type const source,unsigned char *data)
{
    int retval = 0;
    int temp,num;
    int getnum;
    if (data == NULL)
    {
        retval = (-1);
    }
    else
    {
        getnum = 0;
        data[getnum++] = source.id;
        data[getnum++] = source.cmd;

        temp = source.dSize;
        // data[getnum++] = (temp >> 8) & 0xff;
        data[getnum++] = temp & 0xff;
        if (source.dSize > 0)
        {
            memcpy(&data[getnum], source.Data, source.dSize);
            getnum += source.dSize;
        }
        temp = getnum;
        num = ModBusCRC16(data, temp);
        temp = Front_to_back_2byte(num);
        data[getnum++] = (temp >> 8) & 0xff;
        data[getnum++] = temp & 0xff;
        retval = getnum;
    }
    return retval;
}



int Modbus_tcp_info_Make_packet_Fun(modbus_Type const standard, modbus_Type *target, unsigned char data)
{
    int retval = 0;
    int temp_num;
    modbus_Type temp_packet;
    memcpy(&temp_packet,target,sizeof(temp_packet));
    unsigned char * tepm_pData = temp_packet.Data;

    if (temp_packet.Result & 0x50) /* 目标有数据没处理 */
    {
        return (-0x80);
    }
    if (target == NULL)
    {
        return (-0x8F);
    }

    switch (temp_packet.Run_status)
    {
        case 0:                             // tcp
            tepm_pData[temp_packet.Get_num ++] = data;
            temp_num = 4;
            if (temp_packet.Get_num >= temp_num)
            {
                if (tepm_pData[3] == 0 && tepm_pData[4] == 0)
                {
                    temp_packet.num = tepm_pData[0];
                    temp_packet.num = (temp_packet.num << 8) + tepm_pData[1];
                    temp_packet.num = Front_to_back_2byte(temp_packet.num);
                    temp_packet.Run_status ++;
                }
                else
                {
                    temp_packet.Run_status = -1;
                }
            }
            break;
        case 1:                             // size
            temp_packet.size = (temp_packet.size << 8) + data;
            tepm_pData[temp_packet.Get_num ++] = data;
            temp_num = 6;
            if (temp_packet.Get_num >= temp_num)
            {
                temp_packet.Run_status ++;
            }
            break;
        case 2:                             // id
            if (standard.id == data)
            {
                temp_packet.id = data;
                tepm_pData[temp_packet.Get_num ++] = data;
                temp_packet.Run_status ++;
            }
            else
            {
                temp_packet.Run_status = -temp_packet.Run_status;
            }

            break;
        case 3:                             // cmd
            temp_packet.cmd = data;
            tepm_pData[temp_packet.Get_num ++] = data;
            temp_packet.Run_status ++;

            break;
        case 4:                             // addr
            temp_packet.addr = (temp_packet.addr << 8) + data;
            tepm_pData[temp_packet.Get_num ++] = data;
            temp_num = 10;
            if (temp_packet.Get_num >= temp_num)
            {
                temp_packet.Run_status ++;
            }
            break;
        case 5:                             // len
            temp_packet.dSize = (temp_packet.dSize << 8) + data;
            tepm_pData[temp_packet.Get_num ++] = data;
            temp_num = 12;
            if (temp_packet.Get_num >= temp_num)
            {
                temp_packet.dSize *= 2;
                temp_packet.Result |= 0x50;
                temp_packet.Run_status = 0xff;
            }
            break;
        default:
            break;
    }
    if (temp_packet.Get_num > temp_packet.size + 6)     // 飞了
    {
        temp_packet.Run_status = -1;
    }
    /*  结果    */
    if (temp_packet.Run_status < 0) // error
    {
        retval = temp_packet.Run_status;
        Modbus_info_packet_clean_Fun(target);
    }
    else if (temp_packet.Run_status == 0xff) // Successful
    {
        memcpy(target,&temp_packet,sizeof(temp_packet));

        retval = temp_packet.Run_status;
        //        printf("succ %x \n",retval);
    }
    else // doing
    {
        memcpy(target,&temp_packet,sizeof(temp_packet));
        target->Result &= 0x0F;
        retval = temp_packet.Run_status;
    }
    return retval;
}

int Modbus_tcp_info_Split_packet_Fun (modbus_Type const source,unsigned char *data)
{
    int retval = 0;
    int temp;
    int getnum;
    if (data == NULL)
    {
        retval = (-1);
    }
    else
    {
        getnum = 0;
        temp = source.num;
        data[getnum++] = temp & 0xff;
        data[getnum++] = (temp >> 8) & 0xff;

        data[getnum++] = 0;         // tcp
        data[getnum++] = 0;

        temp = 1 + 1 + 1 + source.dSize;    // size
        data[getnum++] = (temp >> 8) & 0xff;
        data[getnum++] = temp & 0xff;

        data[getnum++] = source.id;
        data[getnum++] = source.cmd;

        temp = source.dSize & 0xff;
        data[getnum++] = temp;

        if (temp > 0)
        {
            memcpy(&data[getnum], source.Data, temp);
            getnum += temp;
        }

        retval = getnum;
    }
    return retval;
}


int Modbus_info_packet_clean_Fun(modbus_Type *target)
{
    int retval = 0;
    memset(target,0,sizeof(modbus_Type));
    return retval;
}


int modbus_RFID_order_handle(const modbus_Type data,modbus_Type *target,RFID_data_Type *rfid_data)
{
    int retval = 0;
    target->id = data.id;
    target->num = data.num;
    target->cmd = data.cmd;
    switch (data.cmd)
    {
    case m_Readcard_Receive_Order:
        if (data.addr == 0X00)      //
        {
            RFID_make_modbus(data.dSize,*rfid_data,target);
            memset(rfid_data,0,sizeof(RFID_data_Type));
        }
        break;
    case m_Readcard_SET_Order:

        break;

    default:
        break;
    }
    return retval;
}

int RFID_ReadCard_Center (RFID_data_Type *target)
{
    int retval = 0;
    int counter = 0;
    static int lost_falsg = 0;
    unsigned char temp_array[100];
    uint8_t send_stop_rf_order[] = {0x5A, 0x00, 0x01, 0x02, 0xFF, 0x00, 0x00, 0x88, 0x5A};
    // 5A 00 01 02 10 00 05 00 00 00 01 01 F487
    uint8_t send_Read_epc_rf_order[] = {0x5A, 0x00, 0x01, 0x02, 0x10, 0x00, 0x05, 0x00, 0x00, 0x00, 0x01, 0x01, 0xF4, 0x87};
//    uint8_t send_Read_tid_rf_order[] = {0};
//    uint8_t send_Read_user_rf_order[] = {0};
    if (lost_falsg == target->rfid_Read_mode)
    {
        return retval;
    }
    else
    {

        lost_falsg = target->rfid_Read_mode;
    }

    switch (lost_falsg)
    {
    case m_Readcard_Empty_Order:
//        uart_write_bytes(UART_NUM_1, send_stop_rf_order, sizeof(send_stop_rf_order));
        break;
    case m_Readcard_EPC_Orde:
        memcpy(temp_array,send_Read_epc_rf_order,sizeof(send_Read_epc_rf_order));
        counter = sizeof(send_Read_epc_rf_order);
        break;
    case m_Readcard_TID_Order:
        // counter = sizeof(send_Read_tid_rf_order);
        break;
    case m_Readcard_USER_Order:
        // counter = sizeof(send_Read_user_rf_order);
        break;
    default:
        break;
    }
    
    if (counter > 0)
    {
//        uart_write_bytes(UART_NUM_1, send_stop_rf_order, sizeof(send_stop_rf_order));
//        uart_write_bytes(UART_NUM_1, temp_array, counter);

    }

    retval = lost_falsg;
    return retval;
}

int RFID_make_modbus (int len,RFID_data_Type source,modbus_Type *target)
{
    int retval = 0;
    unsigned char temp_array[100];
    memset(temp_array,0,sizeof(temp_array));
    modbus_Type temp_packet;

    if (target == NULL)
    {
        retval = (-1);
    }
    memcpy(&temp_packet,target,sizeof(temp_packet));

    switch (source.rfid_Read_mode)
    {
        case 1:         // epc
            temp_packet.dSize = len;    //带上寄存器0（本标签上传长度）
            temp_array[0] = 0;
            temp_array[1] = source.epc_size & 0xff;
            memcpy(&temp_array[2],source.epc_data,temp_array[1]);
            memcpy(temp_packet.Data,temp_array,temp_packet.dSize);
            break;
        case 2:         // tid
            temp_packet.dSize = len;    //带上寄存器0（本标签上传长度）
            temp_array[0] = 0;
            temp_array[1] = source.tid_size & 0xff;
            memcpy(&temp_array[2],source.tid_data,temp_array[1]);
            memcpy(temp_packet.Data,temp_array,temp_packet.dSize);
            break;
        case 3:         // user
            temp_packet.dSize = len;    //带上寄存器0（本标签上传长度）
            temp_array[0] = 0;
            temp_array[1] = source.user_size & 0xff;
            memcpy(&temp_array[2],source.user_data,temp_array[1]);
            memcpy(temp_packet.Data,temp_array,temp_packet.dSize);
            break;
        default:
            temp_packet.dSize = len;
            memset(temp_packet.Data,0,temp_packet.dSize);
            break;
    }
    memcpy(target,&temp_packet,sizeof(temp_packet));
    return retval;
}

