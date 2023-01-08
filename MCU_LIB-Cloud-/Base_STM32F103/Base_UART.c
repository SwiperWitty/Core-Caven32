#include "Base_UART.h" 

#ifdef Exist_UART
static USART_TypeDef * Temp;

#endif

void Uart1_Init(int Baud,int Set)
{
#ifdef UART1_EXIST
    FunctionalState set = DISABLE;
    Temp = USART1;
    USART_DeInit(Temp);
    if (Set)
        set = ENABLE;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure; //
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 		//设置中断组，4位抢占优先级，4位响应优先级
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//AFIO复用功能模块时钟

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
//    GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);		//串口1重映射
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //TXD
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	  //RXD
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /***/
    USART_InitStructure.USART_BaudRate = Baud; //波特率；
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位；
    USART_InitStructure.USART_StopBits = USART_StopBits_1; //停止位1位；
    USART_InitStructure.USART_Parity = USART_Parity_No ; //无校验位；
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//
    USART_Init(Temp, &USART_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		  //响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    USART_ITConfig(Temp, USART_IT_RXNE, set);   //
    USART_Cmd(Temp, set);					    //
#endif
}

void Uart2_Init(int Baud,int Set)
{    
#ifdef UART2_EXIST
    FunctionalState set = DISABLE;
    Temp = USART2;
    USART_DeInit(Temp);
    if (Set)
        set = ENABLE;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure; //
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 		//设置中断组，4位抢占优先级，4位响应优先级
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//AFIO复用功能模块时钟

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //USART2 TX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出；
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure); //端口A；
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; //USART2 RX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入；
    GPIO_Init(GPIOA, &GPIO_InitStructure); //端口A；
    
    /***/
    USART_InitStructure.USART_BaudRate = Baud; //波特率；
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位；
    USART_InitStructure.USART_StopBits = USART_StopBits_1; //停止位1位；
    USART_InitStructure.USART_Parity = USART_Parity_No ; //无校验位；
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//
    USART_Init(Temp, &USART_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		  //响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    USART_ITConfig(Temp, USART_IT_RXNE, set);   //
    USART_Cmd(Temp, set);					    //
#endif
}

void Uart3_Init(int Baud,int Set)
{
#ifdef UART3_EXIST
    FunctionalState set = DISABLE;
    Temp = USART3;
    USART_DeInit(Temp);
    if (Set)
        set = ENABLE;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure; //
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 		//设置中断组，4位抢占优先级，4位响应优先级
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//AFIO复用功能模块时钟

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);		//串口3重映射，与串口4 IO一致（STM32F103RB版本只有串口3没有4）
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //USART3 TX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //USART3 RX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    /***/
    USART_InitStructure.USART_BaudRate = Baud; //波特率；
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位；
    USART_InitStructure.USART_StopBits = USART_StopBits_1; //停止位1位；
    USART_InitStructure.USART_Parity = USART_Parity_No ; //无校验位；
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//
    USART_Init(Temp, &USART_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  //响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    USART_ITConfig(Temp, USART_IT_RXNE, set);   //
    USART_Cmd(Temp, set);					    //
#endif
}

void Uart4_Init(int Baud,int Set)
{
#ifdef UART4_EXIST

#endif
}

void Uart5_Init(int Baud,int Set)
{
#ifdef UART5_EXIST
    confirm_state set = FALSE;
    Temp = UART4;
    usart_reset(Temp);
    if (Set)
        set = TRUE;
    crm_periph_clock_enable(CRM_UART4_PERIPH_CLOCK, set);
    crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);

    gpio_pin_remap_config(SWJTAG_GMUX_010,TRUE);                //支持 SWD，禁用 JTAG，PA15/PB3/PB4 可作GPIO
#endif
}

char UART_RXD_Flag(char Channel)
{
    char res;
    switch (Channel)
    {
    case 1:
#ifdef UART1_EXIST
        Temp = USART1;
#endif
        break;
    case 2:
#ifdef UART2_EXIST
        Temp = USART2;
#endif
        break;
    case 3:
#ifdef UART3_EXIST
        Temp = USART3;
#endif
        break;
    case 4:
#ifdef UART4_EXIST
        Temp = UART4;
#endif
        break;
    case 5:
#ifdef UART5_EXIST
        Temp = UART5;
#endif
        break;
    default:
        return (0);
    }
#ifdef Exist_UART
    res = USART_GetITStatus(Temp,RXD_Falg);
#endif
    return res;
}

void UART_RXD_Flag_Clear(char Channel)
{
    switch (Channel)
    {
    case 1:
#ifdef UART1_EXIST
        Temp = USART1;
#endif
        break;
    case 2:
#ifdef UART1_EXIST
        Temp = USART2;
#endif
        break;
    case 3:
#ifdef UART3_EXIST
        Temp = USART3;
#endif
        break;
    case 4:
#ifdef UART4_EXIST
        Temp = UART4;
#endif
        break;
    case 5:
#ifdef UART5_EXIST
        Temp = UART5;
#endif
        break;
    default:
        return;
    }
#ifdef Exist_UART
    USART_ClearFlag(Temp, RXD_Falg);
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
    case 1:
#ifdef UART1_EXIST
        Temp = USART1;
#endif
        break;
    case 2:
#ifdef UART1_EXIST
        Temp = USART2;
#endif
        break;
    case 3:
#ifdef UART3_EXIST
        Temp = USART3;
#endif
        break;
    case 4:
#ifdef UART4_EXIST
        Temp = UART4;
#endif
        break;
    case 5:
#ifdef UART5_EXIST
        Temp = UART5;
#endif
        break;
    default:
        break;
    }
    #ifdef Exist_UART
    res = USART_ReceiveData(Temp);
    #endif
    return res;
    
}

// 发送
void UART_TXD_Send(char Channel,uint16_t DATA)
{
    switch (Channel)
    {
    case 1:
#ifdef UART1_EXIST
        Temp = USART1;
#endif
        break;
//USART1
    case 2:
#ifdef UART2_EXIST
        Temp = USART2;
#endif
        break;
//USART2
    case 3:
#ifdef UART3_EXIST
        Temp = USART3;
#endif
        break;
//USART3
    case 4:
#ifdef UART4_EXIST
        Temp = UART4;
#endif
        break;
//UART4
    case 5:
#ifdef UART5_EXIST
        Temp = UART5;
#endif
        break;
//UART5
    default:
        return;
//error,直接返回
    }
#ifdef Exist_UART
    while (USART_GetFlagStatus(Temp, TXD_Falg) == RESET);  
    USART_SendData(Temp, DATA);
    // usart_flag_clear(Temp, TXD_Falg);        //可以不要
#endif
}

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

