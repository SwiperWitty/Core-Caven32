#ifndef _CAVEN_APP_H_
#define _CAVEN_APP_H_

#include "Mode.h"
#include "system_app.h"

extern SYS_cfg_Type g_SYS_Config;
extern Caven_event_Type g_Events_buff;

int Caven_app_State_machine(Caven_BaseTIME_Type time);
int Caven_app_Make_pack (uint8_t data,int way,Caven_BaseTIME_Type time);

void Caven_app_Init (void);
void Caven_app_Exit (void);

#endif 
