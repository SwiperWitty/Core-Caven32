
/*
 *
 */
#include "main.h"
//#include "GuiLite.h"

u8 send_array[64];
int run_num;

float float_array[10];
float time_temp;

int start_ui(void);
int main(void)
{
    Main_Init();
    start_ui();
    Caven_Watch_Type now_time = {
        .hour = 8,
        .minutes = 7,
        .second = 50,
    };
    Mode_Use.TIME.Set_Watch_pFun(now_time);
    now_time = Mode_Use.TIME.Get_Watch_pFun();

    Task_Overtime_Type LED_Task = {
        .Switch = 1,
        .Begin_time = now_time,
        .Set_time.second = 1,
        .Set_time.time_us = 5000,
        .Flip_falg = 1,
    };
    Vofa_JustFloat_Init_Fun(2, Debug_Out);
    float_array[1] = 3.3;

    char last_s = 0, last_m = 0, last_h = 0;
    char overstep_s = 1, overstep_m = 1, overstep_h = 1;
    int sec_point_drop, min_point_drop, hour_point_drop;
    u8 temp_num,temp_run;

    Mode_Use.LCD.Show_String_pFun(9, 0, "12", LCD_BLACK, LCD_Back_Color, 24);
    Mode_Use.LCD.Show_String_pFun(19, 5, "3", LCD_BLACK, LCD_Back_Color, 24);
    Mode_Use.LCD.Show_String_pFun(9, 9, "6", LCD_BLACK, LCD_Back_Color, 24);
    Mode_Use.LCD.Show_String_pFun(0, 5, "9", LCD_BLACK, LCD_Back_Color, 24);
    while (1)
    {
        now_time = Mode_Use.TIME.Get_Watch_pFun();
        //        printf("sys time: %d : %d : %d , %d (us)\n",now_time.hour,now_time.minutes,now_time.second,now_time.time_us);
        API_Task_Timer(&LED_Task, now_time); // LED任务
        Mode_Use.LED.SET_pFun(1, LED_Task.Flip_falg);

        float_array[0] += 1.133;
        if (float_array[0] > 10)
        {
            float_array[0] = 0;
        }

        time_temp = now_time.second;
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
        time_temp = now_time.minutes;
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
        time_temp = now_time.hour;
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

        if (overstep_s > now_time.second)
        {
            overstep_s = 1;
            Caven_GUI_Draw_Circle(120, 120, 80, 12, 100, LCD_Back_Color);
        }
        if (overstep_m > now_time.minutes)
        {
            overstep_m = 1;
            Caven_GUI_Draw_Circle(120, 120, 60, 12, 100, LCD_Back_Color);
        }
        if (overstep_h > now_time.hour)
        {
            overstep_h = 1;
            Caven_GUI_Draw_Circle(120, 120, 40, 12, 100, LCD_Back_Color);
        }
//        Vofa_JustFloat_Show_Fun (float_array);
//        Mode_Use.TIME.Delay_Ms(10);
    }
}

void Main_Init(void)
{
    system_clock_config();
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Mode_Index();
    Mode_Init.TIME(ENABLE);
    Mode_Init.LED(ENABLE);
    Mode_Init.UART_Init_State = Mode_Init.UART(m_UART_CH1, 115200, ENABLE);
    Mode_Init.LCD(ENABLE);
    Caven_GUI_draw_pixel_bind (Mode_Use.LCD.Draw_Point_pFun);
    //    Mode_Use.UART.Send_String_pFun(m_UART_CH1,"hello world !\n");

    //    printf("SystemClk:%d \r\n", MCU_SYS_FREQ);
}

// Transfer GuiLite 32 bits color to your LCD color
#define GL_RGB_32_to_16(rgb) (((((unsigned int)(rgb)) & 0xFF) >> 3) | ((((unsigned int)(rgb)) & 0xFC00) >> 5) | ((((unsigned int)(rgb)) & 0xF80000) >> 8))
// Encapsulate your LCD driver:
void gfx_draw_pixel(int x, int y, unsigned int rgb)
{
    // LCD_Fast_DrawPoint(x, y, GL_RGB_32_to_16(rgb));
    Mode_Use.LCD.Draw_Point_pFun(x, y, GL_RGB_32_to_16(rgb));
}
// Implement it, if you have more fast solution than drawing pixels one by one.
// void gfx_fill_rect(int x0, int y0, int x1, int y1, unsigned int rgb){}

// UI entry
 struct DISPLAY_DRIVER
{
     void (*draw_pixel)(int x, int y, unsigned int rgb);
     void (*fill_rect)(int x0, int y0, int x1, int y1, unsigned int rgb);
 } my_driver;
 extern void startHelloStar(void* phy_fb, int width, int height, int color_bytes, struct DISPLAY_DRIVER* driver);

 int start_ui(void)
{
     //Link your LCD driver & start UI:
     my_driver.draw_pixel = gfx_draw_pixel;
     my_driver.fill_rect = NULL;//gfx_fill_rect;
     startHelloStar(NULL, 240, 240, 2, &my_driver);
     while(1);
 }
 void delay_ms(int milli_seconds)
{
     SYS_Delay_ms(milli_seconds);
 }
