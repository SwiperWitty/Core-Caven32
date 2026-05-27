#ifndef _CAVEN_APP_H_
#define _CAVEN_APP_H_

#include "Mode.h"
#include "system_app.h"

int Caven_app_State_machine(Caven_BaseTIME_Type time);
int Caven_app_Make_pack (uint8_t data,int way,Caven_BaseTIME_Type time);
int Caven_app_Dual_cache_Make_pack 
(uint8_t *cache_a,int *p_len_a,uint8_t *cache_b,int *p_len_b,char *p_flag,uint8_t *p_Collect_d,int *p_Collect_n,int Collect_max,int way,Caven_BaseTIME_Type time);
int Caven_app_JSON_Make_pack (char *data,int way);

int Caven_app_BOOT_check (void);
void Caven_app_Init (void);
void Caven_app_Exit (void);

#endif 
