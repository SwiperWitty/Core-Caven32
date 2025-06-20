#include "Mode.h"
#include "API.h"
#include "pic.h"
#include "caven_gui.h"

#include "lvgl.h"
#include "ui.h"

/*
            软件文件夹->AT32文件夹->Keil工程
项目文件->
            云端库文件夹...
*/
#ifdef PICTURE
#define Photo1 gImage_4_caven
#define Photo2 gImage_example
#endif

u16 ADC_array[10];			// [0] x,[1] y,[2] vin,[3] vout,[4] temp,[5] ele;

void Main_Init(void);
void ADC_Data_Handle (void * data);

#define BUFF_LEN 5

static lv_disp_draw_buf_t draw_buf_dsc_1;
static lv_color_t buf_1[MY_DISP_HOR_RES * BUFF_LEN];
static lv_disp_drv_t disp_drv;
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
	uint32_t w = (area->x2 - area->x1 + 1);
	uint32_t h = (area->y2 - area->y1 + 1);
	// M5.Lcd.drawBitmap(area->x1, area->y1, w, h, &color_p->full);
	// lcd_show_image(area->x1, area->y1, w, h, &color_p->full);
    Mode_Use.LCD.Show_Picture_pFun(area->x1, area->y1, w, h, (u8*)&color_p->full); // Photo
	lv_disp_flush_ready(disp_drv);
}

uint32_t custom_millis(void)
{
	uint32_t retval;
    Caven_BaseTIME_Type now_time;
    now_time = Mode_Use.TIME.Get_BaseTIME_pFun();
	retval = now_time.SYS_Us / 1000;
	retval += now_time.SYS_Sec * 1000;
	return retval;
}

/**
 * Initialize the Hardware Abstraction Layer (HAL) for LVGL
 */
static void hal_init(void)
{

}


void gui_init (void)
{
	/*Initialize LVGL*/
	lv_init();
	/*Initialize the HAL (display, input devices, tick) for LVGL*/
	hal_init();
	lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, MY_DISP_HOR_RES * BUFF_LEN);
	lv_disp_drv_init(&disp_drv);
	disp_drv.hor_res = MY_DISP_HOR_RES;
	disp_drv.ver_res = MY_DISP_VER_RES;
	disp_drv.flush_cb = disp_flush;
	disp_drv.draw_buf = &draw_buf_dsc_1;
	lv_disp_drv_register(&disp_drv);
	ui_init();
}

float ele_max = 8,tem_max = 40;

int main(void)
{
	Main_Init();
    gui_init ();
	int retval = 0;
    char array_buff[300];
    float float_array[10];
	
	char run_val = 0,run_ele = 0,run_temp = 0;
	float float_vol_array[10];
	float float_ele_array[10];
	float float_temp_array[10];
	
    int temp_num,temp_run;
    float f_temp_num,f_temp_val;
    float vcc_vol,ele_vol,tem_vol;

	int Bat_xs = 0;
    int temp_times;
    int temp_key = 0;
    int temp_pic = 0;
    Caven_BaseTIME_Type now_time;
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

    while (1)
    {
        now_time = Mode_Use.TIME.Get_BaseTIME_pFun();
        //        printf("sys time: %d : %d : %d , %d (us)\n",now_time.hour,now_time.minutes,now_time.second,now_time.time_us);
        API_Task_Timer(&LED_Task, now_time); // LED任务
        Mode_Use.LED.Set_pFun(1, LED_Task.Flip_falg);

		f_temp_num = ADC_array[0];	// VCC
		vcc_vol = (f_temp_num / 4096) * 3.3 * 8.5;	// 还原
		f_temp_num = ADC_array[1];	// ELE
		ele_vol = ((f_temp_num / 4096) * 3.3) * (200/50);	// (x/50)/0.005 = x * 4
		f_temp_num = 4096 - ADC_array[2];	// TEMP
		tem_vol = ((f_temp_num / 4096) * 3.3) * 14;

		temp_num = Caven_Data_Median_filtering_Handle (vcc_vol,float_vol_array,&f_temp_val,&run_val,10);
		if (temp_num)
		{
			f_temp_val = MIN(f_temp_val,25);
			f_temp_num = (f_temp_val / 25) * 100;      // 25v max
			temp_num = Caven_math_approximate_float(f_temp_num);
			sprintf(array_buff,"%5.2fV",f_temp_val+0.005);
			lv_label_set_text(ui_val, array_buff);
//			lv_arc_set_value(ui_valc, temp_num);	// vcc
			f_temp_num = f_temp_val / 3.7;
			if (Bat_xs == 0)
			{
				Bat_xs = f_temp_num;
			}
			else if (Bat_xs > f_temp_num)		// 保护电池放电
			{
				retval = 1;
			}
			f_temp_num = Bat_xs;
			f_temp_num  *= 3.7;
			f_temp_num = f_temp_val - f_temp_num;		// 当前可用电压
			f_temp_val = Bat_xs;
			f_temp_val *= 0.5;		// 4.2-3.7
			f_temp_num = (f_temp_num / f_temp_val) * 100;
			temp_num = Caven_math_approximate_float(f_temp_num);
			lv_arc_set_value(ui_valc, temp_num);	// bat
			sprintf(array_buff,"%dS-Bat",Bat_xs);
			lv_label_set_text(ui_bat, array_buff);
		}
		temp_num = Caven_Data_Median_filtering_Handle (ele_vol,float_ele_array,&f_temp_val,&run_ele,10);
		if (temp_num)
		{
			f_temp_val = MIN(f_temp_val,10);
			f_temp_num = (f_temp_val / ele_max) * 100;      // ele max
			temp_num = Caven_math_approximate_float(f_temp_num);
			sprintf(array_buff,"%5.2fA",f_temp_val+0.005);
			lv_label_set_text(ui_ele, array_buff);
			lv_arc_set_value(ui_elsc, temp_num);
			f_temp_num = f_temp_val * vcc_vol;
			if (f_temp_num > 0.5)		// 功率，静态12*0.02 = 0.24w
			{
				lv_obj_set_style_text_color(ui_start, lv_color_hex(0xFB0303), LV_PART_MAIN | LV_STATE_DEFAULT);
			}
			else
			{
				lv_obj_set_style_text_color(ui_start, lv_color_hex(0x0DA420), LV_PART_MAIN | LV_STATE_DEFAULT);
			}
			if (f_temp_val >= ele_max)		// 超过软件设定的电流最大值 
			{
				retval = 1;
			}
		}
		temp_num = Caven_Data_Median_filtering_Handle (tem_vol,float_temp_array,&f_temp_val,&run_temp,10);
		if (temp_num)
		{
			f_temp_val = MIN(f_temp_val,80);
			f_temp_num = (f_temp_val / tem_max) * 100;                   // temp max
			temp_num = Caven_math_approximate_float(f_temp_num);
			sprintf(array_buff,"%4.1fC",f_temp_val+0.05);
			lv_label_set_text(ui_temp, array_buff);
			lv_arc_set_value(ui_tempc, temp_num);
			if (f_temp_val > tem_max)		// 超过软件设定的温度最大值 
			{
				retval = 1;
			}
		}

//        Vofa_JustFloat_Show_Fun (float_array);
        lv_timer_handler();

		if(User_GPIO_get(3,13) == 0)
		{
			temp_times = 0;
			do{
				Mode_Use.TIME.Delay_Ms(100);
				temp_num = User_GPIO_get(3,13);
				temp_times++;
				if (temp_times > 20)	// 3 sec
				{
					temp_times = 30;
//					LCD_Fill_Fun (0, 0, LCD_W, LCD_H, LCD_WHITE);
					lv_obj_set_style_text_color(ui_start, lv_color_hex(0xFB0303), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(ui_start, "OFF");
					lv_timer_handler();
				}
			}while(temp_num == 0);		// 松手了 
			if (temp_times > 20)		// 2sec 激活kill power
			{
				retval = 1;
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
		if (retval)		// 激活退出
		{
			User_GPIO_set(2,5,ENABLE);
			Mode_Use.TIME.Delay_Ms(100);
		}
		else
		{
        	Mode_Use.TIME.Delay_Ms(5);
		}
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
    Mode_Init.TIME(ENABLE);
    Mode_Init.UART(DEBUG_OUT, 115200, ENABLE);

    Mode_Init.User_ADC(ENABLE);
	Mode_Use.USER_ADC.Receive_Bind_pFun(ADC_Data_Handle);
	
    Mode_Use.LCD.Set_TargetModel_pFun(m_LCD_TYPE_1_28);
    Mode_Init.LCD(ENABLE);
	
	User_GPIO_config(3,13,0);	//
	User_GPIO_config(2,4,1);
	User_GPIO_config(2,5,1);
	User_GPIO_set(2,4,DISABLE);
	User_GPIO_set(2,5,DISABLE);	//kill 
//	IIC_Start_Init(ENABLE);
    
//	Caven_GUI_draw_pixel_bind (Mode_Use.LCD.Draw_Point_pFun);
	while (reverse);
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
