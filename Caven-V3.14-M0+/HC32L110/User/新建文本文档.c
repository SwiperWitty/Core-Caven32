/** \file main.c
 **
 ** A detailed description is available at
 ** @link Sample Group Some description @endlink
 **
 **   - 2016-02-16  1.0  XYZ First version for Device Driver Library of Module.
 **
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "ddl.h"
#include "lpt.h"
#include "lpm.h"
#include "gpio.h"

volatile int u32LptTestFlag = 0;

void LptInt(void)
{
    if (TRUE == Lpt_GetIntFlag())
    {
        Lpt_ClearIntFlag();
        u32LptTestFlag = 0x01;
    }
}

/*******************************************************************************
 * Lpt定时功能测试 （重载模式）
 ******************************************************************************/
en_result_t LptTimerTest(void)
{
    stc_lpt_config_t stcConfig;
    en_result_t      enResult = Error;
    uint16_t         u16ArrData = 0;
 
    stc_lpm_config_t stcLpmCfg;
    
	Gpio_InitIO(0, 3, GpioDirIn);	//!!!!
    Gpio_InitIO(2, 5, GpioDirIn);
    Gpio_InitIO(2, 6, GpioDirOut);
    Gpio_SetIO(2, 6, FALSE);		// on
    
    stcConfig.enGateP  = LptPositive; 
    stcConfig.enGate   = LptGateDisable;
    stcConfig.enTckSel = LptIRC32K;
    stcConfig.enTog    = LptTogDisable;
    stcConfig.enCT     = LptTimer;
    stcConfig.enMD     = LptMode2;
    
    stcConfig.pfnLpTimCb = LptInt;
    
    if (Ok != Lpt_Init(&stcConfig))
    {
        enResult = Error;
    }
    
    //Lpm Cfg
    stcLpmCfg.enSEVONPEND   = SevPndDisable;
    stcLpmCfg.enSLEEPDEEP   = SlpDpEnable;
    stcLpmCfg.enSLEEPONEXIT = SlpExtDisable;
    Lpm_Config(&stcLpmCfg);
    
    //Lpt 中断使能
    Lpt_ClearIntFlag();
    Lpt_EnableIrq();
    EnableNvic(LPTIM_IRQn, 3, TRUE);
    
    
    //设置重载值，计数初值，启动计数
    Lpt_ARRSet(u16ArrData);
    Lpt_Run();
    
    
    //判断P03，如果为高电平则，进入低功耗模式……
    //注：若芯片处于低功耗模式，则芯片无法使用SWD进行调式和下载功能。
    //故如需要继续下载调试其他程序，需要将P25接低电平。
#if 0
    if (TRUE == Gpio_GetIO(2, 5))
    {
        Gpio_SetIO(2, 6, TRUE);
        Lpm_GotoLpmMode();
    }
#endif    

    do {
//        Base_NOP_Delay(10, 10);
    } while (1 == Gpio_GetIO(0,3)); // 注意：此处用户不能屏蔽，否则进入深度休眠模式导致SWD连接不上 //
    //	//!!!!
	Gpio_SetIO(2, 6, TRUE);		//off
	Lpm_GotoLpmMode();
    while(1)
    {
        if (0x01 == u32LptTestFlag)
        {
            u32LptTestFlag = 0;
            Lpt_Stop();
            Gpio_SetIO(2, 6, FALSE);
			Base_NOP_Delay(1000, 100);
            enResult = Ok;
            Gpio_SetIO(2, 6, TRUE);
            Lpm_GotoLpmMode();

            // break;
        }
    }
    
    return enResult;
}


/**
 ******************************************************************************
 ** \brief  Main function of project
 **
 ** \return uint32_t return value, if needed
 **
 ** This sample
 **
 ******************************************************************************/

int32_t main(void)
{
    volatile uint8_t u8TestFlag = 0;
    
    //CLK INIT
    stc_clk_config_t stcClkCfg;
    stcClkCfg.enClkSrc  = ClkRCH;
    stcClkCfg.enHClkDiv = ClkDiv1;
    stcClkCfg.enPClkDiv = ClkDiv1;
    
    Clk_Init(&stcClkCfg);
    
    //使能RCL
    Clk_Enable(ClkRCL, TRUE);
    //使能Lpt、GPIO外设时钟
    Clk_SetPeripheralGate(ClkPeripheralLpTim, TRUE);
    Clk_SetPeripheralGate(ClkPeripheralGpio, TRUE);

    if (Ok != LptTimerTest())
    {
        u8TestFlag |= 0x02;
    }
        
    while (1);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
