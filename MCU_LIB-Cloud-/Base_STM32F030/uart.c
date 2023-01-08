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
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,temp);
    
    if(temp)
    {
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
        
        GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_1);
        GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_1);     //复用IO
        
        GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;                 //TXD
        GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
        GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
        GPIO_Init(GPIOA,&GPIO_InitStructure);
        
        GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;                //RXD
        GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_OType=GPIO_OType_OD;
        GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_Init(GPIOA,&GPIO_InitStructure);
    }
    else
    {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;            //输出
    }
    GPIO_Init(GPIOA, &GPIO_InitStructure);
        
    USART_InitStructure.USART_BaudRate = Baud;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = 1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(UART_Temp, &USART_InitStructure);
    USART_ITConfig(UART_Temp, RXD_Falg, temp);
    
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = temp;
    NVIC_Init(&NVIC_InitStructure);
    
    USART_Cmd(UART_Temp,ENABLE);
}
void Uart2_Init(int Baud,int SET)
{
    
}


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
    default:
        return;
    }
    while (USART_GetFlagStatus(Temp, TXD_Falg) == 0);  
    USART_SendData(Temp, DATA);
    USART_ClearFlag(Temp, TXD_Falg);
}

int fputc(int ch, FILE *f)      //printf
{
    USART_SendData(USART1,(uint8_t)ch);
    while (!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
    return (ch);
}

