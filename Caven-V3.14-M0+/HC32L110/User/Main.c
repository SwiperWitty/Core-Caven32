#include "main.h"


int main (void)
{
    Main_Init ();
	while(1)
	{
		
	}
}

void Main_Init (void)
{
	stc_bt_config_t stcBtConfig;
    stc_lpm_config_t stcLpmCfg;
	
	DDL_ZERO_STRUCT(stcBtConfig);
    DDL_ZERO_STRUCT(stcLpmCfg);
	
	stcLpmCfg.enSLEEPDEEP = SlpDpEnable;		// SlpDpDisable; //
    stcLpmCfg.enSLEEPONEXIT = SlpExtDisable;	// 唤醒后不自动进入睡眠 //
	
    Clk_Enable(ClkRCL, TRUE);
    Clk_SetRCLFreq(ClkFreq38_4K);	// 内部38.4K 
	
//    Clk_SetPeripheralGate(ClkPeripheralLpTim, TRUE);
    Clk_SetPeripheralGate(ClkPeripheralGpio, TRUE);
	
//	Uart_lp_Init (TRUE); 
	
	Gpio_InitIO(3,5,GpioDirIn);		// adc 
	Gpio_InitIO(0,3,GpioDirIn);		// key 
	
	Gpio_InitIO(2,4,GpioDirIn);		// gpi 
	Gpio_InitIO(2,3,GpioDirOut);	// gpo 
	
	Gpio_InitIO(2,6,GpioDirOut);	// mosa 
	Gpio_InitIO(2,5,GpioDirOut);	// mosb 

	Gpio_SetIO(2,3,0);
	
	Gpio_SetIO(2,6,1);
	Gpio_SetIO(2,5,1);
	
	Base_NOP_Delay (6000,100);
	do{
		Base_NOP_Delay (10,10);
	}while(1 == Gpio_GetIO(0,3));	// 注意：此处用户不能屏蔽，否则进入深度休眠模式导致SWD连接不上 // 
	#if LOW_POW
    Lpm_Config(&stcLpmCfg);
    Lpm_GotoLpmMode();
	#endif 
	
//	Gpio_SetIO(2,6,0);
}




/**
 *********************** 
 **使用这个函数需要 
 **配置P25、P26为UART收发端口 
 **配置波特率9600bps、偶校验、1stop 
 **/
void Uart_lp_Init (int Set)
{
    stc_lpuart_config_t	stcConfig;
    stc_lpuart_irq_cb_t	stcLPUartIrqCb;
	stc_lpuart_multimode_t	stcMulti;
	
	stc_lpuart_sclk_sel_t	stcLpuart_clk;
	stc_lpuart_mode_t	stcRunMode;
	
	DDL_ZERO_STRUCT(stcConfig);
    DDL_ZERO_STRUCT(stcLPUartIrqCb);
    DDL_ZERO_STRUCT(stcMulti);
    
    if(Set)
    {
        Clk_SetPeripheralGate(ClkPeripheralLpUart,TRUE);	// 使能LPUART时钟 
        Clk_SetPeripheralGate(ClkPeripheralGpio,TRUE); 
        // 通道端口配置 
        Gpio_InitIOExt(3,3,GpioDirOut,TRUE,FALSE,FALSE,FALSE);
        Gpio_InitIOExt(3,4,GpioDirOut,TRUE,FALSE,FALSE,FALSE);
		Gpio_SetFunc_UART2RX_P33();		// P33 
		Gpio_SetFunc_UART2TX_P34();		// P34 
//        Gpio_SetFunc_UART2RX_P25(); 
//        Gpio_SetFunc_UART2TX_P26(); 
        
        stcLpuart_clk.enSclk_sel = LPUart_Rcl;	// LPUart_Pclk; //
        stcLpuart_clk.enSclk_Prs = LPUartDiv1;
            
        stcRunMode.enLpMode = LPUartLPMode;     // 正常工作模式或低功耗工作模式配置 
        stcRunMode.enMode   = LPUartMode3;
        
        stcLPUartIrqCb.pfnRxIrqCb = RxIntCallback;
        stcLPUartIrqCb.pfnTxIrqCb = TxIntCallback;
        stcLPUartIrqCb.pfnRxErrIrqCb = ErrIntCallback;

        stcMulti.enMulti_mode = LPUartNormal;   // 只有模式2/3才有多主机模式 

		stcConfig.pstcLpuart_clk = &stcLpuart_clk;
		stcConfig.pstcRunMode = &stcRunMode;
        stcConfig.pstcIrqCb = &stcLPUartIrqCb;
		stcConfig.pstcMultiMode = &stcMulti; 
        stcConfig.bTouchNvic = TRUE;
        LPUart_EnableIrq(LPUartRxIrq);
        LPUart_Init(&stcConfig);

        LPUart_EnableFunc(LPUartRx);
    }
}

uint8_t u8TxData[10] = {0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55};//"Xiaohua MCU!";
uint8_t u8RxData[10];
uint8_t u8TxCnt=0,u8RxCnt=0;

void TxIntCallback(void)
{
    if(u8TxCnt<5)
    {
        M0P_LPUART->SBUF = u8RxData[u8TxCnt];

        u8TxCnt++;
    }
    else //if(u8TxCnt>10)
    {
        u8TxCnt = 0;
        u8RxCnt = 0;
        LPUart_ClrStatus(LPUartTxEmpty);   
        LPUart_DisableIrq(LPUartTxIrq);
        LPUart_EnableIrq(LPUartRxIrq);
    }
    
}
void RxIntCallback(void)
{
    if(u8RxCnt<5)
    {
        u8RxData[u8RxCnt]=LPUart_ReceiveData();
		if(LPUart_CheckEvenOrOdd(Even,u8RxData[u8RxCnt])!=Ok)
		{
			u8RxCnt=0;
		}
		else
		{
			u8RxCnt++;
		}
    }
    else 
    {
        u8RxCnt = 0;
    }
}
void ErrIntCallback(void)
{
  
}
