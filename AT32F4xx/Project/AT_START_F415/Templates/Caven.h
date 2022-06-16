#ifndef _CAVEN__H_
#define _CAVEN__H_

/*
    这是一个【纯C】的【.h】文件，是各个底层【.h】的 “共同语言”，上层管理函数的【.h】不需要包含
*/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define U8 unsigned char
#define S8 signed char
#define U16 unsigned short
#define S16 signed short
#define U32 unsigned long
#define S32 signed long

#define Buff_Length 1200
    
#define Destroy(X,N) memset((X),0,sizeof(N))
#define MAX(a,b)	(a>b)?a:b				//比较函数
#define MIN(a,b)	(a<b)?a:b				//比较函数

struct Caven_Data
{
    U16 Length;
    U8 Buff[Buff_Length];
    char *Pointer_8;
    int *Pointer_32;
};



#endif
