/* Includes ------------------------------------------------------------------*/
#include "misc.h"
#include "Base.h"
#include "Mode.h"
#include "API.h"
#include "time.h"

// HSE      外部晶振
// HSI      内部晶振

char string[100];
U8 array[30] = {1, 2, 3};

struct _Agreement Get_DATA;  //接收数据（框架）
struct _Agreement Send_DATA; //发送数据
struct _Function_ Funs;      //作为传参用的函数指针

struct _State_6016
{
    int Bound;
    int Bound_Old;
    unsigned char RFID_x;
    unsigned char RFID_x_Old;
};

struct _State_6016 NOW = { //设置初始状态
    .Bound = 115200,
    .RFID_x = 0};

void Main_Init(void);

int main(void)
{
    Main_Init();
    Base_User.Sys_Time.Delay_ms(10);
    U16 time_temp = 0;
    struct _Over_time LED_Time = {
        .Now_Time = &SYS_Watch.Watch,
        .Now_data = &time_temp,
        .Set_Time.second = 0,
        .Set_Time.time_num = 10000};

    //    Base_User.Uart.Send_String(4,"Hello world !\r\n");        //debug
    while (1)
    {
        API.DATA_Time.Over_Time(&LED_Time);
        // time_temp++;
        Mode_User.LED.Power_LED(LED_Time.Flip); //调用翻转态

        Pick_Agreement(&Uart_4.Data, &Get_DATA, &Funs, 0);
        if (Get_DATA.Flag == 'F') //开始解析(Pick通过)                 //这里只能处理一帧的target 和 要发送的数据，并不处理 source
        {
            U8 Data_temp[30];                  //back data 
            if (Get_DATA.Error.Flag) //协议有误
            {
                Data_temp[0] = Get_DATA.Error.E_Class;
                Data_temp[1] = 0x00;
                Data_temp[2] = 0x00;
                Data_temp[3] = 0x00;
                Data_temp[4] = (U8)((Get_DATA.Length >> 8) & 0x00ff);
                Data_temp[5] = (U8)(Get_DATA.Length & 0x00ff);
                Send_DATA.Length = 6;
                Send_DATA.Head = 0x5A;
                Send_DATA.pointer = Data_temp;
                Send_DATA.Con = 0x0f010000;
            }
            else
            {
                switch (Get_DATA.Control.MID) //查看上位机想做什么
                {
                case 0: //查看版本信息
                    Data_temp[0] = 0x00;
                    Data_temp[1] = 0x01;
                    Data_temp[2] = 0x00;
                    Data_temp[3] = 0x00;
                    Send_DATA.Length = 4;
                    break;
                case 1:                  //设置波特率
                    Data_temp[0] = 0x00; //成功
                    if (Get_DATA.pointer[1] == 0)
                        NOW.Bound = 9600;
                    else if (Get_DATA.pointer[1] == 1)
                        NOW.Bound = 19200;
                    else if (Get_DATA.pointer[1] == 2)
                        NOW.Bound = 115200;
                    else if (Get_DATA.pointer[1] == 3)
                        NOW.Bound = 230400;
                    else if (Get_DATA.pointer[1] == 4)
                        NOW.Bound = 460800;
                    else
                        Data_temp[0] = 0x01; //设置 波特率 失败
                    Send_DATA.Length = 1;
                    break;
                case 2: //读波特率
                    Data_temp[0] = 0x00;
                    if (NOW.Bound == 9600)
                        Data_temp[1] = 0x00;
                    else if (NOW.Bound == 19200)
                        Data_temp[1] = 0x01;
                    else if (NOW.Bound == 115200)
                        Data_temp[1] = 0x02;
                    else if (NOW.Bound == 230400)
                        Data_temp[1] = 0x03;
                    else if (NOW.Bound == 460800)
                        Data_temp[1] = 0x04;
                    Send_DATA.Length = 2;
                    break;
                case 3: //切换通道
                    NOW.RFID_x = *(Get_DATA.pointer);
                    if (NOW.RFID_x > 16 || NOW.RFID_x < 1)
                        Data_temp[0] = 0x01; //设置 通道 失败
                    else
                        Data_temp[0] = 0x00; //成功
                    Send_DATA.Length = 1;
                    break;
                default:
                    break;
                }
                Send_DATA.Head = 0x5A;
                Send_DATA.pointer = Data_temp;
                Send_DATA.Con = 0x0f010100 + Get_DATA.Control.MID;
            }
            Send_Agreement(&Send_DATA, &Funs); //回消息
            Destroy(&Get_DATA, Get_DATA);
            Destroy(&Send_DATA, Send_DATA); //销毁
        }

        if (NOW.RFID_x_Old != NOW.RFID_x)
        {
            char temp;
            NOW.RFID_x_Old = NOW.RFID_x;
            temp = 17 - NOW.RFID_x;
            Mode_User.LED.Switch(temp);
            Mode_User.RFID.Switch(temp);
        }
        if (NOW.Bound_Old != NOW.Bound)
        {
            NOW.Bound_Old = NOW.Bound;
            Base_Init.UARTx(4, NOW.Bound);
        }
    }
}

void Main_Init(void)
{
    API_Index();
    Base_Index();
    Mode_Index();

    Base_Init.Sys_time(ENABLE);
    Base_Init.UARTx(4, NOW.Bound);

    Mode_Init.LED_GPIO();
    Mode_Init.RF_GPIO();

    Funs.Send_Data = Base_User.Uart.Send_Data;
    Funs.Delay = Base_User.Sys_Time.Delay_ms;
    Funs.Over_Time = API.DATA_Time.Over_Time;
    Funs.Result_Code = API.Get_CRC16;
    Funs.Now_Time = &SYS_Watch.Watch;
    Mode_User.LED.Power_LED(DISABLE);
}
