#ifndef _Agreement_Hanlde__H_
#define _Agreement_Hanlde__H_

#include "Check_CRC16.h"
#include "Caven.h"

struct _Protocol
{
    unsigned char Model;
    unsigned char Version;
    unsigned char RS485_Flag;
    unsigned char M_or_S;
    unsigned char Class;
    unsigned char MID;
};

struct _Error
{
    unsigned char E_Class;

    unsigned char Flag;             //if the Flag != 0，Error is true

    unsigned char State;            // 0 free 1 doing 2 error

    unsigned short Get_Con;
    unsigned short Get_Len;
};

struct _Agreement
{
    unsigned char Head;
    signed int Con;
    unsigned char Addr;
    unsigned short Length;
    unsigned char *pointer;
    unsigned short Check;

    unsigned char Buff[1124];
    unsigned char Flag;                 //完成协议解析为 'F'，0是允许解析协议
    int num;                            //这段数据的具体长度
    struct _Protocol Control;
    struct _Error Error;
    char Debug;
};

struct _Function
{
    void (*Send_Data)(char Channel,const unsigned char * Data,int Length);
    void (*Delay)(int );

};

struct _Agreement_Handle
{
    int (*Find_Data)(U8 *source,char Target,int Length);
    char (*Pick)(struct Caven_Data *source, struct _Agreement *Item ,struct _Function *Fun,char debug);
    char (*Send)(struct _Agreement *Item,struct _Function *OUT);
};


int Find_Data(U8 *source,char Target,int Length);
char Pick_Agreement(struct Caven_Data *source, struct _Agreement *Item ,struct _Function *Fun,char debug);
char Send_Agreement(struct _Agreement *Item,struct _Function *OUT);
#endif
