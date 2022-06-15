#ifndef _Agreement_Hanlde__H_
#define _Agreement_Hanlde__H_

#include "Check_CRC16.h"
#include "Mode.h"

#define Destroy(X,N) memset((X),0,sizeof(N))

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

struct _function
{
    void (*Send_Data)(char Channel,const unsigned char * Data,int Length);
    void (*Delay)(int );

};


char Pick_Agreement(struct _uart_x *source, struct _Agreement *Item ,struct _function *Fun,char debug);
char Send_Agreement(struct _Agreement *Item,struct _function *OUT);
#endif
