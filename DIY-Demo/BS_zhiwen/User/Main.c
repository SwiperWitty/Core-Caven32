#include "center_app.h"

void Main_Init(void);

u8 send_array[64];
int run_num;
AS608_Packet zhiwen_pack_get;

int main(void)
{
    int temp_num = 0;
    Caven_BaseTIME_Type now_time;
    Main_Init();
    now_time.SYS_Sec = 1746514130;
    Mode_Use.TIME.Set_BaseTIME_pFun(now_time);
    now_time = Mode_Use.TIME.Get_BaseTIME_pFun();

    Task_Overtime_Type LED_Task = {
            .Switch = 1,
            .Begin_time = now_time,
            .Set_time.SYS_Sec = 1,
            .Set_time.SYS_Us = 500000,
            .Flip_falg = 0,
    };

    while(1)
    {
        now_time = Mode_Use.TIME.Get_BaseTIME_pFun();
        API_Task_Timer (&LED_Task,now_time);        // LED任务
        User_GPIO_set(2,4,LED_Task.Flip_falg);
//        if(LED_Task.Trigger_Flag)
//        {
//            printf("utc %d",now_time.SYS_Sec);
//        }
        if(Center_State_machine(now_time))          // 状态机入口
        {
            break;                                  // 状态机退出,程序重启
        }
		if(zhiwen_pack_get.Run_status & 0x40)
		{
			printf("AS608 PACK\n");
			printf("type: %d\n",zhiwen_pack_get.type);
			printf("code: %d\n \n",zhiwen_pack_get.data[0]);
			AS608_info_packet_fast_clean_Fun(&zhiwen_pack_get);
		}
    }
    SYS_RESET();
}

void zhiwen_data (void *data)
{
	u8 temp_dat = *(u8*)data;
	AS608_info_Make_packet_Fun(&zhiwen_pack_get, temp_dat);
}

void Main_Init(void)
{
    Mode_Index();
	System_app_Init ();
	
	Mode_Use.UART.Receive_Bind_pFun (2,zhiwen_data);
	Mode_Use.UART.Receive_Bind_pFun (DEBUG_OUT,zhiwen_data);
}
