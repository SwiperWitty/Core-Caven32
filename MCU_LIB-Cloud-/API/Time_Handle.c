#include "Time_Handle.h"

//这个是 秒 级的转换，基于Caven_Watch
int Hourly_to_Seconds(struct Caven_Watch Item)
{
    int temp;
    temp = Item.second + (Item.minutes * 60) + (Item.hour * 3600); //设置的超时时差（秒级）
    temp = MIN(temp,86400);        // 60 * 60 * 24 = 86400
    return temp;
}

//这个是 秒 级的转换
struct Caven_Watch Seconds_to_Hourly(int Seconds)
{
    struct Caven_Watch temp;
    int i;
    i = Seconds / 3600;
    temp.hour = MIN(i,24);
    
    i = (Seconds % 3600) / 60;
    temp.minutes = i;
    
    i = Seconds % 60;
    temp.second = i;
    
    return temp;
}

//Over_Time函数需要时分秒、微秒
char Over_Time(struct _Over_time *Item)
{
    if (Item->last_data != *(int *)Item->Now_data) //数据在跳动
    {
        Item->last_data = *(int *)Item->Now_data;
        Item->load = 0;
        Item->Flag = 0;
    }
    else //没有跳动
    {
        if (Item->load == 0) //开始载入超时,如果数据未跳动或者未超时，那么他只载入一次
        {
            Item->load = 1;
            Item->Flag = 0;
            Item->last_Time = *Item->Now_Time;
        }
        else //超时判定
        {
            int temp_num[2] = {0};
            int temp_us = 0;
            temp_num[0] = Hourly_to_Seconds(Item->last_Time);
            temp_num[1] = Hourly_to_Seconds(*Item->Now_Time);//早算早舒服
            
            if(temp_num[0] > temp_num[1])
            {
                temp_num[1] += 24 * 3600;
            }
            
            temp_num[0] = temp_num[1] - temp_num[0];        //计算秒的差值
            temp_num[1] = Hourly_to_Seconds(Item->Set_Time);//计算设置的秒
            
            if(Item->Now_Time->time_us < Item->last_Time.time_us)
            {
                temp_num[0]--;
                temp_us = (1000000 - Item->last_Time.time_us) + Item->Now_Time->time_us;    //计算微秒差值
            }
            else
            {
                temp_us = Item->Now_Time->time_us - Item->last_Time.time_us;
            }
            
            if(temp_us < Item->Set_Time.time_us)
            {
                temp_num[0]--;
            }
            
            if ((temp_num[0] - temp_num[1]) >= 0) //超时
            {
                Item->Flag = 'p';                   //为 0 是常态，为 'p' 是瞬态
                Item->Flip = !(Item->Flip);         //为了让程序捕捉这个瞬态，引入一个翻转态
                Item->load = 0;                     //允许重装载
            }
            else                                    //没有超时
            {
                Item->Flag = 0; //为 0 是常态，为 'p' 是瞬态
            }
            
//            printf("time dif :%d s, %d us\r\n",temp_num[0],temp_us);
//            printf("time set :%d s, %d us\r\n",temp_num[1],Item->Set_Time.time_us);

        }
    }
    return Item->Flag;
}
