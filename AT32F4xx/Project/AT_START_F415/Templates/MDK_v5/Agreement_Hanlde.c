#include "Agreement_Hanlde.h"
#include "string.h"

int Find_Data(U8 *source,char Target,int Length)
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

char Send_Agreement(struct _Agreement *Item,struct _function *Fun)
{
    U32 temp_32 = 0;
    Item->Buff[0] = Item->Head;
    if (Item->Buff[0] != 0x5a)
        return 0;
    
    temp_32 = Item->Con;
    unsigned short i,Num;
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
    memcpy(&Item->Buff[Num],Item->pointer,Item->Length);
    i = CRC16_CCITT_CalculateBuf(Item->Buff + 1, Num + Item->Length - 1);
    Item->Buff[Num + Item->Length] = (char)((i >> 8) & 0x00ff);
    Item->Buff[Num + Item->Length + 1] = (char)(i & 0x00ff);

    Fun->Send_Data(4,Item->Buff,Num + Item->Length + 2);
    return 1;
}

char Pick_Agreement(struct _uart_x *source, struct _Agreement *Item ,struct _function *Fun,char debug)
{
    static int RX_Get_Num;
    U8 N;
    U16 i,num = 0;
    U32 temp_32 = 0;
    char string[100];
    if (source->Rxd_Number != 0)
    {
        if(source->Rxd_Data[0] == 0x5A)
        {
            if (source->Rxd_Number >= 8 && Item->Flag == 0)
            {
                Item->Head = source->Rxd_Data[0];
                for (i = 1; i <= 4; i++)
                {
                    temp_32 = temp_32 << 8;
                    temp_32 += source->Rxd_Data[i];
                }
                Item->Con = temp_32;

                if(source->Rxd_Data[3] & 0x20)           //RS485                    //长(1)
                {
                    Item->Control.RS485_Flag = 1;
                    Item->Addr = source->Rxd_Data[5];
                    Item->Length = ((int)source->Rxd_Data[6] << 8) + source->Rxd_Data[7];
                    Item->Flag = 8;
                }
                else                                                                //短(1)
                {
                    Item->Control.RS485_Flag = 0;
                    Item->Addr = 0;
                    Item->Length = ((int)source->Rxd_Data[5] << 8) + source->Rxd_Data[6];     
                    Item->Flag = 7;
                }
                if(Item->Length >= 1024)
                {
                    Item->Error.Flag = 1;
                    Item->Error.E_Class = 9;
                }
                Item->num = Item->Length + Item->Flag + 2;
                Item->Control.Model = source->Rxd_Data[1];
                Item->Control.Version = source->Rxd_Data[2];        //31-24
                Item->Control.M_or_S = (U8)(source->Rxd_Data[3] & 0X10) >> 4;       //(2)
                Item->Control.Class = (source->Rxd_Data[3] & 0X0F);                 //(3)
                Item->Control.MID = source->Rxd_Data[4];            //7-0
                Item->pointer = &Item->Buff[Item->Flag];
            }
            else if(Item->Flag != 0 && source->Rxd_Number >= Item->num)
            {
                memcpy(Item->Buff,source->Rxd_Data,source->Rxd_Number);
                Item->Check =  ((int)source->Rxd_Data[Item->Length + Item->Flag] << 8) + source->Rxd_Data[Item->Flag + Item->Length + 1];
                i = CRC16_CCITT_CalculateBuf(&(source->Rxd_Data[1]), Item->num - 3);      //除去头帧和 check位
                if (Item->Check != i)
                {
                    Item->Error.Flag = 1;       //不正常
                    Item->Error.E_Class = 3;
                    //return 0;
                }
                N = Find_Data(source->Rxd_Data + Item->num,0x5A,10);
                if (N != 0)
                {
                    source->Rxd_Number = source->Rxd_Number - Item->num;
                    memcpy(source->Rxd_Data,Item->Buff + (Item->num + N - 1),source->Rxd_Number);       //别动 !!!!
//                    string[0] = 0xff;string[1] = 0xff;
//                    Fun->Send_Data(4,string,2);
//                    Fun->Send_Data(4,source->Rxd_Data,source->Rxd_Number);
//                    Fun->Send_Data(4,string,2);
                    //对source重新赋值，重置 Rxd_Number

                }
                else
                    Destroy(source,*source);        //处理完成，销毁源
                Item->Flag = 'F';

                if(debug)
                {
                    sprintf(string,">>----debug %d----<<\r\n",debug);           //debug
                    Fun->Send_Data(4,(U8 *)string,strlen(string) + 1);
                    if (Item->Error.Flag == 1)
                    {
                        Fun->Send_Data(4,"Validation fails !\r\n",sizeof("Validation fails !\r\n"));
                        sprintf(string,"Check :%x  \r\n  \r\n",i);              //debug
                        Fun->Send_Data(4,(U8 *)string,strlen(string) + 1);
                    }
                    else
                        Fun->Send_Data(4,"Validation is successful !\r\n",sizeof("Validation is successful !\r\n"));
                    
                    sprintf(string,"%d :%x  \r\n",num++,Item->Head);            //debug
                    Fun->Send_Data(4,(U8 *)string,strlen(string) + 1);
                    sprintf(string,"%d :%x  \r\n",num++,Item->Con);             //debug
                    Fun->Send_Data(4,(U8 *)string,strlen(string) + 1);
                    sprintf(string,"%d :%x  \r\n",num++,Item->Addr);            //debug
                    Fun->Send_Data(4,(U8 *)string,strlen(string) + 1);
                    sprintf(string,"%d :%x  \r\n",num++,Item->Length);          //debug
                    Fun->Send_Data(4,(U8 *)string,strlen(string) + 1);
                    for (i = 0; i < Item->Length; i++)
                    {
                        sprintf(string,"->  %d :%x  <- \r\n",i,*(Item->pointer + i));          //debug
                        Fun->Send_Data(4,(U8 *)string,strlen(string) + 1);
                    }
                    sprintf(string,"End :%x  \r\n \r\n",Item->Check);          //debug
                    Fun->Send_Data(4,(U8 *)string,strlen(string) + 1);
                    if(debug == 2)
                    {
                        Fun->Send_Data(4,">>-- Control --<<\r\n",sizeof(">>-- Control --<<\r\n"));
                        sprintf(string,"Model :%x \r\n",Item->Control.Model);          //debug
                        Fun->Send_Data(4,(U8 *)string,strlen(string) + 1);
                        sprintf(string,"Version :%d \r\n",Item->Control.Version);          //debug
                        Fun->Send_Data(4,(U8 *)string,strlen(string) + 1);
                        sprintf(string,"RS485 :%x  \r\n",Item->Control.RS485_Flag);          //debug
                        Fun->Send_Data(4,(U8 *)string,strlen(string) + 1);
                        sprintf(string,"M_or_S :%x \r\n",Item->Control.M_or_S);          //debug
                        Fun->Send_Data(4,(U8 *)string,strlen(string) + 1);
                        sprintf(string,"Class :%x \r\n",Item->Control.Class);          //debug
                        Fun->Send_Data(4,(U8 *)string,strlen(string) + 1);
                        sprintf(string,"MID :%x \r\n",Item->Control.MID);          //debug
                        Fun->Send_Data(4,(U8 *)string,strlen(string) + 1);
                    }
                    if(debug == 3)
                    {
                        U8 array[5] = {0xff,0xff,0xff,0xff};
                        Fun->Send_Data(4,array,4);
                        Fun->Send_Data(4,source->Rxd_Data,source->Rxd_Number);          //debug
                    }
                }
                
            }
            else if (source->Rxd_Number != 0)           //存在 source->Rxd_Data[0] == 0x5A ，且不能按完全接收，且串口没有数据接收之后
            {
                if(RX_Get_Num == source->Rxd_Number)        //接收数据没有跳动
                {
                    Fun->Delay(5);
                    if (RX_Get_Num == source->Rxd_Number)   //确认没有
                    {
                        //Fun->Send_Data(4,"over time !\r\n"sizeof("over time !\r\n"));            //debug
                        Destroy(source,*source);
                        Item->Error.Flag = 1;
                        Item->Error.E_Class = 10;
                        Item->Flag = 'F';
                    }
                    else
                        RX_Get_Num = source->Rxd_Number;
                }
                else
                        RX_Get_Num = source->Rxd_Number;
            }
            return 1;
        }
        else
            Destroy(source,*source);
        return 0;
    }
    return 0;
}

