/**
  ******************************************************************************
  * File   : Project/bsp/bsp_tim_pwm.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Bsp Service Routines.
  ******************************************************************************
  */ 

#ifndef __BSP_TMR_PWM_H
#define __BSP_TMR_PWM_H

void bsp_SetTIMOutPWM(GPIO_Type* GPIOx, uint16_t GPIO_Pin, TMR_Type* TIMx, uint8_t _ucChannel,
	 uint32_t _ulFreq, uint32_t _ulDutyCycle);

void bsp_SetTIMOutPWM_N(GPIO_Type* GPIOx, uint16_t GPIO_Pin, TMR_Type* TIMx, uint8_t _ucChannel,
	 uint32_t _ulFreq, uint32_t _ulDutyCycle);
	 
void bsp_SetTMRforInt(TMR_Type* TIMx, uint32_t _ulFreq, uint8_t _PreemptionPriority, uint8_t _SubPriority);

#endif



