/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : cavendish
 * Version            : V1.0.0
 * Date               : 2025/11/4
 * Description        : Main program body.
*********************************************************************************/


#include "Base_ETH.h"
#include "Base_TCP_Server.h"
#include "Base_TCP_Client.h"
#include "Mode.h"

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
    SystemCoreClockUpdate();
    Mode_Index();
    Mode_Init.TIME(ENABLE);
    Mode_Init.UART(DEBUG_CH,115200,ENABLE);

    now_time.SYS_Sec = 1762236074;
    Mode_Use.TIME.Set_BaseTIME_pFun (now_time);
    stb_printf("MCU Init,MCU_SYS_FREQ: %d Hz \n",MCU_SYS_FREQ);

    Base_ETH_config_local_ip (1,"192.168.1.168","192.168.1.1","255.255.255.0");
    Base_ETH_Init(1,ENABLE);

    Mode_Use.TIME.Delay_S(2);
    Base_TCP_Server_Config ("8160",ENABLE);
    Base_TCP_Client_Config ("192.168.1.160","8080",ENABLE);
    int old_sec = 0;
    while(1)
    {
        now_time = Mode_Use.TIME.Get_BaseTIME_pFun();
        if (old_sec != now_time.SYS_Sec)
        {
            old_sec = now_time.SYS_Sec;
            // stb_printf("sys time utc: %d s \n",old_sec);
        }
        Base_ETH_Task ();
    }
}
