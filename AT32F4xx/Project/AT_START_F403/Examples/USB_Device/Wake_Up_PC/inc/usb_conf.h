/**
  ******************************************************************************
  * File   : USB_Device/Mouse/inc/usb_conf.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : USB Endpoint config header.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_CONF_H
#define __USB_CONF_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* External variables --------------------------------------------------------*/
/*-------------------------------------------------------------*/
/* EP_NUM */
/* defines how many endpoints are used by the device */
/*-------------------------------------------------------------*/
#define EP_NUM     (8)

/*-------------------------------------------------------------*/
/* --------------   Buffer Description Table  -----------------*/
/*-------------------------------------------------------------*/
/* buffer table base address */
/* buffer table base address */
#define BTABLE_ADDRESS      (0x00)

/* EP0  */
/* rx/tx buffer base address */
//#define ENDP0_RXADDR        (0x18)
//#define ENDP0_TXADDR        (0x58)


#define ENDP0_RXADDR        (0x150)
#define ENDP0_TXADDR        (0x180)
/* EP1  */
/* tx buffer base address */
#define ENDP1_TXADDR        (0x100)

#define EP_TXADDR        (0x100)
#define EP_RXADDR        (0x100)

#define ENDP7_IN
#define ENDP1_OUT
//ENDP1_OUT

#ifdef ENDP1_IN
#define EP_IN   ENDP1
#define EP_DIN  EP1_IN
#endif

#ifdef ENDP2_IN
#define EP_IN   ENDP2
#define EP_DIN  EP2_IN
#endif

#ifdef ENDP3_IN
#define EP_IN   ENDP3
#define EP_DIN  EP3_IN
#endif

#ifdef ENDP4_IN
#define EP_IN   ENDP4
#define EP_DIN  EP4_IN
#endif

#ifdef ENDP5_IN
#define EP_IN   ENDP5
#define EP_DIN  EP5_IN
#endif

#ifdef ENDP6_IN
#define EP_IN   ENDP6
#define EP_DIN  EP6_IN
#endif

#ifdef ENDP7_IN
#define EP_IN   ENDP7
#define EP_DIN  EP7_IN
#endif


#ifdef ENDP1_OUT
#define EP_OUT   ENDP1
#define EP_DOUT  EP1_OUT
#endif

#ifdef ENDP2_OUT
#define EP_OUT   ENDP2
#define EP_DOUT  EP2_OUT
#endif

#ifdef ENDP3_OUT
#define EP_OUT   ENDP3
#define EP_DOUT  EP3_OUT
#endif

#ifdef ENDP4_OUT
#define EP_OUT   ENDP4
#define EP_DOUT  EP4_OUT
#endif

#ifdef ENDP5_OUT
#define EP_OUT   ENDP5
#define EP_DOUT  EP5_OUT
#endif

#ifdef ENDP6_OUT
#define EP_OUT   ENDP6
#define EP_DOUT  EP6_OUT
#endif

#ifdef ENDP7_OUT
#define EP_OUT   ENDP7
#define EP_DOUT  EP7_OUT
#endif

/*-------------------------------------------------------------*/
/* -------------------   ISTR events  -------------------------*/
/*-------------------------------------------------------------*/
/* IMR_MSK */
/* mask defining which events has to be handled */
/* by the device application software */
#define IMR_MSK (CTRL_CTFR_IEN  | CTRL_WKUP_IEN | CTRL_SUSP_IEN | CTRL_ERR_IEN  | CTRL_SOF_IEN \
                 | CTRL_ESOF_IEN | CTRL_RST_IEN )

/* CTR service routines */
/* associated to defined endpoints */
#define  EP1_IN_Callback   NOP_Process
#define  EP2_IN_Callback   NOP_Process
#define  EP3_IN_Callback   NOP_Process
#define  EP4_IN_Callback   NOP_Process
#define  EP5_IN_Callback   NOP_Process
#define  EP6_IN_Callback   NOP_Process
#define  EP7_IN_Callback   NOP_Process

#define  EP1_OUT_Callback   NOP_Process
#define  EP2_OUT_Callback   NOP_Process
#define  EP3_OUT_Callback   NOP_Process
#define  EP4_OUT_Callback   NOP_Process
#define  EP5_OUT_Callback   NOP_Process
#define  EP6_OUT_Callback   NOP_Process
#define  EP7_OUT_Callback   NOP_Process

#endif /*__USB_CONF_H*/



