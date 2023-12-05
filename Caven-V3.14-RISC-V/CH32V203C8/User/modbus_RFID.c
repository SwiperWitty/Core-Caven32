#include "modbus_RFID.h"
#include <stdio.h>

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

int Modbus_rtu_info_Make_packet_Fun(modbus_Type const standard, modbus_Type *target, unsigned char data)
{
    int retval = 0;
    modbus_Type temp_data = *target;
    unsigned char * tepm_pData = temp_data.p_data;

    if (temp_data.Result == 0 && temp_data.p_data != NULL)
    {
        if (standard.addr == data && temp_data.Run_status == 0)
        {
            temp_data.addr = data;
            temp_data.Run_status = 1;
        }
        else if (temp_data.Run_status == 1)
        {
            temp_data.cmd = data;
            temp_data.Run_status = 2;
        }
        else if (temp_data.Run_status == 2)
        {
            temp_data.cmd = data;
            temp_data.Run_status = 2;

            *(tepm_pData+0) = data;
        }
    }

    return retval;
}

/*
 * 分解modbus协议为数组
 */
int Modbus_rtu_info_Split_packet_Fun (modbus_Type const soure,unsigned char *data)
{
    int retval = 0;
    unsigned char buff_array[300];
    int tx_size = 0;
    int temp;
    memset(buff_array,0,sizeof(buff_array));
    buff_array[0] = soure.addr;
    buff_array[1] = soure.cmd;
    if (soure.p_data != NULL && data != NULL)
    {
        buff_array[2] = *(soure.p_data + 0);
        buff_array[3] = *(soure.p_data + 1);
        tx_size = buff_array[2];
        tx_size = tx_size << 8;
        tx_size += buff_array[3];
        if (tx_size > 0) {
            memcpy(&buff_array[4],(soure.p_data + 2),tx_size);
        }
        else if (tx_size > 100){
            retval = -1;
            return retval;
        }
        temp = ModBusCRC16(buff_array, tx_size + 4);

        buff_array[tx_size + 4] = (temp & 0x00ff);
        buff_array[tx_size + 5] = (temp & 0xff00) >> 8;
//        printf("crc : %x ,[%x,%x] ; size %d \n",data.crc,buff_array[tx_size + 4],buff_array[tx_size + 5],tx_size);

        retval = tx_size + 4 + 2;
        memcpy(data,buff_array,retval);

    }
    return retval;
}


