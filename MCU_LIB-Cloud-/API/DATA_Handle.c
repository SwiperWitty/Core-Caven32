#include "DATA_Handle.h"

int Find_Data(const U8 *source, char Target, int Length)
{
    for (size_t i = 0; i < Length;)
    {
        if (Target == source[i++])
        {
            return i;
        }
    }
    return 0;
}

char Data_Chang_NUM (struct _Chang_Num *Item)
{
    char i,n,m = 0;
    char flag = 1;                            //i是当前找的是 第几个 Sign
	char Data_Array[12] = {0};                  //数字的str数
    char str[50];                               //装source数据到这个缓存区，这个缓存区是可变的
    char Find_Time = strlen(Item->Sign);        //要找多少个
    char Length = strlen(Item->Source);         //源的长度
    strcpy(str,Item->Source);
    if(Length > 50)
        Length = 50;
    for (i = 0; i < Find_Time; i++)
    {
        flag = 0;
        for(n = 0;n < Length;n++)
        {
            if(str[n] == Item->Sign[i])             //找到标志
            {
                str[n] = 0;
                flag = 1;                           //开始识别str
                m = 0;
            }
            if(flag)
            {
                if((str[n] <= '9' && str[n] >= '0') || str[n] == '.' || str[n] == '-')
                {
                    Data_Array[m] = str[n];         //把数字相关str的装起来
                    m++;                            //m是数字在str的长度
                }
                else
                {
                    if(m > 0)                       //数字部分结束
                    {
                        if(Data_Array[m-1] == '.' || Data_Array[m-1] == '-')    //不让最后一位为 '.' and '-'
                            Data_Array[m-1] = '\0';                             //防止 atof()函数出错
                        else
                            Data_Array[m] = '\0';
                        break;
                    }
                }
            }
        
        }
        Item->NUM[i] = atof(Data_Array);
        Item->flag = i;
    }
    // printf(" \n");
    return Item->flag;
}
