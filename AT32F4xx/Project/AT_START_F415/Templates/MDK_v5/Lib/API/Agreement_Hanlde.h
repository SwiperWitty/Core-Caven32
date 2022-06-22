#ifndef _Agreement_Hanlde__H_
#define _Agreement_Hanlde__H_

#include "DATA.h"
#include "Caven.h"

#define Agreement_Head 0x5A     //帧头
#define Agreement_Model 0x0F    //消息属于集线器
#define Agreement_Version 0x01  //限定版本
#define Agreement_Length 1024   //最长字节

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

    unsigned char Flag; // if the Flag != 0，Error is true

    unsigned char State; // 0 free 1 doing 2 error

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

    unsigned char Buff[Agreement_Length + 8];
    unsigned char Flag; //完成协议解析为 'F'，0是允许解析协议
    int num;            //这段数据的具体长度
    struct _Protocol Control;
    struct _Error Error;
    char Debug;
};

extern struct _Agreement Norm;

struct _Function_
{
    void (*Send_Data)(char Channel, const unsigned char *Data, int Length);
    U16 (*Result_Code)
    (unsigned char *ptr, U16 len);
    void (*Delay)(int);
    char (*Over_Time)(struct _Over_time *Item);

    const struct Caven_Watch *Now_Time;
};

struct Agreement_Handle_
{
    int (*Find_Data)(const U8 *source, char Target, int Length);
    char (*Pick)(struct Caven_Data *source, struct _Agreement *Item, struct _Function_ *Fun, char debug);
    char (*Send)(struct _Agreement *Item, struct _Function_ *OUT);
};

//函数目录
int Find_Data(const U8 *source, char Target, int Length);
char Pick_Agreement(struct Caven_Data *source, struct _Agreement *Item, struct _Function_ *Fun, char debug);
char Send_Agreement(struct _Agreement *Item, struct _Function_ *OUT);
#endif
