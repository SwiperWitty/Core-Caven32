#include "uart.h"

void Uart1_Init(int Baud,int SET)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
	USART_InitTypeDef USART_InitStructure = {0};
	NVIC_InitTypeDef NVIC_InitStructure = {0};
	USART_TypeDef * UART_Temp = USART1;
	FunctionalState temp;

    if(SET)
        temp = ENABLE;
    else
        temp = DISABLE;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		 //设置中断组，4位抢占优先级，4位响应优先级
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, temp);	  // USART1  (APB2)
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); // AFIO复用功能模块时钟(暂不需要)
//  GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);     //端口复用
    USART_DeInit(UART_Temp);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;      // RXD
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;       // TXD
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    if (!temp)
    {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    }
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = Baud;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;					//
    USART_InitStructure.USART_Parity = USART_Parity_No;								//
    USART_InitStructure.USART_StopBits = USART_StopBits_1;							//
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//
    USART_Init(UART_Temp, &USART_InitStructure);
    USART_ITConfig(UART_Temp, RXD_Falg, temp);                                 //

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		  //响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = temp;
    NVIC_Init(&NVIC_InitStructure);
    
    USART_Cmd(UART_Temp, temp);
}
void Uart2_Init(int Baud,int SET)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
    
    FunctionalState temp;
    if(SET)
        temp = ENABLE;
    else
        temp = DISABLE;
    
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		 //设置中断组，4位抢占优先级，4位响应优先级
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); // AFIO复用功能模块时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //  PA02 PA03
    RCC_APB2PeriphClockCmd(RCC_APB1Periph_USART2, temp);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;		// USART2 TX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出；
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);				  //端口A；
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;			  // USART2 RX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入；
    GPIO_Init(GPIOA, &GPIO_InitStructure);				  //端口A；
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    if (!temp)
    {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    }
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = Baud;					//波特率；
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位；
    USART_InitStructure.USART_StopBits = USART_StopBits_1;		//停止位1位；
    USART_InitStructure.USART_Parity = USART_Parity_No;			//无校验位；
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);		//配置串口参数
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置中断组，4位抢占优先级，4位响应优先级；
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;		  //中断号；
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级；
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  //响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART2, temp); //使能串口
}
void Uart3_Init(int Baud,int SET)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
    
    FunctionalState temp;
    if(SET)
        temp = ENABLE;
    else
        temp = DISABLE;
    
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		 //设置中断组，4位抢占优先级，4位响应优先级
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); // AFIO复用功能模块时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);   // PC10 PC11
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, temp);
    GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE); //串口3重映射，与串口4 IO一致（STM32F103RB版本只有串口3没有4）

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;		// USART3 TX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;			  // USART3 RX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = Baud;					//波特率；
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位；
    USART_InitStructure.USART_StopBits = USART_StopBits_1;		//停止位1位；
    USART_InitStructure.USART_Parity = USART_Parity_No;			//无校验位；
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStructure);				  //配置串口参数
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);			  //设置中断组，4位抢占优先级，4位响应优先级；
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;		  //中断号；
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级；
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		  //响应优先级；
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART3, temp);
}
#if UART_Channel_MAX >= 4
void Uart4_Init(int Baud,int SET)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
    
    FunctionalState temp;
    if(SET)
        temp = ENABLE;
    else
        temp = DISABLE;
    
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		 //设置中断组，4位抢占优先级，4位响应优先级
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); // AFIO复用功能模块时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);   // PC10 PC11
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, temp);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // UART4 TX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出；
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure); //端口C；
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;			  // UART4 RX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = Baud;					//波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;		//停止位1位
    USART_InitStructure.USART_Parity = USART_Parity_No;			//无校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(UART4, &USART_InitStructure); //配置串口参数

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);			  //设置中断组，4位抢占优先级，4位响应优先级
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;		  //中断号
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  //响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);

    USART_Cmd(UART4, temp); //使能串口
}
#endif
#if UART_Channel_MAX >= 5
void Uart5_Init(int Baud,int SET)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

    FunctionalState temp;
    if(SET)
        temp = ENABLE;
    else
        temp = DISABLE;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		 //设置中断组，4位抢占优先级，4位响应优先级
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); // AFIO复用功能模块时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);    // PC12 PD02
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, temp);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;		// UART5 TX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出；
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;			  // UART5 RX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = Baud;										// 波特率
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 硬件流控制
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;					// 发送 接收 模式都使用
    USART_InitStructure.USART_Parity = USART_Parity_No;								// 没有奇偶校验
    USART_InitStructure.USART_StopBits = USART_StopBits_1;							// 一位停止位
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// 每次发送数据宽度为8位
    USART_Init(UART5, &USART_InitStructure);										//配置串口参数

    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;		  //中断号
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		  //响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(UART5, USART_IT_RXNE, ENABLE); //开启接收中断
    USART_Cmd(UART5, temp);						  //使能串口
}
#endif

char UART_RXD_Flag(char Channel)
{
    char res;
    USART_TypeDef * Temp;
    switch (Channel)
    {
    case 1:
        Temp = USART1;
        break;
    case 2:
        Temp = USART2;
        break;
    case 3:
        Temp = USART3;
        break;
#if UART_Channel_MAX >= 4
    case 4:
        Temp = UART4;
        break; 
#endif
#if UART_Channel_MAX >= 5
    case 5:
        Temp = UART5;
        break;
#endif
    default:
        return 0;
    }
    res = USART_GetITStatus(Temp,RXD_Falg);
    return res;
}

void UART_RXD_Flag_Clear(char Channel)
{
    USART_TypeDef * Temp;
    switch (Channel)
    {
    case 1:
        Temp = USART1;
        break;
    case 2:
        Temp = USART2;
        break;
    case 3:
        Temp = USART3;
        break;
#if UART_Channel_MAX >= 4
    case 4:
        Temp = UART4;
        break; 
#endif
#if UART_Channel_MAX >= 5
    case 5:
        Temp = UART5;
        break;
#endif
    default:
        return;
    }
    USART_ClearFlag(Temp, RXD_Falg);
}

uint16_t UART_RXD_Receive(char Channel)     //RXD 读取值
{
    uint16_t res;
    USART_TypeDef * Temp;
    switch (Channel)
    {
    case 1:
        Temp = USART1;
        break;
    case 2:
        Temp = USART2;
        break;
    case 3:
        Temp = USART3;
        break;
#if UART_Channel_MAX >= 4
    case 4:
        Temp = UART4;
        break; 
#endif
#if UART_Channel_MAX >= 5
    case 5:
        Temp = UART5;
        break;
#endif
    default:
        return 0;
    }
    res = USART_ReceiveData(Temp);
    return res;
}

void UART_TXD_Send(char Channel,uint16_t DATA)
{
    USART_TypeDef * Temp;
    switch (Channel)
    {
    case 1:
        Temp = USART1;
        break;
    case 2:
        Temp = USART2;
        break;
    case 3:
        Temp = USART3;
        break;
#if UART_Channel_MAX >= 4
    case 4:
        Temp = UART4;
        break; 
#endif
#if UART_Channel_MAX >= 5
    case 5:
        Temp = UART5;
        break;
#endif
    default:
        return;
    }
    while (USART_GetFlagStatus(Temp, TXD_Falg) == 0);  
    USART_SendData(Temp, DATA);
    USART_ClearFlag(Temp, TXD_Falg);
}
