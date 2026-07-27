#include "user_data_app.h"

#define Log_tag "User_data_app info"

int User_data_app_curve_updata (u8 mode,Caven_Coord3D_Type *data,int len);
int User_data_app_file_updata (void *data,int len);

Task_Overtime_Type User_updata_task;

u8 updata_array[512];
int curve_x_run = 0;
int User_data_app_State_machine (Caven_BaseTIME_Type time)
{
	int retval = 0;
	int temp_val = 0,temp_run;

	API_Task_Timer (&User_updata_task,time);
	if(User_updata_task.Trigger_flag)
	{
		User_updata_task.Trigger_flag = 0;
		temp_run = 0;
		Caven_Coord3D_Type cr_buf[10];
		temp_val = User_GPIO_get(3,13);
		cr_buf[temp_run].x = 1.0 + curve_x_run;
		cr_buf[temp_run].y = 0.0 + temp_val;
		temp_run ++;
		// cr_buf[temp_run].x = 1.0 + curve_x_run;
		// cr_buf[temp_run].y = 1.0 + curve_x_run*2;
		// temp_run ++;
		curve_x_run ++;
		if(temp_run > 0)
		{
			User_data_app_curve_updata (2,cr_buf,temp_run);
		}
	}
	return retval;
}

void User_data_app_Init (void)
{
	User_updata_task.Switch = 1;
	User_updata_task.Set_time.SYS_Sec = 0;
	User_updata_task.Set_time.SYS_Us = 800000;
	User_updata_task.Flip_flag = 0;
	curve_x_run = 0;
}

int User_data_app_curve_updata (u8 mode,Caven_Coord3D_Type *data,int len)
{
	int retval = 0;
	int temp_len = 0;
	u8 temp_array[BUFF_MAX];
	static const u8 tail[4] = {0x00,0x00,0x80,0x7f};

	if(data == NULL || len <= 0)
	{
		return (-1);
	}
	if(mode < 2 || mode > 3 || len > 255)
	{
		return (-2);
	}
	temp_len = 2 + len * mode * (int)sizeof(float) + (int)sizeof(tail);
	if(temp_len > BUFF_MAX)
	{
		return (-3);
	}

	temp_len = 0;
	temp_array[temp_len++] = len & 0xff;
	temp_array[temp_len++] = mode;
	for(int i = 0; i < len; i++)
	{
		memcpy(&temp_array[temp_len],&data[i].x,sizeof(float));
		temp_len += sizeof(float);
		memcpy(&temp_array[temp_len],&data[i].y,sizeof(float));
		temp_len += sizeof(float);
		if(mode >= 3)
		{
			memcpy(&temp_array[temp_len],&data[i].z,sizeof(float));
			temp_len += sizeof(float);
		}
	}
	memcpy(&temp_array[temp_len],tail,sizeof(tail));
	temp_len += sizeof(tail);

	retval = Caven_info_return_Fun (1,1,g_SYS_Config.Addr,1,m_CAVEN_CMD1_Curve_updata_Order,temp_len,temp_array,m_Result_Back_Empty,updata_array);
	if(retval > 0)
	{
		System_Send_data (updata_array,retval,g_SYS_Config.temp_val->Connect_passage);
	}
	return retval;
}

int User_data_app_file_updata (void *data,int len)
{
	int retval = 0;
	System_Send_data (data,len,g_SYS_Config.temp_val->Connect_passage);
	return retval;
}
