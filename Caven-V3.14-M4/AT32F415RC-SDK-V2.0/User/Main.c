
/*
 *
 */
#include "main.h"


u8 send_array[64];
int run_num;

float float_array[10];
float time_temp;

int Draw_Circle(U16 x0,U16 y0,U16 radius,char wide,char percent,U16 color);

int main(void)
{
    Main_Init();

    Caven_Watch_Type now_time = {
            .hour = 8,
            .minutes = 7,
            .second  = 45,
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
    Vofa_JustFloat_Init_Fun (2,Debug_Out);
    float_array[1] = 3.3;

    char last_s = 0,last_m = 0,last_h = 0;
    char overstep_s = 1,overstep_m = 1,overstep_h = 1;
    int sec_point_drop,min_point_drop,hour_point_drop;

    Mode_Use.LCD.Show_String_pFun (9,0,"12",LCD_BLACK,LCD_Back_Color,24);
    Mode_Use.LCD.Show_String_pFun (19,5,"3",LCD_BLACK,LCD_Back_Color,24);
    Mode_Use.LCD.Show_String_pFun (9,9,"6",LCD_BLACK,LCD_Back_Color,24);
    Mode_Use.LCD.Show_String_pFun (0,5,"9",LCD_BLACK,LCD_Back_Color,24);
    while(1)
    {
        now_time = Mode_Use.TIME.Get_Watch_pFun();
//        printf("sys time: %d : %d : %d , %d (us)\n",now_time.hour,now_time.minutes,now_time.second,now_time.time_us);
        API_Task_Timer (&LED_Task,now_time);        // LED任务
        Mode_Use.LED.SET_pFun(1,LED_Task.Flip_falg);

        float_array[0] += 1.133;
        if (float_array[0] > 10) {
            float_array[0] = 0;
        }

        time_temp = now_time.second;
        time_temp = (time_temp / 60) * 100;
        run_num = Caven_math_approximate ((int)time_temp,5,0,100);
        if (last_s != run_num) {
            last_s = run_num;
            Draw_Circle(((sec_point_drop >> 8) & 0xff),(sec_point_drop & 0xff),1, 5,100,LCD_Back_Color);
            run_num = Draw_Circle(120, 120, 80, 11,last_s,LCD_RED);
            sec_point_drop = run_num;
            Draw_Circle(((sec_point_drop >> 8) & 0xff),(sec_point_drop & 0xff),1, 5,100,LCD_RED);
        }
        time_temp = now_time.minutes;
        time_temp = (time_temp / 60) * 100;
        run_num = Caven_math_approximate ((int)time_temp,5,0,100);
        if (last_m != run_num) {
            last_m = run_num;
            Draw_Circle(((min_point_drop >> 8) & 0xff),(min_point_drop & 0xff),1, 5,100,LCD_Back_Color);
            run_num = Draw_Circle(120, 120, 60, 11,last_m,LCD_BLUE);
            min_point_drop = run_num;
            Draw_Circle(((min_point_drop >> 8) & 0xff),(min_point_drop & 0xff),1, 5,100,LCD_BLUE);
        }
        time_temp = now_time.hour;
        time_temp = (time_temp / 12) * 100;
        run_num = Caven_math_approximate ((int)time_temp,5,0,100);
        if (last_h != run_num) {
            last_h = run_num;
            Draw_Circle(((hour_point_drop >> 8) & 0xff),(hour_point_drop & 0xff),1, 5,100,LCD_Back_Color);
            run_num = Draw_Circle(120, 120, 40, 11,last_h,LCD_GREEN);
            hour_point_drop = run_num;
            Draw_Circle(((hour_point_drop >> 8) & 0xff),(hour_point_drop & 0xff),1, 5,100,LCD_GREEN);
        }

        if (overstep_s > now_time.second) {
            overstep_s = 1;
            Draw_Circle(120, 120, 80, 12,100,LCD_Back_Color);
        }
        if (overstep_m > now_time.minutes) {
            overstep_m = 1;
            Draw_Circle(120, 120, 60, 12,100,LCD_Back_Color);
        }
        if (overstep_h > now_time.hour) {
            overstep_h = 1;
            Draw_Circle(120, 120, 40, 12,100,LCD_Back_Color);
        }
//        Vofa_JustFloat_Show_Fun (float_array);
//        Mode_Use.TIME.Delay_Ms(10);
    }

}

int Draw_Circle(U16 x0,U16 y0,U16 radius,char wide,char percent,U16 color)
{
    int retval = 0;
    int a, b;
    int temp_num;
    int temp_num_b;
    int temp_run;
    char subzone;

    u8 pic_array[256][2];
    U16 show_x,show_y;
    U16 x_point = 0;

    U16 middle_point = 0,middle_maxpointx,middle_maxpointy;
    U16 Outside_sin_max,Outside_cos_max;

    float f_temp_percent;
    float f_temp_num;

    if (percent <= 0) {
        return retval;
    }

    if (wide % 2) {
        middle_point = (wide + 1) / 2;
        middle_point += radius;
    }
    subzone = percent/25 + 1;
    f_temp_num = percent % 25;
    f_temp_percent = (f_temp_num / 25) * 90;    // 变成角度
    f_temp_num = (f_temp_percent / 180) * 3.1415926;    //

    do{
        a = 0;
        x_point = 0;

        b = radius;
        temp_run = 0;
        temp_num = b * b;
        temp_num_b = b * b;
        Outside_sin_max = Caven_math_approximate_float(sinf(f_temp_num) * (radius));
        Outside_cos_max = Caven_math_approximate_float(cosf(f_temp_num) * (radius));
        while (a <= b)      // 画圆（弧）
        {
            pic_array[temp_run][0] = b; pic_array[temp_run++][1] = a;
            pic_array[temp_run][0] = a; pic_array[temp_run++][1] = b;
            a++;
            if ((a * a + temp_num_b) > temp_num) // 判断要画的点是否过远
            {
                b--;
                temp_num_b = b * b;
            }
        }
    //    for (int i = 0; i < temp_run; i++) {            // 1象限
    //        Mode_Use.LCD.Draw_Point_pFun (x0+pic_array[i][0],y0-pic_array[i][1],color);
    //    }
    //    for (int i = 0; i < temp_run; i++) {            // 2
    //        Mode_Use.LCD.Draw_Point_pFun (x0-pic_array[i][0],y0-pic_array[i][1],color);
    //    }
    //    for (int i = 0; i < temp_run; i++) {            // 3
    //        Mode_Use.LCD.Draw_Point_pFun (x0-pic_array[i][0],y0+pic_array[i][1],color);
    //    }
    //    for (int i = 0; i < temp_run; i++) {            // 4
    //        Mode_Use.LCD.Draw_Point_pFun (x0+pic_array[i][0],y0+pic_array[i][1],color);
    //    }
        if(subzone > 1)
        {
            for (int i = 0; i < temp_run; i++) {            // 1
                show_x = x0+pic_array[i][0];
                show_y = y0-pic_array[i][1];
                Mode_Use.LCD.Draw_Point_pFun (show_x,show_y,color);
            }
        }
        else if(subzone == 1)
        {
            x_point = Outside_sin_max + x0;
            middle_maxpointx = 0;
            middle_maxpointy = 0;
            for (int i = 0; i < temp_run; i++) {
                show_x = x0+pic_array[i][0];
                show_y = y0-pic_array[i][1];
                if (show_x <= x_point) {
                    Mode_Use.LCD.Draw_Point_pFun (show_x,show_y,color);
                    if (middle_maxpointx <= show_x) // 取大
                    {
                        middle_maxpointx = show_x;
                        middle_maxpointy = show_y;
                    }
                }
            }
        }
        if(subzone > 2)
        {
            for (int i = 0; i < temp_run; i++) {
                show_x = x0+pic_array[i][0];
                show_y = y0+pic_array[i][1];
                Mode_Use.LCD.Draw_Point_pFun (show_x,show_y,color);
            }
        }
        else if(subzone == 2)
        {
            x_point = Outside_cos_max + x0;
            middle_maxpointx = ~0;
            middle_maxpointy = ~0;
            for (int i = 0; i < temp_run; i++) {
                show_x = x0+pic_array[i][0];
                show_y = y0+pic_array[i][1];
                if (show_x >= x_point) {
                    Mode_Use.LCD.Draw_Point_pFun (show_x,show_y,color);
                    if (middle_maxpointx >= show_x)  // 取小
                    {
                        middle_maxpointx = show_x;
                        middle_maxpointy = show_y;
                    }
                }
            }
        }
        if(subzone > 3)
        {
            for (int i = 0; i < temp_run; i++) {
                show_x = x0-pic_array[i][0];
                show_y = y0+pic_array[i][1];
                Mode_Use.LCD.Draw_Point_pFun (show_x,show_y,color);
            }
        }
        else if(subzone == 3)
        {
            x_point = x0 - Outside_sin_max;
            middle_maxpointx = ~0;
            middle_maxpointy = ~0;
            for (int i = 0; i < temp_run; i++) {
                show_x = x0-pic_array[i][0];
                show_y = y0+pic_array[i][1];
                if (show_x >= x_point) {
                    Mode_Use.LCD.Draw_Point_pFun (show_x,show_y,color);
                    if (middle_maxpointx >= show_x) // 取小
                    {
                        middle_maxpointx = show_x;
                        middle_maxpointy = show_y;
                    }
                }
            }
        }
        if(subzone > 4)
        {
            for (int i = 0; i < temp_run; i++) {
                show_x = x0-pic_array[i][0];
                show_y = y0-pic_array[i][1];
                Mode_Use.LCD.Draw_Point_pFun (show_x,show_y,color);
            }
        }
        else if(subzone == 4)
        {
            x_point = x0 - Outside_cos_max;
            middle_maxpointx = 0;
            middle_maxpointy = 0;
            for (int i = 0; i < temp_run; i++) {
                show_x = x0-pic_array[i][0];
                show_y = y0-pic_array[i][1];
                if (show_x <= x_point) {
                    Mode_Use.LCD.Draw_Point_pFun (show_x,show_y,color);
                    if (middle_maxpointx <= show_x) // 取大
                    {
                        middle_maxpointx = show_x;
                        middle_maxpointy = show_y;
                    }
                }
            }
        }
        if (radius == middle_point) {
            retval = middle_maxpointx;
            retval <<= 8;
            retval += middle_maxpointy;
        }
        radius++;
    }while(wide--);
    return retval;
//    printf("x : %d %%,quadrant: %d,x: %5.2f -> sin x = %5.3f,cos y = %5.3f \n",percent,quadrant,f_temp_percent,x_point,y_point);
//    sprintf(send_array,"%3d%% ",percent);
//    Mode_Use.LCD.Show_String_pFun (8,0,send_array,LCD_BLACK,LCD_Back_Color,24);
}

void Main_Init(void)
{
    system_clock_config();
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
    
    Mode_Index();
    Mode_Init.TIME(ENABLE);
    Mode_Init.LED(ENABLE);
    Mode_Init.UART_Init_State = Mode_Init.UART(m_UART_CH1,115200,ENABLE);
    Mode_Init.LCD(ENABLE);

//    Mode_Use.UART.Send_String_pFun(m_UART_CH1,"hello world !\n");

//    printf("SystemClk:%d \r\n", MCU_SYS_FREQ);
}

