/**
  ******************************************************************************
  * File   : Project/bsp/bsp_key.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Bsp Service Routines.
  ******************************************************************************
  */ 

#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include <string.h>

#define KEY_COUNT    1	   //number of all key combinations					

#define KEY_DOWN_K1		KEY_1_DOWN
#define KEY_UP_K1		KEY_1_UP
#define KEY_LONG_K1		KEY_1_LONG

#define KEY_DOWN_K2		KEY_2_DOWN
#define KEY_UP_K2		KEY_2_UP
#define KEY_LONG_K2		KEY_2_LONG

#define KEY_DOWN_K3		KEY_3_DOWN
#define KEY_UP_K3		KEY_3_UP
#define KEY_LONG_K3		KEY_3_LONG

#define JOY_DOWN_U		KEY_4_DOWN		/* up key */
#define JOY_UP_U		KEY_4_UP
#define JOY_LONG_U		KEY_4_LONG

#define JOY_DOWN_D		KEY_5_DOWN		/* down key */
#define JOY_UP_D		KEY_5_UP
#define JOY_LONG_D		KEY_5_LONG

#define JOY_DOWN_L		KEY_6_DOWN		/* left key */
#define JOY_UP_L		KEY_6_UP
#define JOY_LONG_L		KEY_6_LONG

#define JOY_DOWN_R		KEY_7_DOWN		/* right key */
#define JOY_UP_R		KEY_7_UP
#define JOY_LONG_R		KEY_7_LONG

#define JOY_DOWN_OK		KEY_8_DOWN		/* ok */
#define JOY_UP_OK		KEY_8_UP
#define JOY_LONG_OK		KEY_8_LONG

#define SYS_DOWN_K1K2	KEY_9_DOWN		/* K1 and K2 combination */
#define SYS_UP_K1K2	    KEY_9_UP
#define SYS_LONG_K1K2	KEY_9_LONG

#define SYS_DOWN_K2K3	KEY_10_DOWN		/* K2 and K3 combination */
#define SYS_UP_K2K3  	KEY_10_UP
#define SYS_LONG_K2K3	KEY_10_LONG

/* key ID, used with in function bsp_KeyState() */
typedef enum
{
	KID_K1 = 0,
	KID_K2,
	KID_K3,
	KID_JOY_U,
	KID_JOY_D,
	KID_JOY_L,
	KID_JOY_R,
	KID_JOY_OK
}KEY_ID_E;

//unit: 10ms
#define KEY_FILTER_TIME   5    //short press:50ms
#define KEY_LONG_TIME     100  //long press: 1s			

//key structure for each key
typedef struct
{
	
	uint8_t (*IsKeyDownFunc)(void); //detect key pressed

	uint8_t  Count;			/* short filter counter*/
	uint16_t LongCount;		/* long filter counter */
	uint16_t LongTime;		/* key pressed still time, 0:don't detect long pressed */
	uint8_t  State;			/* current key state. 1:pressed, 0:no pressed */
	uint8_t  RepeatSpeed;	/* continuous send key state period */
	uint8_t  RepeatCount;	/* continuous send key state counter */
}KEY_T;

//define key value
typedef enum
{
	KEY_NONE = 0,			

	KEY_1_DOWN,				
	KEY_1_UP,				
	KEY_1_LONG,				

	KEY_2_DOWN,				
	KEY_2_UP,				
	KEY_2_LONG,				

	KEY_3_DOWN,				
	KEY_3_UP,				
	KEY_3_LONG,				

	KEY_4_DOWN,				
	KEY_4_UP,				
	KEY_4_LONG,				

	KEY_5_DOWN,				
	KEY_5_UP,				
	KEY_5_LONG,				

	KEY_6_DOWN,				
	KEY_6_UP,				
	KEY_6_LONG,				

	KEY_7_DOWN,				
	KEY_7_UP,				
	KEY_7_LONG,				

	KEY_8_DOWN,				
	KEY_8_UP,				
	KEY_8_LONG,				

	/* value below for button combinations*/
	KEY_9_DOWN,				
	KEY_9_UP,				
	KEY_9_LONG,				

	KEY_10_DOWN,			
	KEY_10_UP,				
	KEY_10_LONG,			
}KEY_ENUM;

#define KEY_FIFO_SIZE	10
typedef struct
{
	uint8_t Buf[KEY_FIFO_SIZE];		/* buffer for key value */
	uint8_t Read;					/* read pointer */
	uint8_t Write;					/* write pointer */
	uint8_t Read2;
}KEY_FIFO_T;

void bsp_InitKey(void);
void bsp_KeyScan(void);
void bsp_PutKey(uint8_t _KeyCode);
uint8_t bsp_GetKey(void);
uint8_t bsp_GetKeyState(KEY_ID_E _ucKeyID);
void bsp_SetKeyParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t  _RepeatSpeed);
void bsp_ClearKey(void);

#endif

