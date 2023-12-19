#ifndef _GX_INFO_FRAME__H_
#define _GX_INFO_FRAME__H_

#ifdef CAVEN    /* 预编译 */
#include "Caven_Type.h"
#else
#include "stdint.h"
#include "string.h"
#include "stddef.h"
#include "stdio.h"
#endif

/*
    Caven_info_frame.h
    描述Caven的通信协议规范
    可能需要依赖外部的crc校验函数
*/

/*
    此结构体包含指针变量，所以此结构体需要初始化其中的指针。
*/
typedef struct
{
    unsigned char Head;    //2byte

    unsigned char Prot_W_Type;      //1byte
    unsigned char Prot_W_Versions;  //1byte
    unsigned char Prot_W_485Type;   //1bit
    unsigned char Prot_W_DFlag;     //1bit
    unsigned char Prot_W_Class;     //4bit
    unsigned char Prot_W_MID;       //1byte

    unsigned char Addr;     //1byte 5

    unsigned short dSize;   //2byte 9
    unsigned char *p_Data;  // min 12byte buff

    unsigned short End_crc; //2byte

    unsigned char Result;   //1byte 
    int Run_status;
    unsigned short Get_num;

    unsigned char Comm_way;
}GX_info_packet_Type;


int GX_info_packet_clean_Fun(GX_info_packet_Type *target);
int GX_info_packet_index_Fun(GX_info_packet_Type *target, unsigned char *data);
int GX_packet_data_copy_Fun(GX_info_packet_Type *source,GX_info_packet_Type *target);

int GX_info_Make_packet_Fun(GX_info_packet_Type const standard, GX_info_packet_Type *target, unsigned char data);
int GX_info_Split_packet_Fun(GX_info_packet_Type const source, unsigned char *data);



#endif
