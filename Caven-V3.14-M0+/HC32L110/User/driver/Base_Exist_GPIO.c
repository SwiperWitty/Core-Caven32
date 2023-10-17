#include "Base_Exist_GPIO.h" 
#include "gpio.h"

void IO_Sleep_Congfig_Fun(void)
{
	Clk_SetPeripheralGate(ClkPeripheralGpio,TRUE);//使能GPIO时钟

	//XTH
	Gpio_InitIOExt(0, 1,GpioDirIn,TRUE,FALSE,FALSE,FALSE);
	Gpio_InitIOExt(0, 2,GpioDirIn,TRUE,FALSE,FALSE,FALSE);

	//XTL
	Gpio_InitIOExt(1, 4,GpioDirIn,TRUE,FALSE,FALSE,FALSE);
	Gpio_InitIOExt(1, 5,GpioDirIn,TRUE,FALSE,FALSE,FALSE);

	Gpio_InitIOExt(2, 3,GpioDirIn,FALSE,FALSE,FALSE,FALSE);
	Gpio_InitIOExt(2, 4,GpioDirIn,FALSE,FALSE,FALSE,FALSE);
	Gpio_InitIOExt(2, 5,GpioDirIn,FALSE,FALSE,FALSE,FALSE);
	Gpio_InitIOExt(2, 6,GpioDirIn,FALSE,FALSE,FALSE,FALSE);

	Gpio_InitIOExt(0, 3,GpioDirIn,FALSE,FALSE,FALSE,FALSE);

	Gpio_InitIOExt(3, 2,GpioDirIn,FALSE,FALSE,FALSE,FALSE);
	Gpio_InitIOExt(3, 3,GpioDirIn,FALSE,FALSE,FALSE,FALSE);
	Gpio_InitIOExt(3, 4,GpioDirIn,FALSE,FALSE,FALSE,FALSE);
    Gpio_InitIOExt(3, 5,GpioDirIn,FALSE,FALSE,FALSE,FALSE);
	Gpio_InitIOExt(3, 6,GpioDirIn,FALSE,FALSE,FALSE,FALSE);

	Gpio_SetIO(0,3,0);
	Gpio_SetIO(2,3,0);
	Gpio_SetIO(2,4,0);
	Gpio_SetIO(2,5,0);
	Gpio_SetIO(2,6,0);
	
	Gpio_SetIO(3,2,0);
	Gpio_SetIO(3,4,0);
	Gpio_SetIO(3,5,0);
	Gpio_SetIO(3,6,0);
}

/*
** 只要主时钟跑起来，就能用
*/
void User_GPIO_Init(int Set)
{
    boolean_t temp;
	IO_Sleep_Congfig_Fun();
    if (Set)
    {
        temp = TRUE;
        Clk_SetPeripheralGate(ClkPeripheralGpio, temp);
		
		Gpio_InitIOExt(3, 5,GpioDirIn,TRUE,FALSE,FALSE,FALSE);	// adc 
		Gpio_InitIOExt(0, 3,GpioDirIn,TRUE,FALSE,FALSE,FALSE);	// key 
        Gpio_InitIOExt(2, 4,GpioDirIn,TRUE,FALSE,FALSE,FALSE);	// gpi 
		
		/* 不带拉	*/
		Gpio_InitIOExt(2, 3,GpioDirOut,FALSE,FALSE,FALSE,FALSE);	// gpo 
		Gpio_InitIOExt(2, 6,GpioDirOut,FALSE,FALSE,FALSE,FALSE);	// mosa 
		Gpio_InitIOExt(2, 5,GpioDirOut,FALSE,FALSE,FALSE,FALSE);	// mosb 
		
		User_GPIO_set(GPO, 0);
		User_GPIO_set(MOSA, 0);
		User_GPIO_set(MOSB, 0);		// off 
    }
    else
    {
        temp = FALSE;
		Clk_SetPeripheralGate(ClkPeripheralGpio, temp);
    }
    
}

/*
**使用的前提是初始化GPIO
**User_GPIO_Type 限制了GPIO的选择
**Set也就是使能，是业务逻辑，非电平逻辑 
*/
void User_GPIO_set (User_GPIO_Type Type,int Set)
{

    switch (Type)
    {
    case 1:
        if (Set)    GPO_H();     // GPO OPEN
        else        GPO_L();
        break;
    case 2:
        if (Set)    MOSA_L();   // MOSA OPEN
        else        MOSA_H();
        break;
    case 3:
        if (Set)    MOSB_L();   // MOSB OPEN
        else        MOSB_H();
        break;

    default:
        break;
    }

}

