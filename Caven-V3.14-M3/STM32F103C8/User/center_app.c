#include "center_app.h"
#include "GX_info_frame.h"

Caven_info_packet_Type debug_pack;
GX_info_packet_Type debug_gx_pack;
uint8_t debug_array[500];

Caven_BaseTIME_Type Center_time;
Caven_BaseTIME_Type show_time;

TIM_Capture_Type time1_Capture_val;
TIM_Capture_Type time2_Capture_val;

void debug_info_handle (void *data);
void Capture1_pwm_handle (void *data);
void Capture2_pwm_handle (void *data);

Task_Overtime_Type Vofa_JustFloat_Show_Task = {
		.Switch = 1,
		.Begin_time = {0},
		.Set_time.SYS_Sec = 0,
		.Set_time.SYS_Us = 50000,
		.Flip_falg = 0,
};

char Capture_flag = 0;
int period = 0,temp_val = 0,Freq = 0,Freq_last = 0xff;
float period_t,temp_f = 0.00f,cycle = 0;
float show_buff[10];
char str_array[100];
int time_one = 0;
int Center_State_machine(Caven_BaseTIME_Type time)
{
	int retval = 0;
    Center_time = time;
	
    if (debug_pack.Result & m_Result_SUCC) 
    {
        printf("Caven pack \n");
        printf("Versions: 0x%02x \n",debug_pack.Versions);
        printf("Type: 0x%02x \n",debug_pack.Type);
        printf("Addr: 0x%02x \n",debug_pack.Addr);
        printf("Cmd: 0x%02x \n",debug_pack.Cmd);
        printf("Cmd_sub: 0x%02x \n",debug_pack.Cmd_sub);

        printf("dSize: 0x%x \n",debug_pack.dSize);
        printf("data: --- \n");
        printf("Result: 0x%02x \n",debug_pack.Result);
        printf("Get_num: 0x%x \n",debug_pack.Get_num);
        printf("Comm_way: 0x%02x \n",debug_pack.Comm_way);
        
        Caven_info_packet_fast_clean_Fun(&debug_pack);
    }
    if (debug_gx_pack.Result & m_Result_SUCC) 
    {
        printf("GX pack \n");
        printf("Prot_W_Type: 0x%02x \n",debug_gx_pack.Prot_W_Type);
        printf("Prot_W_Versions: 0x%02x \n",debug_gx_pack.Prot_W_Versions);
        printf("Prot_W_485Type: 0x%02x \n",debug_gx_pack.Prot_W_485Type);
        printf("Prot_W_DFlag: 0x%02x \n",debug_gx_pack.Prot_W_DFlag);
        printf("Prot_W_Class: 0x%02x \n",debug_gx_pack.Prot_W_Class);
        printf("Prot_W_MID: 0x%02x \n",debug_gx_pack.Prot_W_MID);
        printf("Addr: 0x%02x \n",debug_gx_pack.Addr);
        
        printf("dSize: 0x%x \n",debug_gx_pack.dSize);
        printf("data: --- \n");
        printf("Result: 0x%02x \n",debug_gx_pack.Result);
        printf("Get_num: 0x%x \n",debug_gx_pack.Get_num);
        printf("Comm_way: 0x%02x \n",debug_gx_pack.Comm_way);
        
        GX_info_packet_fast_clean_Fun(&debug_gx_pack);
    }
	// Vofa任务
	API_Task_Timer (&Vofa_JustFloat_Show_Task,Center_time);
	if(Center_time.SYS_Sec != time_one)
	{
		time_one = Center_time.SYS_Sec;
		struct tm now_date = Mode_Use.TIME.Get_Date_pFun(8*60*60);
//		printf("date %d/%d/%d %02d:%02d:%02d  utc [%d] [%d]\n",
//			now_date.tm_year,now_date.tm_mon,now_date.tm_mday,
//			now_date.tm_hour,now_date.tm_min,now_date.tm_sec,
//			Center_time.SYS_Sec,Center_time.SYS_Us);
		memset(str_array,0,sizeof(str_array));
		sprintf(str_array,"time: %02d s ",(Center_time.SYS_Sec%60));
		Mode_Use.OLED.Show_String_pFun(6,0,"hello",0,0,16);
		Mode_Use.OLED.Show_String_pFun(0,1,str_array,0,0,16);
		Mode_Use.OLED.Refresh();
		Mode_Use.OLED.Draw_Line_pFun(0,63,127,63,1);
	}
	// Capture
	if (time1_Capture_val.finish_flag)
	{
		period = time1_Capture_val.period_val;
		cycle = time1_Capture_val.high_val;
		temp_f = period;
		cycle /= temp_f;
		period_t = period;
		period_t *= 0.001;
		Capture_flag = 1;
		temp_f = 1 / (period_t / 1000);
		Freq = (int)temp_f;
		memset(&time1_Capture_val,0,sizeof(TIM_Capture_Type));
	}
	if (time2_Capture_val.finish_flag)
	{
		memset(&time2_Capture_val,0,sizeof(TIM_Capture_Type));
	}
	// Vofa
	if(Vofa_JustFloat_Show_Task.Trigger_Flag && Capture_flag)
	{
		Capture_flag = 0;
		show_time = Center_time;
		show_buff[0] = Freq;
		show_buff[1] = cycle;
		Vofa_JustFloat_Show_Fun (2,show_buff);
		Freq = 0;
		cycle = 0.00f;
	}
	temp_val = (Center_time.SYS_Sec - show_time.SYS_Sec)%10;
	temp_val *= 1000000;
	temp_val += Center_time.SYS_Us;
	if (temp_val > 1200000)
	{
		show_time = Center_time;
		show_buff[0] = 0;
		show_buff[1] = 0;
		Vofa_JustFloat_Show_Fun (2,show_buff);
	}
	// LCD
	if (Freq_last != Freq)
	{
		Freq_last = Freq;
		memset(str_array,0,sizeof(str_array));
		sprintf(str_array,"Freq: %04d Hz ",Freq);
		Mode_Use.OLED.Show_String_pFun (0,2,str_array,0,0,16);
	}
	return retval;
}

void Center_app_Init (void)
{
    Caven_info_packet_index_Fun(&debug_pack, debug_array);
	GX_info_packet_index_Fun(&debug_gx_pack, debug_array);
	
	Mode_Use.UART.Receive_Bind_pFun (DEBUG_OUT,debug_info_handle);
	
	TIMx_Capture_Callback_pFunBind(1,Capture1_pwm_handle);
    TIMx_Capture_Callback_pFunBind(2,Capture2_pwm_handle);
}

Caven_info_packet_Type Caven_standard = {
	.Head = 0xFA55,
    .Versions = 0x01,		// 版本
    .dSize = BUFF_MAX,		// 最大长度
};
GX_info_packet_Type GX_standard = {
	.Head = 0x5A,
	.Prot_W_Type = 0x01,
    .Prot_W_Versions = 0x01,	// 版本
    .dSize = BUFF_MAX,			// 最大长度
};
Caven_BaseTIME_Type debug_info_time = {0};

void debug_info_handle (void *data)
{
	uint8_t temp_data = *(uint8_t *)data;
    // time over = 1s
	int temp_num = Center_time.SYS_Sec - debug_info_time.SYS_Sec;
    if (temp_num > 1)
    {
        Caven_info_packet_fast_clean_Fun(&debug_pack);
        GX_info_packet_fast_clean_Fun(&debug_gx_pack);
    }
    debug_info_time = Center_time;
	// caven
	temp_num = Caven_info_Make_packet_Fun(Caven_standard, &debug_pack, temp_data);
	if (debug_pack.Run_status == 0xFF)
	{
		debug_pack.Comm_way = DEBUG_OUT;
	}
	else if (debug_pack.Run_status < 0)
	{
		Caven_info_packet_fast_clean_Fun(&debug_pack);
	}
	if(temp_num <= 0)
	{
		// gx
		temp_num = GX_info_Make_packet_Fun(GX_standard,&debug_gx_pack,temp_data);
		if (debug_gx_pack.Run_status == 0xFF)
		{
			debug_gx_pack.Comm_way = DEBUG_OUT;
		}
		else if (debug_gx_pack.Run_status < 0)
		{
			GX_info_packet_fast_clean_Fun(&debug_gx_pack);
		}
	}
    if(temp_num <= 0)
	{
		// xx

	}
}

void Capture1_pwm_handle (void *data)
{
	TIM_Capture_Type temp_Capture_val;
	if(data != NULL)
	{
		memcpy(&temp_Capture_val,data,sizeof(TIM_Capture_Type));
		memcpy(&time1_Capture_val,data,sizeof(TIM_Capture_Type));

	}
}

void Capture2_pwm_handle (void *data)
{
	TIM_Capture_Type temp_Capture_val;
	if(data != NULL)
	{
		memcpy(&temp_Capture_val,data,sizeof(TIM_Capture_Type));
		memcpy(&time2_Capture_val,data,sizeof(TIM_Capture_Type));

	}
}
