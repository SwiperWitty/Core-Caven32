
#if 1

#include "gd32f4xx.h"
#include <stdio.h>
#include "main.h"
#include "string.h"
#include "gd32f450i_eval.h"
#include "base_flash.h"

typedef struct fcm_data_t
{
    int a;
    char b;
    char c[1100];
    char end;
} fcm_data;

fcm_data test_f = {
    .a = 0x0000f0fa,
    .b = 58,
//    .c = "012345",
    .end = 0x5A,
};
fcm_data test_user_r,test_user_1;

#define addr_f (int)0x0803e000
/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    memset(&test_user_r,0,sizeof(test_user_r));

    gd_eval_led_init(LED2);
    fmc_init();
    gd_eval_led_off(LED2);
    memset(test_f.c,0,sizeof(test_f.c));
    /* read 32 bit length data from a given address */
    fmc_read_data(addr_f,&test_user_r,sizeof(test_user_r));

    if(test_user_r.end == 0x5A)                             //上电读取
    {
        gd_eval_led_on(LED2);
        test_user_r.end++;
        test_user_r.a = sizeof(fcm_data);
        fmc_write_data(addr_f, &test_user_r,sizeof(test_user_r));
    }
    else
    {
        gd_eval_led_off(LED2);
        fmc_write_data(addr_f, &test_f,sizeof(test_f));     //写入默认
    }

    fmc_read_data(addr_f,&test_user_1,sizeof(test_user_1));

    while(1);
}

#endif
