/*
 * center_app.h
 *
 *  Created on: 2023
 */

#ifndef USER_CENTER_APP_H_
#define USER_CENTER_APP_H_

#include "system_app.h"
#include "gpio_app.h"
#include "API.h"
#include "GX_info_frame.h"
#include "modbus_RFID.h"



typedef struct
{
    int Status_flag;
    u8 aa;

}CAVEN_Status_Event_Type;

int Center_Init(void);
int Center_State_machine (Caven_Watch_Type time);

#endif /* USER_CENTER_APP_H_ */
