#include "center_app.h"

void Main_Init(void);

u8 send_array[64];
int run_num;

int main(void)
{
    Caven_BaseTIME_Type now_time;
    Main_Init();
    now_time.SYS_Sec = 1742200000;
    Mode_Use.TIME.Set_BaseTIME_pFun(now_time);
    now_time = Mode_Use.TIME.Get_BaseTIME_pFun();

    Task_Overtime_Type LED_Task = {
            .Switch = 1,
            .Begin_time = {0},
            .Set_time.SYS_Sec = 1,
            .Set_time.SYS_Us = 500000,
            .Flip_flag = 0,
    };
    User_GPIO_config(2,4,1);
	uint8_t iic_buf[10] = {0xff,6,7};
	uint8_t iic_array[10];
    while(1)
    {
        now_time = Mode_Use.TIME.Get_BaseTIME_pFun();
        API_Task_Timer (&LED_Task,now_time);        // LED任务
        if(LED_Task.Trigger_flag)
		{
			Base_IIC_Send_DATA(0x5a,iic_buf,1,1,10,1);
			Base_IIC_Receive_DATA(0x5a,iic_array,1,1,10);
			User_GPIO_set(2,4,LED_Task.Flip_flag);
			Debug_printf("->UTC %ds:%d us \n",now_time.SYS_Sec,now_time.SYS_Us);
//			Debug_printf("begin time %ds:%d us \n",LED_Task.Begin_time.SYS_Sec,LED_Task.Begin_time.SYS_Us);
		}

        if(Center_State_machine(now_time))          // 状态机入口
        {
            break;                                  // 状态机退出,程序重启
        }
    }
    SYS_RESET();
}

void Main_Init(void)
{
    Mode_Index();
    Mode_Init.TIME(ENABLE);
    Mode_Use.TIME.Delay_Ms(10);

    Mode_Init.UART(DEBUG_CH,115200,ENABLE);
    Debug_printf("MCU Init,MCU_SYS_FREQ: %d Hz \n",MCU_SYS_FREQ);
	Base_IIC_Init(ENABLE);
}
