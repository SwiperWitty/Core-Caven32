#ifndef _BIRD_APP_H_
#define _BIRD_APP_H_

#include "Mode.h"
#include "system_app.h"

int Bird_app_State_machine(Caven_BaseTIME_Type time);
void Bird_app_Init (void);
void Bird_app_Exit (void);

#endif 
