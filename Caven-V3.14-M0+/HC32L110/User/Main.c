#include "main.h"

volatile int LptTest_L    = 0;
volatile int LptTest_falg = 0;
volatile uint32_t Clk_Val = 0;

uint32_t Lpt_CNT = 0;
int Delay_num = 0;

#define CLK_H 1 
#define Lpt_10MS 100 	// MAX [200] * 10 (ms)

int SYS_Status = 0;

int main(void)
{
    Main_Init();
    while (1) {
        switch (SYS_Status) {
            case (0): {
				if(LptTest_falg)
				{
					LptTest_falg = 0;
					Lpt_Stop();
					Lpt_ARRSet(Lpt_CNT);
					if(Delay_num == 1)
					{
						SYSClkwork_Mode (m_Clk_H);	// have 
					}
					if (LptTest_L >= 2) 
					{
						User_do_check();
						
						SYS_Status++;
					}
					if(SYS_Status == 0)					// nothing
					{
						Lpt_Run();
//                		SYSClkwork_Mode (m_Clk_L); // or 
						Low_Pow_MODE(TRUE);
					}
				}
            } break;
            case (1): {
                SYS_Status++;
                /* do something	*/
            } break;
            case (2): {
                SYS_Status++;
                LptTest_L = 0;
                Lpt_Run();
                /* run time	*/
            } break;
            case (3): {
                SYS_Status = 0;
//                SYSClkwork_Mode (m_Clk_L); // or 
				Low_Pow_MODE(TRUE); 
                /* low pow & clean Status	*/
            } break;
            default:
                break;
        }
		Base_NOP_Delay(1, Delay_num);
    }
}

int User_do_check(void)
{
    int retval = 0;
	User_GPIO_set(MOSA, 1);
	User_GPIO_set(MOSB, 1);
    Base_NOP_Delay(10, Delay_num);
    User_GPIO_set(MOSA, 0);
	User_GPIO_set(MOSB, 0);
    return retval;
}





void Main_Init(void)
{
    Clk_Val = Clk_GetHClkFreq();
	SYSClkwork_Mode (m_Clk_H);		// Start 
	
	Clk_SetRCLFreq(ClkFreq32768);	//  \ ClkFreq32768 
	Clk_Enable(ClkRCL, TRUE);
	
    User_GPIO_Init(TRUE);
	
	User_GPIO_set(MOSA, 1);
	User_GPIO_set(MOSB, 1);
    //	Uart_lp_Init (TRUE);
    TIME_lp_Init(TRUE);
    Base_NOP_Delay(500, Delay_num);
	
    do {
        Base_NOP_Delay(10, Delay_num);
    } while (1 == KEY_R()); // 注意：此处用户不能屏蔽，否则进入深度休眠模式导致SWD连接不上 //
	
	User_GPIO_set(MOSA, 0);
    User_GPIO_set(MOSB, 0); // 全关
//	SYSClkwork_Mode (m_Clk_L);
	
}

void SYSClkwork_Mode (User_Clk_Type Mode)
{
    stc_clk_config_t stcClkCfg;
	
    stcClkCfg.enHClkDiv = ClkDiv1;
    stcClkCfg.enPClkDiv = ClkDiv1;
    
    if (Mode == m_Clk_H)
    {
        Clk_SetRCHFreq(ClkFreq4Mhz); 
        stcClkCfg.enClkSrc  = ClkRCH; 
        Delay_num = 200; 
    }
    else
    {
        stcClkCfg.enClkSrc  = ClkRCL;
        Delay_num = 1;
    }
    Clk_Init(&stcClkCfg);
}

void LptInt(void)
{
    if (TRUE == Lpt_GetIntFlag()) {
        Lpt_ClearIntFlag();
        LptTest_falg = 1;
        LptTest_L++;
        if (LptTest_L > 1000) {
            LptTest_L = 0;
        }
    }
}

int TIME_lp_Init(int Set)
{
    stc_lpt_config_t stcConfig; // time
    int retval          = Ok;
	Lpt_CNT = 0xFFFF - (327*Lpt_10MS);
	
    if (Set) {
        Clk_SetPeripheralGate(ClkPeripheralLpTim, TRUE);
        stcConfig.enGateP  = LptPositive;
        stcConfig.enGate   = LptGateDisable;
        stcConfig.enTckSel = LptIRC32K;
        stcConfig.enTog    = LptTogDisable;
        stcConfig.enCT     = LptTimer;
        stcConfig.enMD     = LptMode2;

        stcConfig.pfnLpTimCb = LptInt;

        if (Lpt_Init(&stcConfig) != Ok)
            retval = Error;
        if (retval == Ok) {

            // Lpt 中断使能
            Lpt_ClearIntFlag();
            Lpt_EnableIrq();
            EnableNvic(LPTIM_IRQn, 3, TRUE);

            // 设置重载值，计数初值，启动计数
            Lpt_ARRSet(Lpt_CNT);
            Lpt_Run();
        }
    } else {
        Clk_SetPeripheralGate(ClkPeripheralLpTim, FALSE);
    }

    return retval;
}

void Low_Pow_MODE(int Set)
{
	stc_lpm_config_t stcLpmCfg; // Low Power Mode
    DDL_ZERO_STRUCT(stcLpmCfg);
	if(Set)
	{
#ifdef LOW_POW 
		Base_NOP_Delay(1, Delay_num);
		// Low Power Mode Cfg 
		stcLpmCfg.enSEVONPEND   = SevPndDisable;	// 事件挂起禁止
		stcLpmCfg.enSLEEPDEEP   = SlpDpEnable;		// 睡眠立即生效 
		stcLpmCfg.enSLEEPONEXIT = SlpExtDisable;	// 中斷退出低功耗去執行外設 
		Lpm_Config(&stcLpmCfg);
		Base_NOP_Delay(1, Delay_num);
		Lpm_GotoLpmMode();
#endif
	}
}

/**
 ***********************
 **使用这个函数需要
 **配置P25、P26为UART收发端口
 **配置波特率9600bps、偶校验、1stop
 **/
void Uart_lp_Init(int Set)
{
    stc_lpuart_config_t stcConfig;
    stc_lpuart_irq_cb_t stcLPUartIrqCb;
    stc_lpuart_multimode_t stcMulti;

    stc_lpuart_sclk_sel_t stcLpuart_clk;
    stc_lpuart_mode_t stcRunMode;

    DDL_ZERO_STRUCT(stcConfig);
    DDL_ZERO_STRUCT(stcLPUartIrqCb);
    DDL_ZERO_STRUCT(stcMulti);

    if (Set) {
        Clk_SetPeripheralGate(ClkPeripheralLpUart, TRUE); // 使能LPUART时钟
        Clk_SetPeripheralGate(ClkPeripheralGpio, TRUE);
        // 通道端口配置
        Gpio_InitIOExt(3, 3, GpioDirOut, TRUE, FALSE, FALSE, FALSE);
        Gpio_InitIOExt(3, 4, GpioDirOut, TRUE, FALSE, FALSE, FALSE);
        Gpio_SetFunc_UART2RX_P33(); // P33
        Gpio_SetFunc_UART2TX_P34(); // P34
                                    //        Gpio_SetFunc_UART2RX_P25();
                                    //        Gpio_SetFunc_UART2TX_P26();

        stcLpuart_clk.enSclk_sel = LPUart_Rcl; // LPUart_Pclk; //
        stcLpuart_clk.enSclk_Prs = LPUartDiv1;

        stcRunMode.enLpMode = LPUartLPMode; // 正常工作模式或低功耗工作模式配置
        stcRunMode.enMode   = LPUartMode3;

        stcLPUartIrqCb.pfnRxIrqCb    = RxIntCallback;
        stcLPUartIrqCb.pfnTxIrqCb    = TxIntCallback;
        stcLPUartIrqCb.pfnRxErrIrqCb = ErrIntCallback;

        stcMulti.enMulti_mode = LPUartNormal; // 只有模式2/3才有多主机模式

        stcConfig.pstcLpuart_clk = &stcLpuart_clk;
        stcConfig.pstcRunMode    = &stcRunMode;
        stcConfig.pstcIrqCb      = &stcLPUartIrqCb;
        stcConfig.pstcMultiMode  = &stcMulti;
        stcConfig.bTouchNvic     = TRUE;
        LPUart_EnableIrq(LPUartRxIrq);
        LPUart_Init(&stcConfig);

        LPUart_EnableFunc(LPUartRx);
    }
}

uint8_t u8TxData[10] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55}; //"Xiaohua MCU!";
uint8_t u8RxData[10];
uint8_t u8TxCnt = 0, u8RxCnt = 0;

void TxIntCallback(void)
{
    if (u8TxCnt < 5) {
        M0P_LPUART->SBUF = u8RxData[u8TxCnt];

        u8TxCnt++;
    } else // if(u8TxCnt>10)
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
    if (u8RxCnt < 5) {
        u8RxData[u8RxCnt] = LPUart_ReceiveData();
        if (LPUart_CheckEvenOrOdd(Even, u8RxData[u8RxCnt]) != Ok) {
            u8RxCnt = 0;
        } else {
            u8RxCnt++;
        }
    } else {
        u8RxCnt = 0;
    }
}
void ErrIntCallback(void)
{
}




