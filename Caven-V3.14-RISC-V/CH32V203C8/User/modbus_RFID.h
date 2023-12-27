#ifndef _MODBUS_RFID__H_
#define _MODBUS_RFID__H_

#ifdef CAVEN    /* 预编译 */
#include "Caven_Type.h"
#else
#include "stdint.h"
#include "string.h"
#include "stddef.h"
#include "stdio.h"
#endif

enum MODBUS_RFID_CMD
{
    m_Readcard_Receive_Order = 3,   // 这个下面没有指定寄存器
    m_Readcard_SET_Order = 6,

};

enum MODBUS_RFID_Read_Reg_SIZE
{
    m_Readcard_Empty_Order = 0,
    m_Readcard_EPC_Orde,
    m_Readcard_TID_Order,
    m_Readcard_USER_Order,
};

/*
RTU
id   cmd  addr    len     crc
[01] [03] [00 00] [00 07] [04 08]
id   cmd  len     epc_len data
[01] [03] [0E]    [00 0c] [30 31 32 32 32 32 00 00 00 00 00 00 00 00] [8C C6]
*/

/*
TCP
       tcp   size  id  cmd  addr    len
[01 00 00 00 00 06 01] [03] [00 00] [00 07]
       tcp   size  id  cmd  len  epc_len
[01 00 00 00 00 11 01] [03] [0e] [00 0c] [11 22 33 44 dd d9 01 40 20 17 09 01]
*/
typedef struct
{
    short int num;
    unsigned char id;
    short int addr;         // register
    unsigned char cmd;
    short int size;
    short int dSize;
    unsigned char Data[100];
    short int crc;

    int Run_status;             //4byte
    unsigned char Result;       //1byte
    unsigned short Get_num;     //2byte
}modbus_Type;

typedef struct
{
    unsigned char epc_size;
    unsigned char epc_data[20];
    unsigned char tid_size;
    unsigned char tid_data[20];
    unsigned char user_size;
    unsigned char user_data[20];
    unsigned char other_size;
    unsigned char other_data[20];
    unsigned char rfid_Read_mode;
    unsigned char rfid_Work_mode;
}RFID_data_Type;


int Modbus_rtu_info_Make_packet_Fun(modbus_Type const standard, modbus_Type *target, unsigned char data);
int Modbus_rtu_info_Split_packet_Fun (modbus_Type const soure,unsigned char *data);

int Modbus_tcp_info_Make_packet_Fun(modbus_Type const standard, modbus_Type *target, unsigned char data);
int Modbus_tcp_info_Split_packet_Fun (modbus_Type const source,unsigned char *data);

int Modbus_info_packet_clean_Fun(modbus_Type *target);

int modbus_RFID_order_handle(const modbus_Type data,modbus_Type *target,RFID_data_Type *rfid_data);
int RFID_ReadCard_Center (RFID_data_Type *target);
int RFID_make_modbus (int len,RFID_data_Type source,modbus_Type *target);

#endif // !_MODBUS_RFID__H_
