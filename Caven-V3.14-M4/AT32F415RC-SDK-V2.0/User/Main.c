#include "mode.h"
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

Caven_App_Type Open_Proj = {
    .app_ID = 1,

    .p_Data = NULL,
    .string = NULL,
};

float ADC_array[8];
void Main_Init(void);
void ADC_Data_Handle (void * data);
void LCD_Show_string(char *string,char cursor,char width,char length);
int Home_app (Caven_App_Type * message);
/*
    Home_app
*/
int main(void)
{
    int retval = 0;
    Main_Init();

    while (1)
    {
        switch (Open_Proj.app_ID) 
        {
            case 1:
                Open_Proj.p_Data = NULL;
                retval = Home_app(&Open_Proj);
                break;

            default:
                break;
        }
        if (retval) {
            break;
        }
        LCD_Show_string("Cavendish  ",1,20,10);
        Mode_Use.TIME.Delay_Ms(100);
    }
}

int Home_app (Caven_App_Type * message)
{
    int retval = 0;
    
    return retval;
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
    
    Mode_Init.User_ADC(ENABLE);
	Mode_Use.USER_ADC.Receive_Bind_pFun(ADC_Data_Handle);

    while (reverse);

    Mode_Use.UART.Send_String_pFun(DEBUG_OUT, "Hello world ! \n");


#ifdef PICTURE
    Mode_Use.LCD.Show_Picture_pFun(0, 0, 240, 240, Photo2); // Photo
#endif
}

/*
string_a = " ",end is \0,so strlen(string_a) = 1.
*/
void LCD_Show_string(char *string,char cursor,char width,char length)
{
    char temp_str[50];
    char *p_str;
    int run_num = 0;
    int get_num;
    int show_point_x,show_point_y = 0;
    int color;
    if (string == NULL && width > 0 && length > 0)
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
        p_str = temp_str;
        if(temp_str[0] == '\n')
        {
            show_point_x = 0;
            show_point_y = length - 1;
            p_str = &temp_str[1];
        }
        if(show_point_y == 0)
        {
            color = LCD_BLUE;
        }
        else if(show_point_y == cursor)
        {
            color = LCD_MAGENTA;
        }
        else
        {
            color = LCD_Word_Color;
        }
        
        if(get_num > 0)
        {
            Mode_Use.LCD.Show_String_pFun(show_point_x, show_point_y, p_str, color, LCD_Back_Color, 24);
            show_point_y++;
        }
        if (show_point_y > length)
        {
            break;
        }
    } while (get_num);
}

void ADC_Data_Handle (void * data)
{
    memcpy(ADC_array,data,sizeof(ADC_array));

}

