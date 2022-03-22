#include "Data_Handle.h"

char Data_Array[20];

float Data_Chang_NUM (char *Source,char Length)
{
	char n,Data_S;
	char j = 0;
	float i;
	for(n = 0;n < Length;n++)
	{
		Data_S = *(Source + n);
		if((Data_S <= '9' && Data_S >= '0') || Data_S == '.' || Data_S == '-')
		{
			*(Data_Array + j) = Data_S;
			j++;
		}
		else
		{
			if(j > 0)	break;
		}
	}
	i = atof(Data_Array);
	Data_Replace ("123",Data_Array,0,sizeof(Data_Array));
	return i;
}

//atof() 用于将字符串转换为单精度浮点数(float)
//atoi() 函数用来将字符串转换成整数(int)
//strtod() 用来将字符串转换成双精度浮点数(double)

void Data_Replace (char *Source,char *Target,char Data,char Length)
{
	int n;
	for(n = 0;n < Length;n++)
	{
		if(Data == 'F')
		{
			*(Target + n) = *(Source + n);
		}
		else 
		{
			*(Target + n) = Data;
		}
	}
}
//strcpy 从另一个地址copy数据


char Data_Judge(char *Source,char *Target,char Length)
{
	char n,Data_S,Data_T;
	for(n = 0;n < Length - 1;n++)
	{
		Data_S = *(Source + n);
		Data_T = *(Target + n);
		if(Data_T == Data_S)
		{
			
		}
		else 
		{
			return 0;
		}
	}
	return n;
}
//strcmp()函数：比较两个字符串
