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

#define YG_DEFAULT	50
#define YG_DIF_MIN	10
#define YG_DIF_MAX	45

int temp = 0;

int yg_lock = 0; 

u16 ADC_array[10];

int Val_YG_x,Val_YG_y;

float Val_array[10];

float Val_temp_f;
     
float Val_OUT_array[10];
float Val_ELE_array[10];
float Val_TEM_array[10];
char Val_OUT_num = 0;
char Val_ELE_num = 0;
char Val_TEM_num = 0;

int show_max_x = 20;
int show_max_y = 10;
char array_buff[300];
Caven_App_Type Open_power = {
    .app_ID = 0x01,
    .p_Data = NULL,
    .string = NULL,
};
Caven_Control_Type YG_Control;
void Main_Init(void);
void LCD_Show_string(char *string,char cursor,char width,char length);
void ADC_Data_Handle (void * data);
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
    Open_power.p_Data = &YG_Control;
    while (1) 
    {
        int temp = 0,num = 0;
        char array_buff[300];
        Val_array[temp++]  = (0x0fff - ADC_array[num++]) / 40;       // 0
        Val_array[temp++] = ADC_array[num++] / 40;

        //
        Val_YG_x = Val_array[0];
        Val_YG_y = Val_array[1];
        temp = Val_YG_x - YG_DEFAULT;
        if ((temp > YG_DIF_MIN) && (yg_lock > 0))		// 左
        {
            YG_Control.Control_x = (-1);
            yg_lock = 0;
        }
        else if ((-temp > YG_DIF_MIN) && (yg_lock > 0))	// 右
        {
            YG_Control.Control_x = (1);
            yg_lock = 0;
        }
        else if(abs(temp) < YG_DIF_MIN)					// 空闲　
        {
            yg_lock++;
            yg_lock = MIN(yg_lock,100);
        }
        temp = Val_YG_y - YG_DEFAULT;
        if ((temp > YG_DIF_MIN) && (yg_lock > 0))		// 上
        {
            YG_Control.Control_y = (-1);
            yg_lock = 0;
        }
        else if ((-temp > YG_DIF_MIN) && (yg_lock > 0))	// 下
        {
            YG_Control.Control_y = (1);
            yg_lock = 0;
        }
        else if(abs(temp) < YG_DIF_MIN)					// 空闲　
        {
            yg_lock++;
            yg_lock = MIN(yg_lock,100);
        }
        
        // 
        if (YG_KEY_STATE() == 0)
        {
            YG_Control.Control_botton = 1;
            do{
                
            }while(YG_KEY_STATE() == 0);
        }
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
        if (Open_power.string != NULL && Open_power.str_switch == 1) {
            LCD_Show_string(Open_power.string,Open_power.cursor,show_max_x,show_max_y);
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
    
    Mode_Init.User_ADC(ENABLE);
	Mode_Use.USER_ADC.Receive_Bind_pFun(ADC_Data_Handle);
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
    int run_num = 0;
    static int first = 1;
    Caven_Control_Type control;

    if (message != NULL)
    {
        memcpy(&control,message->p_Data,sizeof(control));
        memset(message->string,0,50);
        if(first)
        {
            message->str_switch = 1;
            message->cursor = 0;
            message->layer = first;
            first = 0;
        }
        if(control.Control_y > 0)
        {
            message->cursor ++;
        }
        else if(control.Control_y < 0)
        {
            message->cursor --;
        }
        control.Control_y = 0;
        message->cursor = MAX(message->cursor,2);       // 第一个可选是2
        if(message->cursor > 5)
        {
            message->cursor = show_max_y -1;            // 最后一个可选是5，跳转到break 
        }
        
        sprintf(message->string,"Caven Open power");            // 0
        run_num += sizeof("Caven Open power");
        if(message->layer == 1)
        {
            sprintf(message->string + run_num," ");             // 1
            run_num += sizeof(" ");
            sprintf(message->string + run_num,"DC-DC Power");   // 2
            run_num += sizeof("DC-DC Power");
            sprintf(message->string + run_num,"Steer Control"); // 3
            run_num += sizeof("Steer Control");
            sprintf(message->string + run_num,"Games");         // 4
            run_num += sizeof("Games");
            sprintf(message->string + run_num,"About");         // 5
            run_num += sizeof("About");
            sprintf(message->string + run_num,"\nbreak[user/home]");    // end(6)

        }
    }
    
    return retval;
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
    if(ADC_array[6] > 2000)
    {
        DC_5V_OFF();
    }
}

