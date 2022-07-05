#include "uart.h"

struct _Uart_Data CV_UART[5];

char UART_Send_Data(USART_TypeDef *Channel, const U8 *Data, int Length)
{
    int temp = Length, i = 0;
    if (Length > Length_MAX)
    {
        return 0;
    }
    assert_param(IS_USART_ALL_PERIPH(Channel));

    while (temp--)
    {
		while (USART_GetFlagStatus(Channel, USART_FLAG_TC) == RESET)	//完全发送完成;【USART_FLAG_TXE】这个至是说明，数据被cpu取走
			;		
        USART_SendData(Channel, Data[i++]);
        USART_ClearFlag(Channel, USART_FLAG_TC);
    }

    return 1;
}

void UART_Send_String(USART_TypeDef *Channel, const char *String)
{
    int Length = strlen(String),i = 0;
    if (Length > Length_MAX)
    {
        return;
    }
    assert_param(IS_USART_ALL_PERIPH(Channel));

    while (Length--)
    {
		while (USART_GetFlagStatus(Channel, USART_FLAG_TC) == RESET)
			;
        USART_SendData(Channel, String[i++]);
        USART_ClearFlag(Channel, USART_FLAG_TC);
    }
}

void NVIC_Configuration(void)
{

}

void Uart_Init(char Channel, int Baud,FunctionalState SET)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		 //设置中断组，4位抢占优先级，4位响应优先级
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); // AFIO复用功能模块时钟

	if (Channel == 1)
	{
		/*	Clock	*/
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // GPIOB 服用 PB6 PB7
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, SET);	  // USART1  (APB2)
		GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
		USART_DeInit(USART1);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; // TXD
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	  // RXD
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		USART_InitStructure.USART_BaudRate = Baud;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //
		USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;					//
		USART_InitStructure.USART_Parity = USART_Parity_No;								//
		USART_InitStructure.USART_StopBits = USART_StopBits_1;							//
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//
		USART_Init(USART1, &USART_InitStructure);
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //
		USART_Cmd(USART1, SET);						   //

		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		  //响应优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}
	else if (Channel == 2)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, SET);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;		// USART2 TX；
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出；
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);				  //端口A；
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;			  // USART2 RX；
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入；
		GPIO_Init(GPIOA, &GPIO_InitStructure);				  //端口A；

		USART_InitStructure.USART_BaudRate = Baud;					//波特率；
		USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位；
		USART_InitStructure.USART_StopBits = USART_StopBits_1;		//停止位1位；
		USART_InitStructure.USART_Parity = USART_Parity_No;			//无校验位；
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(USART2, &USART_InitStructure);		//配置串口参数；
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置中断组，4位抢占优先级，4位响应优先级；

		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;		  //中断号；
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级；
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  //响应优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
		USART_Cmd(USART2, SET); //使能串口
	}

	else if (Channel == 3)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, SET);
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
		USART_Cmd(USART3, SET);
	}
	else if (Channel == 4)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, SET);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // UART4 TX；

		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出；
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC, &GPIO_InitStructure); //端口C；

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;			  // UART4 RX；
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
		GPIO_Init(GPIOC, &GPIO_InitStructure);

		USART_InitStructure.USART_BaudRate = 9600;					//波特率
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

		USART_Cmd(UART4, SET); //使能串口
	}
	else if (Channel == 5)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, SET);

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
		USART_Cmd(UART5, SET);						  //使能串口
	}
    else
        return;
}

void Get_RXD(struct _Uart_Data *Target, char res) //接收处理函数
{
    if (!Target->Rxd_Received)
    {
        Target->DATA.Buff[Target->DATA.Length++] = res;
        if (END_Data != 'N')
        {
            if (res == END_Data) //停止位
            {
                Target->Rxd_Received = 1;
            }
        }

        if (Target->DATA.Length > Length_MAX) //超长（异常需要清零）
        {
            Target->Rxd_Received = 1;
        }
    }
}

void USART1_IRQHandler(void)
{
    char temp;
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        USART_ClearFlag(USART1, USART_IT_RXNE);
        temp = USART_ReceiveData(USART1);
        Get_RXD(&CV_UART[1], temp);
        //USART_SendData(USART1, temp);               //debug
    }
}

void USART3_IRQHandler(void)
{
    char temp;
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        USART_ClearFlag(USART3, USART_IT_RXNE);
        temp = USART_ReceiveData(USART3);
        Get_RXD(&CV_UART[3], temp);
        //USART_SendData(USART3, temp);               //debug
    }
}

void UART4_IRQHandler(void)
{
    char temp;
    if (USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
    {
        USART_ClearFlag(UART4, USART_IT_RXNE);
        temp = USART_ReceiveData(UART4);
        Get_RXD(&CV_UART[4], temp);
        //USART_SendData(UART4, temp);               //debug
    }
}




//#include "sys_time.h"
//#include "Data_Handle.h"

//struct _CV_UART CV_UART;
//struct UART_ UART;
////static unsigned int Overflow_Time;		//暂时没用

//void UART_x_Init(char UART_x, uint32_t Baud, FunctionalState SET)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	USART_InitTypeDef USART_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		 //设置中断组，4位抢占优先级，4位响应优先级
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); // AFIO复用功能模块时钟
//	UART.UART_x_Send_Data = UART_x_Send_Data;
//	UART.UART_x_Send_String = UART_x_Send_String;
//	if (UART_x == 1)
//	{
//		/*	Clock	*/
//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // GPIOB 服用 PB6 PB7
//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, SET);	  // USART1  (APB2)
//		GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
//		USART_DeInit(USART1);

//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; // TXD
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //
//		GPIO_Init(GPIOB, &GPIO_InitStructure);
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	  // RXD
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //
//		GPIO_Init(GPIOB, &GPIO_InitStructure);

//		USART_InitStructure.USART_BaudRate = Baud;
//		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //
//		USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;					//
//		USART_InitStructure.USART_Parity = USART_Parity_No;								//
//		USART_InitStructure.USART_StopBits = USART_StopBits_1;							//
//		USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//
//		USART_Init(USART1, &USART_InitStructure);
//		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //
//		USART_Cmd(USART1, SET);						   //

//		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		  //响应优先级
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//		NVIC_Init(&NVIC_InitStructure);
//	}
//	else if (UART_x == 2)
//	{
//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, SET);

//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;		// USART2 TX；
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出；
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_Init(GPIOA, &GPIO_InitStructure);				  //端口A；
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;			  // USART2 RX；
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入；
//		GPIO_Init(GPIOA, &GPIO_InitStructure);				  //端口A；

//		USART_InitStructure.USART_BaudRate = Baud;					//波特率；
//		USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位；
//		USART_InitStructure.USART_StopBits = USART_StopBits_1;		//停止位1位；
//		USART_InitStructure.USART_Parity = USART_Parity_No;			//无校验位；
//		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//		USART_Init(USART2, &USART_InitStructure);		//配置串口参数；
//		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置中断组，4位抢占优先级，4位响应优先级；

//		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;		  //中断号；
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级；
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  //响应优先级
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//		NVIC_Init(&NVIC_InitStructure);

//		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
//		USART_Cmd(USART2, SET); //使能串口
//	}

//	else if (UART_x == 3)
//	{
//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, SET);
//		GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE); //串口3重映射，与串口4 IO一致（STM32F103RB版本只有串口3没有4）

//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;		// USART3 TX；
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_Init(GPIOC, &GPIO_InitStructure);
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;			  // USART3 RX；
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
//		GPIO_Init(GPIOC, &GPIO_InitStructure);

//		USART_InitStructure.USART_BaudRate = Baud;					//波特率；
//		USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位；
//		USART_InitStructure.USART_StopBits = USART_StopBits_1;		//停止位1位；
//		USART_InitStructure.USART_Parity = USART_Parity_No;			//无校验位；
//		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

//		USART_Init(USART3, &USART_InitStructure);				  //配置串口参数
//		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);			  //设置中断组，4位抢占优先级，4位响应优先级；
//		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;		  //中断号；
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级；
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		  //响应优先级；
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//		NVIC_Init(&NVIC_InitStructure);

//		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
//		USART_Cmd(USART3, SET);
//	}
//	else if (UART_x == 4)
//	{
//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, SET);
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // UART4 TX；

//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出；
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_Init(GPIOC, &GPIO_InitStructure); //端口C；

//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;			  // UART4 RX；
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
//		GPIO_Init(GPIOC, &GPIO_InitStructure);

//		USART_InitStructure.USART_BaudRate = 9600;					//波特率
//		USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位
//		USART_InitStructure.USART_StopBits = USART_StopBits_1;		//停止位1位
//		USART_InitStructure.USART_Parity = USART_Parity_No;			//无校验位
//		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//		USART_Init(UART4, &USART_InitStructure); //配置串口参数

//		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);			  //设置中断组，4位抢占优先级，4位响应优先级
//		NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;		  //中断号
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  //响应优先级
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//		NVIC_Init(&NVIC_InitStructure);
//		USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);

//		USART_Cmd(UART4, SET); //使能串口
//	}
//	else if (UART_x == 5)
//	{
//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, SET);

//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;		// UART5 TX；
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出；
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_Init(GPIOC, &GPIO_InitStructure);
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;			  // UART5 RX；
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
//		GPIO_Init(GPIOD, &GPIO_InitStructure);

//		USART_InitStructure.USART_BaudRate = Baud;										// 波特率
//		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 硬件流控制
//		USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;					// 发送 接收 模式都使用
//		USART_InitStructure.USART_Parity = USART_Parity_No;								// 没有奇偶校验
//		USART_InitStructure.USART_StopBits = USART_StopBits_1;							// 一位停止位
//		USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// 每次发送数据宽度为8位
//		USART_Init(UART5, &USART_InitStructure);										//配置串口参数

//		NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;		  //中断号
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		  //响应优先级
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//		NVIC_Init(&NVIC_InitStructure);

//		USART_ITConfig(UART5, USART_IT_RXNE, ENABLE); //开启接收中断
//		USART_Cmd(UART5, SET);						  //使能串口
//	}
//}

//void UART_x_Send_Data(char UART_x, char *Data, unsigned char Length)
//{
//	int n = 0;
//	if(Length > 100)	return;
//	while (Length--)
//	{
//		switch (UART_x)
//		{
//		case 1:
//			while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
//				;
//			USART_SendData(USART1, *(Data + n));
//			break;
//		case 2:
//			while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
//				;
//			USART_SendData(USART2, *(Data + n));
//			break;
//		case 3:
//			while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
//				;
//			USART_SendData(USART3, *(Data + n));
//			break;
//		case 4:
//			while (USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET)
//				;
//			USART_SendData(UART4, *(Data + n));
//			break;
//		case 5:
//			while (USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET)
//				;
//			USART_SendData(UART5, *(Data + n));
//			break;
//		default:
//			break;
//		}
//		n++;
//	}
//}

//void UART_x_Send_String(char UART_x, char *String)
//{
//	int n = 0;
//	while (String_Num_MAX - n) //最长
//	{
//		switch (UART_x)
//		{
//		case 1:
//			while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
//				;
//			USART_SendData(USART1, *(String + n));
//			break;
//		case 2:
//			while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
//				;
//			USART_SendData(USART2, *(String + n));
//			break;
//		case 3:
//			while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
//				;
//			USART_SendData(USART3, *(String + n));
//			break;
//		case 4:
//			while (USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET)
//				;
//			USART_SendData(UART4, *(String + n));
//			break;
//		case 5:
//			while (USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET)
//				; // USART_FLAG_TXE 寄存器空标志位，USART_FLAG_TC 硬件 TXD IO结束 标志位
//			USART_SendData(UART5, *(String + n));
//			break;
//		default:
//			break;
//		}
//		if (*(String + n) == String_End1 || *(String + n) == String_End2)			// 字符串结束字符
//			return;
//		n++;
//	}
//}

//void Read_RXD(char UART_x,char res)			//查看能不能接收
//{
//	if (CV_UART.Read_Flag[UART_x] == 0)
//	{
//		CV_UART.UART_x_Array[UART_x][CV_UART.Rxd_Num[UART_x]++] = res;
//		if (res == RXD_End1 || res == RXD_End2)		//停止位
//		{
//			CV_UART.Read_Flag[UART_x] = 1;
//			CV_UART.Rxd_Num[UART_x] = 0;
//		}
//		if(CV_UART.Rxd_Num[UART_x] > RXD_Num_MAX)	//超长（异常需要清零）
//		{
//			CV_UART.Read_Flag[UART_x] = 1;
//			CV_UART.Rxd_Num[UART_x] = 0;
//			Data_Replace("123",CV_UART.UART_x_Array[UART_x],0,sizeof(CV_UART.UART_x_Array[UART_x]));
//		}
//	}
//}

//void USART1_IRQHandler(void) //中断处理函数；
//{
//	char res;
//	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
//	{
//		USART_ClearFlag(USART1, USART_IT_RXNE);
//		res = USART_ReceiveData(USART1); //接收数据
//		Read_RXD(UART_1,res);			//查看能不能接收
//	}
//}

//void USART2_IRQHandler(void) //中断处理函数；
//{
//	char res;
//	if (USART_GetITStatus(USART2, USART_IT_RXNE)) //判断是否发生中断；
//	{
//		USART_ClearFlag(USART2, USART_IT_RXNE); //清除标志位
//		res = USART_ReceiveData(USART2); //接收数据
//		Read_RXD(UART_2,res);			//查看能不能接收
//	}
//}

//void USART3_IRQHandler(void) //中断处理函数
//{
//	char res;
//	if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET) //判断是否发生中断
//	{
//		USART_ClearFlag(USART3, USART_IT_RXNE); //清除标志位
//		res = USART_ReceiveData(USART3); //接收数据
//		Read_RXD(UART_3,res);			//查看能不能接收
//	}
//}

//void UART4_IRQHandler(void) //中断处理函数
//{
//	char res;
//	if (USART_GetITStatus(UART4, USART_IT_RXNE)) //判断是否发生中断；
//	{
//		USART_ClearFlag(UART4, USART_IT_RXNE); //清除标志位
//		res = USART_ReceiveData(UART4); //接收数据
//		Read_RXD(UART_4,res);			//查看能不能接收
//	}
//}

//void UART5_IRQHandler(void) //中断处理函数；
//{
//	char res;
//	if (USART_GetITStatus(UART5, USART_IT_RXNE)) //判断是否发生中断；
//	{
//		USART_ClearFlag(UART5, USART_IT_RXNE); //清除标志位
//		res = USART_ReceiveData(UART5); //接收数据
//		Read_RXD(UART_5,res);			//查看能不能接收
//	}
//}
