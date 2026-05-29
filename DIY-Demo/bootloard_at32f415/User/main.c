#include "center_app.h"
#include "Mode.h"

void Main_Init (void);

uint64_t _LocalTime() {
    Caven_BaseTIME_Type now_time = Mode_Use.TIME.Get_BaseTIME_pFun();
    return (uint64_t)(now_time.SYS_Sec * 1000 + now_time.SYS_Us / 1000);
}

int main (void) {
    int retval = 0;
    Caven_BaseTIME_Type now_time;
    Main_Init();
    now_time.SYS_Sec = 1742299486;
    Mode_Use.TIME.Set_BaseTIME_pFun (now_time);

    while (1) {
        now_time = Mode_Use.TIME.Get_BaseTIME_pFun();
        if(Center_State_machine(now_time) == 1)
        {
            break;
        }
    }
    Mode_Use.TIME.Delay_Ms (250);
    SYS_RESET();
}

void Main_Init (void) {
    Mode_Index();

    Center_app_Init();
    System_app_Init();
}
