
/*
 *    主频使用96mhz，这样操作flash不需要降频，同时可以使用USB。
 *
 */
#include "main.h"
#include "math.h"


u8 send_array[64];
int run_num;
float float_array[10];

void Draw_Circle(U16 x0,U16 y0,char radius,char wide,char percent,U16 color);

int main(void)
{
    Main_Init();

    Caven_Watch_Type now_time;
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
    char x_bili = 50;
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
//        Mode_Use.LCD.Draw_Circle_pFun(100,100,50,LCD_BLACK);
        Mode_Use.LCD.Draw_Point_pFun (120,120,LCD_BLACK);
        Draw_Circle(120, 120, 80, 16,x_bili,LCD_BLACK);
        x_bili += 5;
        if (x_bili > 100) {
            x_bili = 0;
            LCD_Fill_Fun (0, 0, LCD_W, LCD_H, LCD_Back_Color);
        }
//        Vofa_JustFloat_Show_Fun (float_array);
        Mode_Use.TIME.Delay_Ms(1000);
    }

    SYS_RESET();
}

void Draw_Circle(U16 x0,U16 y0,char radius,char wide,char percent,U16 color)
{
    int a, b;
    int temp_num;
    int temp_num_b;
    int temp_run;
    u8 pic_array[256][2];
    u8 show_x,show_y;

    char quadrant = 0;          //
    char subzone;
    float f_temp_percent;
    float f_temp_num;
    float x_point = 0,y_point = 0;

    subzone = percent/25 + 1;
    switch (subzone) {
        case 1:
            quadrant = 1;
            break;
        case 2:
            quadrant = 4;
            break;
        case 3:
            quadrant = 3;
            break;
        case 4:
            quadrant = 2;
            break;
        default:
            quadrant = 5;
            break;
    }
    f_temp_num = percent % 25;
    f_temp_percent = (f_temp_num / 25) * 90;
    f_temp_num = (f_temp_percent / 180) * 3.1415926;

    do{
        a = 0;
        b = radius++;
        temp_run = 0;
        temp_num = b * b;
        temp_num_b = b * b;

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
            x_point = sinf(f_temp_num) * radius + x0;
            y_point = cosf(f_temp_num) * radius + y0;
            for (int i = 0; i < temp_run; i++) {
                show_x = x0+pic_array[i][0];
                show_y = y0-pic_array[i][1];
                if (show_x <= x_point) {
                    Mode_Use.LCD.Draw_Point_pFun (show_x,show_y,color);
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
            x_point = cosf(f_temp_num) * radius + x0;
            y_point = sinf(f_temp_num) * radius + y0;
            for (int i = 0; i < temp_run; i++) {
                show_x = x0+pic_array[i][0];
                show_y = y0+pic_array[i][1];
                if (show_y <= y_point) {
                    Mode_Use.LCD.Draw_Point_pFun (show_x,show_y,color);
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
            x_point = sinf(f_temp_num) * radius + x0;
            y_point = cosf(f_temp_num) * radius + y0;
            for (int i = 0; i < temp_run; i++) {
                show_x = x0-pic_array[i][0];
                show_y = y0+pic_array[i][1];
                if (show_y >= y_point) {
                    Mode_Use.LCD.Draw_Point_pFun (show_x,show_y,color);
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
            x_point = cosf(f_temp_num) * radius + x0;
            y_point = y0 - sinf(f_temp_num) * radius;
            for (int i = 0; i < temp_run; i++) {
                show_x = x0-pic_array[i][0];
                show_y = y0-pic_array[i][1];
                if (show_y >= y_point) {
                    Mode_Use.LCD.Draw_Point_pFun (show_x,show_y,color);
                }
            }
        }
    }while(wide--);
    printf("x : %d %%,quadrant: %d,x: %5.2f -> sin x = %5.3f,cos y = %5.3f \n",percent,quadrant,f_temp_percent,x_point,y_point);
    sprintf(send_array,"%3d%% ",percent);
    Mode_Use.LCD.Show_String_pFun (8,0,send_array,LCD_BLACK,LCD_Back_Color,24);
}

void Main_Init(void)
{
//    SystemInit() 由系统自启完成
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Mode_Index();
    Mode_Init.TIME(ENABLE);
    Mode_Init.LED(ENABLE);
    Mode_Init.UART_Init_State = Mode_Init.UART(m_UART_CH1,115200,ENABLE);
    Mode_Init.LCD(ENABLE);

//    Mode_Use.UART.Send_String_pFun(m_UART_CH1,"hello world !\n");

//    printf("SystemClk:%d \r\n", MCU_SYS_FREQ);
}

