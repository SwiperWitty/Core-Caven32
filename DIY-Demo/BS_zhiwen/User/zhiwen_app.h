#ifndef _ZHIWEN_APP_H_
#define _ZHIWEN_APP_H_

#include "Mode.h"

int zhiwen_app_State_machine (Caven_BaseTIME_Type time);

void zhiwen_info_handle (void *data);
void zhiwen_app_mode (u8 mode,u8 sub,int id);

void zhiwen_app_Init (void);
void zhiwen_app_Exit (void);

#endif 
