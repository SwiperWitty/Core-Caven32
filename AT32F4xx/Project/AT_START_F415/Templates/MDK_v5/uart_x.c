#include "uart_x.h"

struct _uart_x Uart_4;

void UARTx_Send_Data(char Channel,const unsigned char * Data,int Length)
{
    int temp = 0;
    if(Channel == 4)
    {
        while(temp < Length)
        {
            USART_SendData(UART4,Data[temp++]);
            while(!USART_GetFlagStatus(UART4,USART_FLAG_TRAC));
            USART_ClearFlag(UART4, USART_FLAG_TRAC);
        }
    }
}

void UARTx_Send_String(char Channel,const char * String)
{
    int temp = 0;
    if(Channel == 4)
    {
        do{
            USART_SendData(UART4,String[temp]);
            while(!USART_GetFlagStatus(UART4,USART_FLAG_TRAC));
            USART_ClearFlag(UART4, USART_FLAG_TRAC);
        }while(String[++temp] != '\0');
    }
}

void NVIC_Configuration(void)
{
    NVIC_InitType NVIC_InitStructure;

    /* Configure the NVIC Preemption Priority Bits */  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    /* Enable the USART2 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void UARTx_Init(char Channel,int bound)
{
    if (Channel == 4)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOC, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_UART4, ENABLE);
        NVIC_Configuration();
        GPIO_InitType GPIO_InitStructure;

        GPIO_InitStructure.GPIO_Pins = GPIO_Pins_11;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOC, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
        GPIO_InitStructure.GPIO_Pins = GPIO_Pins_10;
        GPIO_Init(GPIOC, &GPIO_InitStructure);

        USART_InitType USART_InitStructure;
        USART_StructInit(&USART_InitStructure);
        USART_InitStructure.USART_BaudRate = bound;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	

        USART_Init(UART4, &USART_InitStructure); 
        USART_INTConfig(UART4, USART_INT_RDNE, ENABLE);
        USART_Cmd(UART4, ENABLE);
    }
    else
        return;

}

extern struct _Agreement Get_DATA;                 //接收数据（框架）

void UART4_IRQHandler(void)
{
	char res;
	if(USART_GetITStatus(UART4, USART_INT_RDNE) != RESET)
	{
		USART_ClearFlag(UART4, USART_FLAG_RDNE);
		res = USART_ReceiveData(UART4);
        if(Uart_4.Rxd_Received == 0)
        {
            Uart_4.Rxd_Data[Uart_4.Rxd_Number++] = res;
            if(Uart_4.Rxd_Number >= 1100)
                Uart_4.Rxd_Received = 1;            //  阻止其溢出及非法访问
            // USART_SendData(UART4,res);
        }

	}
}
