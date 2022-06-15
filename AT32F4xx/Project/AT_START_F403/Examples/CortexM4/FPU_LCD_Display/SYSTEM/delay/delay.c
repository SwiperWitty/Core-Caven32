/**
  **************************************************************************
  * File   : delay.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file provides a set of functions abourt delay tiems.
  **************************************************************************
  */

#include "delay.h"

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup CortexM4_FPU_LCD_Display
  * @{
  */ 
  
static u32  fac_us=0;					   
static u32 fac_ms=0;						
	
/**
  * @brief  Init the delay .
  * @param  None
  * @retval None
  */
void delay_init()
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择外部时钟  HCLK/8
	fac_us=SystemCoreClock/8000000;				//为系统时钟的1/8  
	fac_ms=(u16)fac_us*1000;					//非OS下,代表每个ms需要的systick时钟数   
}								    

/**
  * @brief  Init the us time.
  * @param  nus : How many us you want delay.
  * @retval None
  */	    								   
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; 					//时间加载	  		 
	SysTick->VAL=0x00;        					//清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//开始倒数	  
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		//等待时间到达   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
	SysTick->VAL =0X00;      					 //清空计数器	 
}

/**
  * @brief  Init the ms time.
  * @param  nus : How many ms you want delay.
  *         the most delay ms is : 0xFFFFFF*8*1000/SYSCLK
  * @retval None
  */	 
void delay_ms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;				//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL =0x00;							//清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//开始倒数  
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		//等待时间到达   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
	SysTick->VAL =0X00;       					//清空计数器	  	    
} 

/**
  * @}
  */ 

/**
  * @}
  */ 







































