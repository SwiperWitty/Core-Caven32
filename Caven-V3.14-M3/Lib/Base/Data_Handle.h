#ifndef _DATA_HANDLE_H__
#define _DATA_HANDLE_H__

/*				纯C数据处理算法

	1.数据转换	Data_Replace
	2.数据判断	Data_Judge
	3.数字提取	Data_Chang_NUM


							2021.10.06

************************************/
#include "stdio.h"
#include <stdlib.h>

struct _Data
{
	void (*Replace) (char *Source,char *Target,char Data,char Length);
	char (*Judge)(char *Source,char *Target,char Length);
	float (*Chang_NUM) (char *Source,char Sign,char Length);
};

void Data_Replace (char *Source,char *Target,char Data,char Length);		// Source 源	Target 要修改目标		Data 源（单字节）	Length 长度
char Data_Judge(char *Source,char *Target,char Length);

float Data_Chang_NUM (char *Source,char Sign,char Length);

int Data_Overflow_Time(int Target,unsigned int Overflow_Time);

void Data_Init(void);
#endif
