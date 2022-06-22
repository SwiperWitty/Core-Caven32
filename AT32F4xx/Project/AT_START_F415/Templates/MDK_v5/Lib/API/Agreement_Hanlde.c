#include "Agreement_Hanlde.h"

struct _Agreement Norm = {
    //规范
    .Head = Agreement_Head,
    .Length = Agreement_Length,
    .Control = {
        .Version = Agreement_Version,
        .Model = Agreement_Model,
    },

};

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

char Send_Agreement(struct _Agreement *Item, struct _Function_ *Fun)
{
    U32 temp_32 = 0;
    Item->Buff[0] = Item->Head;
    if (Item->Buff[0] != 0x5a)
        return 0;

    temp_32 = Item->Con;
    unsigned short i, Num;
    for (i = 4; i >= 1; i--)
    {
        Item->Buff[i] = (char)(temp_32 & 0x000000ff);
        temp_32 = temp_32 >> 8;
    }
    if (Item->Control.RS485_Flag)
    {
        Item->Buff[5] = Item->Addr;
        Item->Buff[6] = (char)(Item->Length >> 8) & 0x00ff;
        Item->Buff[7] = (char)(Item->Length & 0x00ff);
        Num = 8;
    }
    else
    {
        Item->Buff[5] = (char)(Item->Length >> 8) & 0x00ff;
        Item->Buff[6] = (char)(Item->Length & 0x00ff);
        Num = 7;
    }
    memcpy(&Item->Buff[Num], Item->pointer, Item->Length);
    i = Fun->Result_Code(Item->Buff + 1, Num + Item->Length - 1);
    Item->Buff[Num + Item->Length] = (char)((i >> 8) & 0x00ff);
    Item->Buff[Num + Item->Length + 1] = (char)(i & 0x00ff);

    Fun->Send_Data(4, Item->Buff, Num + Item->Length + 2);
    return 1;
}

// trouble1: why rm- delay the handle not doing make again ?
//比起出堆，更快的方法是直接指针偏移，但是直接出堆也有他的优势，那就是数据不易溢出（先用着）

char Pick_Agreement(struct Caven_Data *source, struct _Agreement *Item, struct _Function_ *Fun, char debug)
{
    U8 N;
    U16 i, num = 0;
    U32 temp_32 = 0;
    char string[100];
    if (source->Length != 0)
    {
        N = Find_Data(source->Buff + source->Run_num, Norm.Head, 7); //找 标准头帧
        if (N)
        {
            source->Run_num += (N - 1);
            static struct _Over_time temp_sou;
            temp_sou.Now_data = &source->Length;
            temp_sou.Now_Time = Fun->Now_Time;
            if (Item->Flag == 0 && (source->Length - source->Run_num) >= 8) // 允许解析【Con】，(前8个数据已经拿到)
            {
                Destroy(Item, *Item);                                       //解析之前保持数据纯净
                if (debug)
                {
                    Fun->Send_Data(4, "in 1 ", sizeof("in 1")); // debug
                }
                Item->Head = source->Buff[source->Run_num];
                for (i = (source->Run_num + 1); i <= (source->Run_num + 4); i++)
                {
                    temp_32 = temp_32 << 8;
                    temp_32 += source->Buff[i];
                }
                Item->Con = temp_32;

                if (Item->Con & 0x00002000) // RS485                    //长(1)
                {
                    Item->Control.RS485_Flag = 1;
                    Item->Addr = source->Buff[source->Run_num + 5];
                    Item->Length = ((int)source->Buff[source->Run_num + 6] << 8) + source->Buff[source->Run_num + 7];
                    Item->Flag = 8;
                }
                else //短(1)
                {
                    Item->Control.RS485_Flag = 0;
                    Item->Addr = 0;
                    Item->Length = ((int)source->Buff[source->Run_num + 5] << 8) + source->Buff[source->Run_num + 6];
                    Item->Flag = 7;
                }
                if (Item->Length >= Norm.Length)
                {
                    Item->Error.Flag = 1;
                    Item->Error.E_Class = 9;
                }
                Item->num = Item->Length + Item->Flag + 2; //本帧数据长度，也是本次处理的长度
                Item->Control.Model = source->Buff[source->Run_num + 1];
                Item->Control.Version = source->Buff[source->Run_num + 2];                  // 31-24（bit）
                Item->Control.M_or_S = (U8)(source->Buff[source->Run_num + 3] & 0X10) >> 4; //(2)
                Item->Control.Class = (source->Buff[source->Run_num + 3] & 0X0F);           //(3)
                Item->Control.MID = source->Buff[source->Run_num + 4];                      // 7-0 （bit）
                Item->pointer = &source->Buff[source->Run_num + Item->Flag];
            }
            else if (Item->Flag != 0 && (source->Length - source->Run_num) >= Item->num) // 【Con】解析完成，Item->num 这是完整一帧的长度
            {
                if (debug)
                {
                    Fun->Send_Data(4, "in 2 ", sizeof("in 1")); // debug
                }
                Item->Check = ((int)source->Buff[source->Run_num + Item->num - 2] << 8) + source->Buff[source->Run_num + Item->num - 1]; //本帧最后两位【校验位】
                i = Fun->Result_Code(source->Buff + source->Run_num + 1, Item->num - 3);                                                 //除去头帧(1) 和 check位(2)
                if (Item->Check != i)
                {
                    Item->Error.Flag = 1; //不正常
                    Item->Error.E_Class = 3;
                    // return 0;
                }
                if (source->Buff[source->Run_num + 1] == Norm.Control.Model) //消息属于 当前设计
                {
                    Item->Flag = 'F';
                }
                source->Run_num += Item->num; //下一帧起点

                if (debug)
                {
                    sprintf(string, ">>----debug %d----<<\r\n", debug); // debug
                    Fun->Send_Data(4, (U8 *)string, strlen(string) + 1);
                    if (Item->Error.Flag == 1)
                    {
                        Fun->Send_Data(4, "Validation fails !\r\n", sizeof("Validation fails !\r\n"));
                        sprintf(string, "Check :%x  \r\n  \r\n", i); // debug
                        Fun->Send_Data(4, (U8 *)string, strlen(string) + 1);
                    }
                    else
                        Fun->Send_Data(4, "Validation is successful !\r\n", sizeof("Validation is successful !\r\n"));
                    num = 0;
                    sprintf(string, "%d :%x  \r\n", num++, Item->Head); // debug
                    Fun->Send_Data(4, (U8 *)string, strlen(string) + 1);
                    sprintf(string, "%d :%x  \r\n", num++, Item->Con); // debug
                    Fun->Send_Data(4, (U8 *)string, strlen(string) + 1);
                    sprintf(string, "%d :%x  \r\n", num++, Item->Addr); // debug
                    Fun->Send_Data(4, (U8 *)string, strlen(string) + 1);
                    sprintf(string, "%d :%x  \r\n", num++, Item->Length); // debug
                    Fun->Send_Data(4, (U8 *)string, strlen(string) + 1);
                    for (i = 0; i < Item->Length; i++)
                    {
                        sprintf(string, "->  %d :%x  <- \r\n", i, *(Item->pointer + i)); // debug
                        Fun->Send_Data(4, (U8 *)string, strlen(string) + 1);
                    }
                    sprintf(string, "End :%x  \r\n \r\n", Item->Check); // debug
                    Fun->Send_Data(4, (U8 *)string, strlen(string) + 1);
                    if (debug == 2)
                    {
                        Fun->Send_Data(4, ">>-- Control --<<\r\n", sizeof(">>-- Control --<<\r\n"));
                        sprintf(string, "Model :%x \r\n", Item->Control.Model); // debug
                        Fun->Send_Data(4, (U8 *)string, strlen(string) + 1);
                        sprintf(string, "Version :%d \r\n", Item->Control.Version); // debug
                        Fun->Send_Data(4, (U8 *)string, strlen(string) + 1);
                        sprintf(string, "RS485 :%x  \r\n", Item->Control.RS485_Flag); // debug
                        Fun->Send_Data(4, (U8 *)string, strlen(string) + 1);
                        sprintf(string, "M_or_S :%x \r\n", Item->Control.M_or_S); // debug
                        Fun->Send_Data(4, (U8 *)string, strlen(string) + 1);
                        sprintf(string, "Class :%x \r\n", Item->Control.Class); // debug
                        Fun->Send_Data(4, (U8 *)string, strlen(string) + 1);
                        sprintf(string, "MID :%x \r\n", Item->Control.MID); // debug
                        Fun->Send_Data(4, (U8 *)string, strlen(string) + 1);
                    }
                    if (debug == 3)
                    {
                        U8 array[5] = {0xff, 0xff, 0xff, 0xff};
                        Fun->Send_Data(4, array, 4);
                        Fun->Send_Data(4, source->Buff, source->Length); // debug
                    }
                }
            }
            else //以上条件不满足,即可查看串口是否还有串口数据跳动(发一半卡死)
            {
                temp_sou.Set_Time = (struct Caven_Watch){
                    .second = 2};
                Fun->Over_Time(&temp_sou);
                if (temp_sou.Flag) //数据超时
                {
                    if (debug)
                    {
                        Fun->Send_Data(4, "in 3 ", sizeof("in 1")); // debug
                    }
                    if ((source->Length - source->Run_num) >= 7)    //虽然前面的条件不符合，标志位停了，但是还要大量数据未处理
                    {
                        source->Run_num++;
                        Destroy(Item, *Item);                       //清除上次数据
                    }
                    else
                    {
                        if (debug)
                        {
                            Fun->Send_Data(4, "in 4 ", sizeof("in 1")); // debug
                        }
                        if (source->Buff[source->Run_num + 1] == Norm.Control.Model) //消息属于 当前设计
                        {
                            Item->Error.Flag = 1;
                            Item->Error.E_Class = 10;
                            Item->Flag = 'F';
                        }
                        temp_sou.Flag = 0;
                        Destroy(source, *source);
                    }
                }
            }
            return 1;
        }
        else
            Destroy(source, *source);
        return 0;
    }
    return 0;
}
