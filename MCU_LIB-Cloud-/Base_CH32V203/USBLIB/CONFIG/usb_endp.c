/********************************** (C) COPYRIGHT *******************************
 * File Name          : usb_endp.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/08/08
 * Description        : Endpoint routines
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/ 
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
#include "hw_config.h"      //������ caven
#include "usb_istr.h"
#include "usb_pwr.h"

/* Interval between sending IN packets in frame number (1 frame = 1ms) */
#define IN_FRAME_INTERVAL             5

uint8_t HIDKey[ENDP1_IN_SIZE] = {0};		  // dd-dd
uint8_t HIDTxBuffer[ENDP2_IN_SIZE] = {0};
uint8_t HIDRxBuffer[ENDP2_OUT_SIZE] = {0};

struct Caven_Data HID_Data = {0};
unsigned char HID_Data_Buff[1030];

volatile u8 Endp1Busy = FALSE;            // dd-dd
volatile u8 Endp2Busy = FALSE;
u16 USB_Rx_Cnt=0; 

/*********************************************************************
 * @fn      EP1_IN_Callback & EP2_IN_Callback
 *
 * @brief  Endpoint 2 IN.
 *
 * @return  none
 */
void EP1_IN_Callback (void)
{
    Endp1Busy = FALSE;
}
void EP2_IN_Callback (void)               // dd-dd
{ 
	Endp2Busy = FALSE;
}

/*********************************************************************
 * @fn      EP2_OUT_Callback
 *
 * @brief  Endpoint 2 IN.
 *
 * @return  none
 */
void EP1_OUT_Callback(void)               // dd-dd
{

}

void EP2_OUT_Callback(void)                   // dd-dd
{
    uint8_t USB_RX_LEN;
    USB_RX_LEN = USB_SIL_Read(EP2_OUT, HIDRxBuffer);
    HID_Data.Poit_U8 = HID_Data_Buff;
//    printf("EP1_OUT:\n");
    memcpy(HID_Data.Poit_U8 + HID_Data.Length,&HIDRxBuffer,USB_RX_LEN);  //�����µ��������� buff��      //  dd-dd
    HID_Data.Length += USB_RX_LEN;

//    for (int i = 0; i < USB_RX_LEN; i++)
//    {
//        printf("%02x ",HIDRxBuffer[i]);
//    }
//    printf("num: %d\n",(int)USB_RX_LEN);
    SetEPRxValid(ENDP2);
}

/*******************************************************************************
 * @fn       SOF_Callback
 *
 * @brief    SOF call back.
 *
 * @return   None.
 */
void SOF_Callback(void)
{
	static uint32_t FrameCount = 0;

	if(bDeviceState == CONFIGURED)
	{
		if (FrameCount++ == IN_FRAME_INTERVAL)
		{
			FrameCount = 0;
			//Handle_USBAsynchXfer();
		}
	}  
}






