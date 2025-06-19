#ifndef _GX_APP_H_
#define _GX_APP_H_

#include "Mode.h"
#include "system_app.h"

#include "GX_info_frame.h"

int GX_app_State_machine(Caven_BaseTIME_Type time);
int GX_app_Make_pack (uint8_t data,int way,Caven_BaseTIME_Type time);

void GX_app_Init (void);
void GX_app_Exit (void);

#endif 
