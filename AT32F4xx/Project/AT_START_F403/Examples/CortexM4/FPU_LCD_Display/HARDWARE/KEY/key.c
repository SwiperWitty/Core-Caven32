/**
  **************************************************************************
  * File   : key.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Init the Key.
  **************************************************************************
  */

#include "at32f4xx.h"
#include "key.h"
#include "sys.h" 
#include "delay.h" 

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup CortexM4_FPU_LCD_Display
  * @{
  */ 
								    
/**
  * @brief  Initialize Key.
  * @param  None
  * @retval None
  */
void KEY_Init(void) 
{ 
 	GPIO_InitType GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA|RCC_APB2PERIPH_GPIOE,ENABLE);

	GPIO_InitStructure.GPIO_Pins  = GPIO_Pins_2|GPIO_Pins_3|GPIO_Pins_4;//KEY0-KEY2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU; 
 	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pins  = GPIO_Pins_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

}

/**
  * @brief  Key processing.
  * @param  mode 
  *         0: Don't support continuous press.
  *         1:  support continuous press.
  * @retval 0: No press Key.
  *         1: press KEY0
  *         2: press KEY1
  *         3: press KEY2
  *         4: press KEY3
  */
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(KEY0==0||KEY1==0||KEY2==0||WK_UP==1))
	{
		delay_ms(10);//去抖动 
		key_up=0;
		if(KEY0==0)return KEY0_PRES;
		else if(KEY1==0)return KEY1_PRES;
		else if(KEY2==0)return KEY2_PRES;
		else if(WK_UP==1)return WKUP_PRES;
	}else if(KEY0==1&&KEY1==1&&KEY2==1&&WK_UP==0)key_up=1; 	    
 	return 0;// 无按键按下
}

/**
  * @}
  */ 

/**
  * @}
  */ 