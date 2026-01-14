/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : cavendish
 * Version            : V1.0.0
 * Date               : 2025/11/4
 * Description        : Main program body.
*********************************************************************************/

#include "system_app.h"

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program
 *
 * @return  none
 */
int main(void)
{
    Caven_BaseTIME_Type now_time;
    Mode_Index();
    system_app_init();
    now_time.SYS_Sec = 1762236074;
    Mode_Use.TIME.Set_BaseTIME_pFun (now_time);
    int old_sec = 0;
    while(1)
    {
        now_time = Mode_Use.TIME.Get_BaseTIME_pFun();
        if (old_sec != now_time.SYS_Sec)
        {
            old_sec = now_time.SYS_Sec;
            // stb_printf("sys time utc: %d s \n",old_sec);
        }
        sys_gpio_State_machine (now_time);
        Base_ETH_Task ();
    }
}


