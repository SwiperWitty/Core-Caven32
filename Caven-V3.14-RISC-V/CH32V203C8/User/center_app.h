/*
 * center_app.h
 *
 *  Created on: 2023
 */

#ifndef USER_CENTER_APP_H_
#define USER_CENTER_APP_H_

#include "system_app.h"


#include "modbus_RFID.h"

typedef struct
{
    u8 GPO_num;
    u8 GPI_num;

    u8 GPO_Status_Set[36];
    u8 GPO_Status_Last[36];
    int GPO_Keep_Time;

    u8 GPI_Status_Get[36];
    u8 GPI_Statls_Last[36];

    u8 LED_Status_Set[4];
    u8 BZZ_Status_Set[2];
    int BZZ_Keep_Time;

}GPIO_cfg_Type;


int Center_Init(void);
int Center_State_machine (Caven_Watch_Type time);

#endif /* USER_CENTER_APP_H_ */
