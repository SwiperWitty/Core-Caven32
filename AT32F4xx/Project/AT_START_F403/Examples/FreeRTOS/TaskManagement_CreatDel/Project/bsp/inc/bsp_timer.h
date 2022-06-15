/**
  ******************************************************************************
  * File   : FreeRTOS/bsp/bsp_timer.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Bsp Service Routines.
  ******************************************************************************
  */

#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H

  
#define TMR_COUNT	4	

typedef enum
{
	TMR_ONCE_MODE = 0,
	TMR_AUTO_MODE = 1		
}TMR_MODE_E;


typedef struct
{
	volatile uint8_t Mode;	
	volatile uint8_t Flag;	
	volatile uint32_t Count;	
	volatile uint32_t PreLoad;	
}SOFT_TMR;


void bsp_InitTimer(void);
void bsp_DelayMS(uint32_t n);
void bsp_DelayUS(uint32_t n);
void bsp_StartTimer(uint8_t _id, uint32_t _period);
void bsp_StartAutoTimer(uint8_t _id, uint32_t _period);
void bsp_StopTimer(uint8_t _id);
uint8_t bsp_CheckTimer(uint8_t _id);
int32_t bsp_GetRunTime(void);
int32_t bsp_CheckRunTime(int32_t _LastTime);

void bsp_InitHardTimer(void);
void bsp_StartHardTimer(uint8_t _CC, uint32_t _uiTimeOut, void * _pCallBack);

#endif

  
  
