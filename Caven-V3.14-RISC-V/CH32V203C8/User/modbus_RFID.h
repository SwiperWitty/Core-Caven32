#ifndef _MODBUS_RFID__H_
#define _MODBUS_RFID__H_

#include "stdint.h"
#include "string.h"

enum MODBUS_RFID_CMD
{
    m_Readcard_EPC_Order = 3,
    m_Readcard_TID_Order = 6,
    m_Readcard_USER_Order = 9,

    m_Writecard_EPC_Order = 1,
    m_Writecard_USER_Order = 2,

};

typedef struct 
{
    unsigned char addr;
    unsigned char cmd;
    unsigned char *p_data;
    short int crc;

    unsigned char Run_status;   //1byte
    unsigned char Result;       //1byte
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
}RFID_data_Type;

typedef void (*Send_pFun) (void *data, int len);


int Modbus_rtu_info_Make_packet_Fun(modbus_Type const standard, modbus_Type *target, unsigned char data);
int Modbus_rtu_info_Split_packet_Fun (modbus_Type const soure,unsigned char *data);

int RFID_make_modbus (RFID_data_Type source,modbus_Type target);

#endif // !_MODBUS_RFID__H_
