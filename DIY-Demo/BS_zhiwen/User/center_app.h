/*
 * center_app.h
 *
 *  Created on: 2023
 */

#ifndef USER_CENTER_APP_H_
#define USER_CENTER_APP_H_

#include "mode.h"
#include "caven_app.h"
#include "zhiwen_app.h"

int start_ui(void);
int Center_Init(void);
int Center_State_machine (Caven_BaseTIME_Type time);

#endif /* USER_CENTER_APP_H_ */
