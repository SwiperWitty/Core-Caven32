#ifndef _DATA_HANDLE_H__
#define _DATA_HANDLE_H__

/*				纯C#数据处理算法

	1.数据转换	Data_Replace
	2.数据判断	Data_Judge
	3.数字提取	Data_Chang_NUM


							2021.10.06

************************************/
#include "stdio.h"
#include <stdlib.h>

extern char Data_Array[20];

void Data_Replace (char *Source,char *Target,char Data,char Length);		// Source 源	Target 要被目标		Data 源（单字节）	Length 长度
char Data_Judge(char *Source,char *Target,char Length);

float Data_Chang_NUM (char *Source,char Length);

#endif
