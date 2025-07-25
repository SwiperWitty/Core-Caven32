#ifndef _CENTER__APP_H_
#define _CENTER__APP_H_

#include "Mode.h"
#include "API.h"

#include "system_app.h"
#include "caven_app.h"
#include "gx_app.h"

void Center_app_Init (void);
int Center_State_machine(Caven_BaseTIME_Type time);

#endif
