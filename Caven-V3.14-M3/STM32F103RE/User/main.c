#include "center_app.h"

void Main_Init(void);

u8 send_array[64];
int run_num;

int main(void)
{
    Caven_BaseTIME_Type now_time;
    Main_Init();
    now_time.SYS_Sec = 1742299486;
    Mode_Use.TIME.Set_BaseTIME_pFun(now_time);
    now_time = Mode_Use.TIME.Get_BaseTIME_pFun();

    Task_Overtime_Type LED_Task = {
            .Switch = 1,
            .Begin_time = {0},
            .Set_time.SYS_Sec = 1,
            .Set_time.SYS_Us = 500000,
            .Flip_falg = 0,
    };
    User_GPIO_config(2,4,1);
    while(1)
    {
        now_time = Mode_Use.TIME.Get_BaseTIME_pFun();
        API_Task_Timer (&LED_Task,now_time);        // LED任务
        User_GPIO_set(2,4,LED_Task.Flip_falg);

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

    Mode_Init.UART(1,115200,ENABLE);
    stb_printf("MCU Init,MCU_SYS_FREQ: %d Hz \n",MCU_SYS_FREQ);
}
