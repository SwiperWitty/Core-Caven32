#include "power_app.h"
#include "pic.h"

/*
            软件文件夹->AT32文件夹->Keil工程
项目文件->
            云端库文件夹...
*/
#ifdef PICTURE
#define Photo1 0
#define Photo2 gImage_pai
#endif
#define HOME_APP  0x01 // 主页
#define POWER_APP 0x02 // 数控电源
#define STEE_APP  0x03 // 舵机
#define GAMES_APP 0x04 // Games

int temp = 0;
char array_buff[300];
Caven_App_Type Open_power = {
    .app_ID = 0x01,
    .p_Data = NULL,
    .string = NULL,
};

void Main_Init(void);
void LCD_Show_string(char *string,char width);
int Home_app (Caven_App_Type * message);
/*
    Home_app
    Power_app
    Steering_Engine_app
    Games_app
*/
int main(void)
{
    int retval = 0;
    Main_Init();
    
    Open_power.string = array_buff;
    
    while (1) 
    {
        switch (Open_power.app_ID) 
        {
            case HOME_APP:
                retval = Home_app(&Open_power);
                break;
            case POWER_APP:
                retval = Power_app(&Open_power);
                break;
            case STEE_APP:
                retval = 0;
                break;
            case GAMES_APP:
                retval = 0;
                break;
            default:
                break;
        }
        if (Open_power.string != NULL) {
            LCD_Show_string(Open_power.string,20);
        }

        if (retval) {
            break;
        }
        //        Mode_Use.TIME.Delay_Ms(100);
    }
}

void Main_Init(void)
{
    int reverse = 0;
    system_clock_config();
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
    Mode_Index();

    Mode_Init.TIME_Init_State = Mode_Init.TIME(ENABLE);
    Mode_Init.UART_Init_State = Mode_Init.UART(DEBUG_OUT, 115200, ENABLE);
    Mode_Init.LCD_Init_State = Mode_Init.LCD(ENABLE);
    //    Mode_Init.Steering_Engine(ENABLE);

    reverse |= Power_app_init(ENABLE);
    while (reverse)
        ;

    Mode_Use.UART.Send_String_pFun(DEBUG_OUT, "Hello world ! \n");
    //	Mode_Use.Steering_Engine.Set_Angle(1,0);
    //	Mode_Use.Steering_Engine.Set_Angle(2,90);

#ifdef PICTURE
    Mode_Use.LCD.Show_Picture_pFun(0, 0, 240, 240, Photo2); // Photo
#endif
}

int Home_app (Caven_App_Type * message)
{
    int retval = 0;
    int aaa;
    if (message != NULL)
    {
        memcpy(aaa,message.p_Data,sizeof(aaa));
        
    }
    
    return retval;
}
/*
string_a = " ",end is \0,so strlen(string_a) = 1.
*/
void LCD_Show_string(char *string,char width)
{
    char temp_str[50];
    int run_num = 0;
    int get_num;
    int show_point_x,show_point_y = 0;
    if (string == NULL)
    {
        return;
    }
    do
    {
        memset(temp_str,0,sizeof(temp_str));
        get_num = strlen(&string[run_num]);
        memcpy(temp_str,&string[run_num],get_num);
        run_num += (get_num + 1);
        if(get_num > width)
        {
            show_point_x = 0;
            temp_str[width] = 0;
        }
        else
        {
            show_point_x = (width - get_num) / 2;
        }
        if(get_num > 0)
        {
            Mode_Use.LCD.Show_String_pFun(show_point_x, show_point_y++, temp_str, LCD_Word_Color, LCD_Back_Color, 24);
        }
        if (show_point_y > 10)
        {
            break;
        }
    } while (get_num);
}
