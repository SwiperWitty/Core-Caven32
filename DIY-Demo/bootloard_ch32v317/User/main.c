#include "center_app.h"
#include "Mode.h"

void Main_Init(void);

int main(void)
{
    Caven_BaseTIME_Type now_time;
	
    Main_Init();
    now_time.SYS_Sec = 1742299486;
    Mode_Use.TIME.Set_BaseTIME_pFun(now_time);
	
	while(1)
    {
		now_time = Mode_Use.TIME.Get_BaseTIME_pFun();

		if(Center_State_machine(now_time))          // 状态机入口 11000 b
		{
			break;									// 状态机退出,程序重启
		}
    }
	Mode_Use.TIME.Delay_Ms (250);
    SYS_RESET();
}

void Main_Init(void)
{
	SystemCoreClockUpdate();
    Mode_Index();
	
	Center_app_Init ();		// 6300 b
	System_app_Init ();		// 2000	b
}


