#ifndef _API__H_
#define _API__H_

#include "Agreement_Hanlde.h"
#include "Check_CRC16.h"

struct _API_Data
{
    U16 (*Get_CRC16)(U8 *ptr, U16 len);
    struct _Agreement_Handle Agreement;
};

extern struct _API_Data API;

//这个文件是不需要【初始化结构体】的

void API_Index(void);

#endif
