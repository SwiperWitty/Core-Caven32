#include "Base_UART.h" 
#include "uart.h"
#include "bt.h"

#ifdef Exist_UART
static uint8_t Temp;

#endif


void Uart0_Init(int Baud,int Set)
{
#ifdef UART1_EXIST
   uint16_t timer;
    uint32_t pclk;
    stc_clk_config_t stcCfg;
    stc_uart_config_t  stcConfig;
    stc_uart_irq_cb_t stcUartIrqCb;
    stc_uart_multimode_t stcMulti;
    stc_uart_baud_config_t stcBaud;
    stc_bt_config_t stcBtConfig;

    en_uart_mmdorck_t enTb8;

    DDL_ZERO_STRUCT(stcConfig);
    DDL_ZERO_STRUCT(stcUartIrqCb);
    DDL_ZERO_STRUCT(stcMulti);
    DDL_ZERO_STRUCT(stcBaud);
    DDL_ZERO_STRUCT(stcBtConfig);
    //时钟部分配置
    stcCfg.enClkSrc = ClkXTH;
    stcCfg.enHClkDiv = ClkDiv1;
    stcCfg.enPClkDiv = ClkDiv1;
    Clk_Init(&stcCfg);
    
    Gpio_InitIOExt(3,5,GpioDirOut,TRUE,FALSE,FALSE,FALSE); 
    Gpio_InitIOExt(3,6,GpioDirOut,TRUE,FALSE,FALSE,FALSE);
    
    //通道端口配置
    Gpio_SetFunc_UART1TX_P35();
    Gpio_SetFunc_UART1RX_P36();

    //外设时钟使能
    Clk_SetPeripheralGate(ClkPeripheralBt,TRUE);
    Clk_SetPeripheralGate(ClkPeripheralUart1,TRUE);

    stcUartIrqCb.pfnRxIrqCb = NULL;
    stcUartIrqCb.pfnTxIrqCb = NULL;
    stcUartIrqCb.pfnRxErrIrqCb = NULL;
    stcConfig.pstcIrqCb = &stcUartIrqCb;
    stcConfig.bTouchNvic = FALSE;

    stcConfig.enRunMode = UartMode3;
    stcMulti.enMulti_mode = UartMulti;
    enTb8 = Addr;
    Uart_SetMMDOrCk(UARTCH1,enTb8);
    stcConfig.pstcMultiMode = &stcMulti;
    
    stcBaud.bDbaud = 0u;
    stcBaud.u32Baud = 2400u;
    stcBaud.u8Mode = UartMode3; //计算波特率需要模式参数
    pclk = Clk_GetPClkFreq();
    timer=Uart_SetBaudRate(UARTCH1,pclk,&stcBaud);
  
    stcBtConfig.enMD = BtMode2;
    stcBtConfig.enCT = BtTimer;
    Bt_Init(TIM1, &stcBtConfig);//调用basetimer1设置函数产生波特率
    Bt_ARRSet(TIM1,timer);
    Bt_Cnt16Set(TIM1,timer);
    Bt_Run(TIM1);

    Uart_Init(UARTCH1, &stcConfig);
    Uart_ClrStatus(UARTCH1,UartTxEmpty);//清所有中断请求
    Uart_EnableFunc(UARTCH1,UartRx);   

    Uart_SendData(UARTCH1,0xC0);
    enTb8 = Data;
    Uart_SetMMDOrCk(UARTCH1,enTb8);
    Uart_ClrStatus(UARTCH1,UartTxEmpty);

#endif
}

void Uart1_Init(int Baud,int Set)
{    
#ifdef UART2_EXIST
	
#endif
}



char UART_RXD_Flag(char Channel)
{
    char res;
    switch (Channel)
    {
    case 0:
#ifdef UART1_EXIST
        Temp = UARTCH0;
#endif
        break;
    case 1:
#ifdef UART2_EXIST
        Temp = UARTCH1;
#endif
        break;

    default:
        return (0);
    }
#ifdef Exist_UART
//    res = Uart_ClrStatus(Temp,UartTxEmpty);
#endif
    return res;
}

void UART_RXD_Flag_Clear(char Channel)
{
    switch (Channel)
    {
    case 0:
#ifdef UART1_EXIST
        Temp = UARTCH0;
#endif
        break;
    case 1:
#ifdef UART2_EXIST
        Temp = UARTCH1;
#endif
        break;

    default:
        return ;
    }
#ifdef Exist_UART
    Uart_ClrStatus(Temp,UartRxFull);
#endif
    return;
}

/*  发送 接收    */

// 接收
uint16_t UART_RXD_Receive(char Channel)     //RXD 读取值
{
    uint16_t res;
    switch (Channel)
    {
    case 0:
#ifdef UART1_EXIST
        Temp = UARTCH0;
#endif
        break;
    case 1:
#ifdef UART2_EXIST
        Temp = UARTCH1;
#endif
        break;
	
	default:
        return (0);
    }
    #ifdef Exist_UART
//    
    #endif
    return res;
    
}

// 发送
void UART_TXD_Send(char Channel,uint16_t Data)
{
    switch (Channel)
    {
    case 0:
#ifdef UART1_EXIST
        Temp = UARTCH0;
#endif
        break;
    case 1:
#ifdef UART2_EXIST
        Temp = UARTCH1;
#endif
        break;
	
	default:
        return ;
	
//error,直接返回
    }
#ifdef Exist_UART
    Uart_SendData(Temp,DATA);
	Uart_ClrStatus(UARTCH1,UartTxEmpty); 
#endif
}

#ifdef UART_pf
int fputc(int ch, FILE *f)      //printf
{
#ifdef DEBUG_OUT
    #ifdef Exist_UART
//    USART_SendData(USART1,(uint8_t)ch);
//    while (!USART_GetFlagStatus(USART1, TXD_Falg));
    UART_TXD_Send(DEBUG_OUT,(uint8_t)ch);
    #endif
#endif // DEBUG
    return (ch);
}
#endif

//你找中断？UART的中断权限给MODE了！

