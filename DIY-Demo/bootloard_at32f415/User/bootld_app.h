#ifndef _BOOTLD__APP_H_
#define _BOOTLD__APP_H_

#include "Mode.h"
#include "API.h"

#include "system_app.h"

void Bootld_app_Init (void);
int Bootld_Save_bin_Fun(u8 mode,u8 *data,u16 len,u32 pack_num,u16 pack_crc);

int SYS_Boot_CheckAndLoad(SYS_boot_Type boot,uint16_t *crc);

#endif
