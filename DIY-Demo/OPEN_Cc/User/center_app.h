#ifndef _CENTER__APP_H_
#define _CENTER__APP_H_

#include "Mode.h"
#include "API.h"

#include "system_app.h"
#include "caven_app.h"
#if SYS_BTLD == 0
// #include "gx_app.h"
#endif

void Center_app_Init (void);
int Center_State_machine(Caven_BaseTIME_Type time);

#endif
