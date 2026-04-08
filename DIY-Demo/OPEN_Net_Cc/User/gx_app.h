#ifndef _GX_APP_H_
#define _GX_APP_H_

#include "Mode.h"
#include "system_app.h"

#include "GX_info_frame.h"

typedef struct
{
    uint8_t epc_len;
    uint8_t epc[16];
    uint8_t tid_len;
    uint8_t tid[16];
    uint16_t tag_crc;
    uint8_t RSSI;
    uint8_t ant; 
}RFID_Tag_Type;

int GX_app_State_machine(Caven_BaseTIME_Type time);
int GX_app_Make_pack (uint8_t data,int way,Caven_BaseTIME_Type time);

void GX_app_Init (void);
void GX_app_Exit (void);

#endif 
