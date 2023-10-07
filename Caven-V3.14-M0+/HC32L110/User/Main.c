#include "main.h"


void Main_Init (void);
int main (void)
{
    Main_Init ();
	while(1)
	{
		
	}
}

void Main_Init (void)
{
    Clk_Enable(ClkRCL, TRUE);
    Clk_SetRCLFreq(ClkFreq38_4K);//内部38.4K

    Clk_SetPeripheralGate(ClkPeripheralLpUart,TRUE);//使能LPUART时钟
    Clk_SetPeripheralGate(ClkPeripheralBt,TRUE);    
    
    //通道端口配置
    Gpio_InitIOExt(2,5,GpioDirOut,TRUE,FALSE,FALSE,FALSE);
    Gpio_InitIOExt(2,6,GpioDirOut,TRUE,FALSE,FALSE,FALSE);

    Gpio_SetFunc_UART2RX_P25();
    Gpio_SetFunc_UART2TX_P26();
}

