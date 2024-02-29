
/*
 *    主频使用96mhz，这样操作flash不需要降频，同时可以使用USB。
 *
 */
#include "main.h"

u8 send_array[64];
int run_num;
float float_array[10];

void Draw_Circle(U16 x0, U16 y0, char radius, char wide,U16 color);

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
        Draw_Circle(120, 120, 50, 0,LCD_BLACK);

//        Vofa_JustFloat_Show_Fun (float_array);
        Mode_Use.TIME.Delay_Ms(10);
    }

    SYS_RESET();
}

void Draw_Circle(U16 x0, U16 y0, char radius, char wide,U16 color)
{
    int a, b;
    a = 0;
    b = radius;
    int temp_num = radius * radius;
    int temp_num_b = b * b;
    int temp_run = 0;

    char pic_array[400][2];

    while (a <= b)
    {
        pic_array[temp_run][0] = x0 + b - 120; pic_array[temp_run++][1] = y0 - a - 120;
        pic_array[temp_run][0] = x0 + a - 120; pic_array[temp_run++][1] = y0 - b - 120;
//        temp_run++;
        a++;
        if ((a * a + temp_num_b) > temp_num) // 判断要画的点是否过远
        {
            b--;
            temp_num_b = b * b;
        }
    }
    for (int i = 0; i < temp_run; i++) {
        Mode_Use.LCD.Draw_Point_pFun (pic_array[i][0]+120,pic_array[i][1]+120,color);
    }
    for (int i = 0; i < temp_run; i++) {
        Mode_Use.LCD.Draw_Point_pFun (pic_array[i][0]+120,120-pic_array[i][1],color);
    }
    for (int i = 0; i < temp_run; i++) {
        Mode_Use.LCD.Draw_Point_pFun (120-pic_array[i][0],pic_array[i][1]+120,color);
    }
    for (int i = 0; i < temp_run; i++) {
        Mode_Use.LCD.Draw_Point_pFun (120-pic_array[i][0],120-pic_array[i][1],color);
    }
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

    Mode_Use.UART.Send_String_pFun(m_UART_CH1,"hello world !\n");

//    printf("SystemClk:%d \r\n", MCU_SYS_FREQ);
}

