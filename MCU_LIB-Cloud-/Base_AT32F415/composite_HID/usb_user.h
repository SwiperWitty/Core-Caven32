#ifndef __USB_USER_H
#define __USB_USER_H

#include "usb_core.h"
#include "at32f415.h"
#include "string.h"

#include "cdc_keyboard_class.h"

extern otg_core_type otg_core_struct;


void USB_User_init (void);
//keyboard
void keyboard_send_string(uint8_t *string, uint8_t len);
uint16_t USB_Keyboard_Send_Data (uint8_t *data, uint16_t u16Sendlen);

//custom
uint16_t USB_Buffer_send (const void *Data,uint16_t *bufflen);        //有缓冲的发送
uint16_t USB_Buffer_Receive (uint8_t *Data);

#endif

//
