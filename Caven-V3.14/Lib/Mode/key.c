#include "key.h"

void KEY_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能PC端口时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //下拉输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

u16 KEY_Input (void)			//没按下去是1，按下去是0
{
	u16 Temp = 0;
	
	
	return Temp;
}
