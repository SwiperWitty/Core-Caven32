#include "Mode.h"
#include "API.h"
#include "pic.h"
#include "caven_gui.h"

/*
            软件文件夹->AT32文件夹->Keil工程
项目文件->
            云端库文件夹...
*/
#ifdef PICTURE
#define Photo1 gImage_4_caven
#define Photo2 gImage_example
#endif


int temp = 0;
u16 ADC_array[10];			// [0] x,[1] y,[2] vin,[3] vout,[4] temp,[5] ele;
char array_buff[300];
int run_num;

float float_array[10];
float time_temp;
float vcc_vol,ele_vol,tem_vol;

int temp_times = 0;
int temp_key = 0;
int temp_pic = 0;
void Main_Init(void);
void ADC_Data_Handle (void * data);

/*

*/
int main(void)
{
    Main_Init();

    Caven_Watch_Type now_time = {
        .hour = 8,
        .minutes = 7,
        .second = 55,
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

//    Mode_Use.LCD.Show_String_pFun(9, 0, "12", LCD_BLACK, LCD_Back_Color, 24);
//    Mode_Use.LCD.Show_String_pFun(19, 5, "3", LCD_BLACK, LCD_Back_Color, 24);
//    Mode_Use.LCD.Show_String_pFun(9, 9, "6", LCD_BLACK, LCD_Back_Color, 24);
//    Mode_Use.LCD.Show_String_pFun(0, 5, "9", LCD_BLACK, LCD_Back_Color, 24);
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
		vcc_vol = ADC_array[0];	// VCC
		vcc_vol = (vcc_vol / 4096) * 3.3 * 8.5;	// 还原
		sprintf(array_buff,"Vcc:%5.2f",vcc_vol);
//		Mode_Use.LCD.Show_String_pFun(10, 6, array_buff, LCD_BLACK, LCD_Back_Color, 16);
		ele_vol = ADC_array[1];	// ELE
		ele_vol = ((ele_vol / 4096) * 3.3) * (200/50);	// (x/50)/0.005 = x * 4
		sprintf(array_buff,"Ele:%5.2f",ele_vol);
//		Mode_Use.LCD.Show_String_pFun(10, 7, array_buff, LCD_BLACK, LCD_Back_Color, 16);
		tem_vol = ADC_array[2];	// TEMP
		
		time_temp = (vcc_vol / 25) * 100;
        run_num = Caven_math_approximate((int)time_temp, 5, 1, 100);
        if (run_num != last_s)
        {
            last_s = run_num;
			Caven_GUI_Draw_Circle(120, 120, 80, 12, 100, LCD_Back_Color);
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
		
		time_temp = (ele_vol / 10) * 100;
        run_num = Caven_math_approximate((int)time_temp, 5, 1, 100);
        if (run_num != last_m)
        {
            last_m = run_num;
			Caven_GUI_Draw_Circle(120, 120, 60, 12, 100, LCD_Back_Color);
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
//        time_temp = now_time.hour;
//        time_temp = (time_temp / 12) * 100;
//        run_num = Caven_math_approximate((int)time_temp, 5, 0, 100);
//        if (last_h != run_num)
//        {
//            last_h = run_num;
//				Caven_GUI_Draw_Circle(120, 120, 40, 12, 100, LCD_Back_Color);
//            Caven_GUI_Draw_Circle(((hour_point_drop >> 8) & 0xff), (hour_point_drop & 0xff), 1, 5, 100, LCD_Back_Color);
//            run_num = Caven_GUI_Draw_Circle(120, 120, 40, 11, last_h, LCD_GREEN);
//            hour_point_drop = run_num;
//            temp_num = run_num & 0xff;
//            temp_run = (run_num >> 8) & 0xff;
//            if (temp_num > 0 && temp_run > 0)
//            {
//                Caven_GUI_Draw_Circle(temp_run, temp_num, 1, 5, 100, LCD_GREEN);
//            }
//        }

        if (overstep_s != now_time.second)
        {
            overstep_s = now_time.second;
        }

//        Vofa_JustFloat_Show_Fun (float_array);

		if(User_GPIO_get(3,13) == 0)
		{
			temp_times = 0;
			do{
				Mode_Use.TIME.Delay_Ms(100);
				temp = User_GPIO_get(3,13);
				temp_times++;
				if (temp_times > 30)	// 3 sec
				{
					temp_times = 30;
					LCD_Fill_Fun (0, 0, LCD_W, LCD_H, LCD_WHITE);
				}
			}while(temp == 0);
			//松手了 
			if (temp_times >= 20)		// 3 sec 激活kill power
			{
				User_GPIO_set(2,5,ENABLE);
				Mode_Use.TIME.Delay_Ms(100);
			}
			else
			{
				temp_key++;
			}
		}
		if(temp_pic != temp_key)
		{
			temp_pic = temp_key;
			User_GPIO_set(2,4,temp_pic%2);
		}
        Mode_Use.TIME.Delay_Ms(100);
    }
}

void Main_Init(void)
{
    int reverse = 0;
	//
    system_clock_config();
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
	//
    Mode_Index();
	//
    Mode_Init.TIME_Init_State = Mode_Init.TIME(ENABLE);
    Mode_Init.UART_Init_State = Mode_Init.UART(DEBUG_OUT, 115200, ENABLE);
    Mode_Init.LCD_Init_State = Mode_Init.LCD(ENABLE);
    
    Mode_Init.User_ADC(ENABLE);
	Mode_Use.USER_ADC.Receive_Bind_pFun(ADC_Data_Handle);

	User_GPIO_config(3,13,0);	//
	User_GPIO_config(2,4,1);
	User_GPIO_config(2,5,1);
	User_GPIO_set(2,4,DISABLE);
	User_GPIO_set(2,5,DISABLE);	//kill 
	IIC_Start_Init(ENABLE);
	// 
//	Caven_GUI_draw_pixel_bind (Mode_Use.LCD.Draw_Point_pFun);
    while (reverse);

    Mode_Use.UART.Send_String_pFun(DEBUG_OUT, "Hello world ! \n");
//	Mode_Use.LCD.Show_String_pFun(5, 5, "Bird Final", LCD_Word_Color, LCD_Back_Color, 24);
#ifdef PICTURE
    Mode_Use.LCD.Show_Picture_pFun(0, 0, 240, 240, Photo1); // Photo
//    Mode_Use.LCD.Show_Picture_pFun(0, 0, 240, 240, Photo2); // Photo
#endif
}


void ADC_Data_Handle (void * data)
{
    memcpy(ADC_array,data,sizeof(ADC_array));
    if(ADC_array[5] > 2000) // ELE
    {
//        DC_5V_OFF();
    }
    if(ADC_array[6] > 2000) // MCU_TEMP
    {
//        DC_5V_OFF();
    }
}

