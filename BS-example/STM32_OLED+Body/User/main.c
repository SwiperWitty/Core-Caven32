#include "center_app.h"

#include "Items.h"		// 功能列表文件


void Main_Init(void);

int main(void)
{
    int temp_num = 0;
    Caven_BaseTIME_Type now_time;
	Task_Overtime_Type led_task;
	/*
	调用初始化
	*/
    Main_Init();
    /*
	设置当前utc时间
	*/
    now_time.SYS_Sec = 1742299486;
    Mode_Use.TIME.Set_BaseTIME_pFun(now_time);
    /*
	刷新获取utc时间
	*/
    now_time = Mode_Use.TIME.Get_BaseTIME_pFun();
	
	/*
	设置任务开始时间为现在。
	*/
	led_task.Begin_time = now_time;
	/*
	设置1.5s触发一次任务，也就是led灯任务1.5s闪烁一次
	*/
	led_task.Set_time.SYS_Sec = 1,
	led_task.Set_time.SYS_Us = 500000,
	/*
	任务开启
	*/
	led_task.Switch = 1;
    while(1)
    {
		/*
		刷新获取utc时间
		*/
		now_time = Mode_Use.TIME.Get_BaseTIME_pFun();
		/*
		led闪烁任务,只要灯在闪烁，说明程序运行没有死机
		*/
		API_Task_Timer (&led_task,now_time);
		User_GPIO_set(1,11,led_task.Flip_falg);		// 灯的状态跟随任务状态（led_task.Flip_falg）
		/*
		主线程
		*/
		if(Center_State_machine(now_time))          // 状态机入口
		{
			break;									// 状态机退出,程序重启
		}
    }
    SYS_RESET();
}

void Main_Init(void)
{
	/*
	caven sdk初始化，不可修改
	*/
    Mode_Index();
	/*
	软件层初始化（内存以及函数初始化）
	*/
	Center_app_Init ();
	/*
	硬件层初始化（外设）
	*/
	System_app_Init ();
}
