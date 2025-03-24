
/*
 *
 */
#include "Mode.h"
#include "math.h"
#include "caven_gui.h"

void Main_Init(void);

u8 send_array[64];
int run_num;

float float_array[10];
float time_temp;

int main(void)
{
    Main_Init();
//    start_ui();
    Caven_BaseTIME_Type now_time = {
        .SYS_Sec = 1742197625,
        .SYS_Us = 0,
    };
    Mode_Use.TIME.Set_BaseTIME_pFun(now_time);
    now_time = Mode_Use.TIME.Get_BaseTIME_pFun();

    Task_Overtime_Type LED_Task = {
        .Switch = 1,
        .Begin_time = now_time,
        .Set_time.SYS_Sec = 1,
        .Set_time.SYS_Us = 5000,
        .Flip_falg = 1,
    };
    Vofa_JustFloat_Init_Fun(Debug_Out);
    float_array[1] = 3.3;

    char last_s = 0, last_m = 0, last_h = 0;
    char overstep_s = 1, overstep_m = 1, overstep_h = 1;
    int sec_point_drop, min_point_drop, hour_point_drop;
    u8 temp_num,temp_run;
    struct tm date;
    Mode_Use.LCD.Show_String_pFun(9, 0, "12", LCD_BLACK, LCD_Back_Color, 24);
    Mode_Use.LCD.Show_String_pFun(19, 5, "3", LCD_BLACK, LCD_Back_Color, 24);
    Mode_Use.LCD.Show_String_pFun(9, 9, "6", LCD_BLACK, LCD_Back_Color, 24);
    Mode_Use.LCD.Show_String_pFun(0, 5, "9", LCD_BLACK, LCD_Back_Color, 24);
    User_GPIO_config(1,0,1);
    
    while (1)
    {
        now_time = Mode_Use.TIME.Get_BaseTIME_pFun();
        date = MODE_TIME_Get_Date ();

        API_Task_Timer(&LED_Task, now_time); // LED任务
        Mode_Use.LED.SET_pFun(1, LED_Task.Flip_falg);
        
        User_GPIO_set(1,0,LED_Task.Flip_falg);
        
        float_array[0] += 0.001;
        if (float_array[0] > 2)
        {
            float_array[0] = 0;
        }
//        float_array[1] += 0.002;
        if (float_array[1] > 3)
        {
            float_array[1] = 0;
        }
        
        time_temp = date.tm_sec % 60;
        time_temp = (time_temp / 60) * 100;

        run_num = Caven_math_approximate((int)time_temp, 5, 0, 100);
        if (last_s != run_num)
        {
            last_s = run_num;
            Caven_GUI_Draw_Circle(((sec_point_drop >> 8) & 0xff), (sec_point_drop & 0xff), 1, 5, 100, LCD_Back_Color);
            run_num = Caven_GUI_Draw_Circle(120, 120, 80, 11, last_s, LCD_RED);
            sec_point_drop = run_num;
            temp_num = run_num & 0xff;
            temp_run = (run_num >> 8) & 0xff;
            if (temp_num > 0 && temp_run > 0)
            {
                Caven_GUI_Draw_Circle(temp_run, temp_num, 1, 5, 100, LCD_RED);
            }
        }
        time_temp = date.tm_min;
        time_temp = (time_temp / 60) * 100;
        run_num = Caven_math_approximate((int)time_temp, 5, 0, 100);
        if (last_m != run_num)
        {
            last_m = run_num;
            Caven_GUI_Draw_Circle(((min_point_drop >> 8) & 0xff), (min_point_drop & 0xff), 1, 5, 100, LCD_Back_Color);
            run_num = Caven_GUI_Draw_Circle(120, 120, 60, 11, last_m, LCD_BLUE);
            min_point_drop = run_num;
            temp_num = run_num & 0xff;
            temp_run = (run_num >> 8) & 0xff;
            if (temp_num > 0 && temp_run > 0)
            {
                Caven_GUI_Draw_Circle(temp_run, temp_num, 1, 5, 100, LCD_BLUE);
            }
        }
        time_temp = date.tm_hour;
        time_temp = (time_temp / 12) * 100;
        run_num = Caven_math_approximate((int)time_temp, 5, 0, 100);
        if (last_h != run_num)
        {
            last_h = run_num;
            Caven_GUI_Draw_Circle(((hour_point_drop >> 8) & 0xff), (hour_point_drop & 0xff), 1, 5, 100, LCD_Back_Color);
            run_num = Caven_GUI_Draw_Circle(120, 120, 40, 11, last_h, LCD_GREEN);
            hour_point_drop = run_num;
            temp_num = run_num & 0xff;
            temp_run = (run_num >> 8) & 0xff;
            if (temp_num > 0 && temp_run > 0)
            {
                Caven_GUI_Draw_Circle(temp_run, temp_num, 1, 5, 100, LCD_GREEN);
            }
        }

        if (overstep_s > date.tm_sec)
        {
            overstep_s = 1;
            Caven_GUI_Draw_Circle(120, 120, 80, 12, 100, LCD_Back_Color);
        }
        if (overstep_m > date.tm_min)
        {
            overstep_m = 1;
            Caven_GUI_Draw_Circle(120, 120, 60, 12, 100, LCD_Back_Color);
        }
        if (overstep_h > date.tm_hour)
        {
            overstep_h = 1;
            Caven_GUI_Draw_Circle(120, 120, 40, 12, 100, LCD_Back_Color);
        }
        Vofa_JustFloat_Show_Fun (3,float_array);
//        Mode_Use.TIME.Delay_Ms(10);
    }
}

void Main_Init(void)
{
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Mode_Index();
    Mode_Init.TIME(ENABLE);
    Mode_Init.LED(ENABLE);
    Mode_Init.UART(m_UART_CH1, 115200, ENABLE);
    Mode_Init.UART(DEBUG_OUT, 115200, ENABLE);
    Mode_Init.LCD(ENABLE);
	
	User_GPIO_config(3,13,0);	//
	User_GPIO_config(2,4,1);
	User_GPIO_config(2,5,1);
	User_GPIO_set(2,4,DISABLE);
	User_GPIO_set(2,5,DISABLE);	//kill 
	
    Caven_GUI_draw_pixel_bind (Mode_Use.LCD.Draw_Point_pFun);
    Mode_Use.UART.Send_String_pFun(m_UART_CH1,"hello world !\n");

    //    printf("SystemClk:%d \r\n", MCU_SYS_FREQ);
}

