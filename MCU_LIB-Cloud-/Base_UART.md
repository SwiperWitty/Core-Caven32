# Base_UART



串口本身不麻烦，但是如何接收这个事情比较麻烦，不同的接收方式和逻辑代码就完全不同，所以把这个复杂的问题丢给【Mode->UART】。

![image-20220917171805066](https://raw.githubusercontent.com/SwiperWitty/img/main/img/image-20220917171805066.png)



### 中断接口 & 串口编号

明确那些通道能用

~~~C
#define UART1_EXIST 1
#define UART2_EXIST 2
#define UART3_EXIST 3
#define UART4_EXIST 4

#define UART_End 5
~~~



中断的函数（沁恒的需要说明函数）

~~~c
#ifdef Exist_UART
/*  中断   */
	#ifdef UART1_EXIST
    	#define UART1_Interrupt() USART1_IRQHandler()
	#endif
    #ifdef UART2_EXIST
    	#define UART2_Interrupt() USART2_IRQHandler()
	#endif
    #ifdef UART3_EXIST
    	#define UART3_Interrupt() USART3_IRQHandler()
	#endif
#endif
~~~



### 初始化

~~~c
void Uart1_Init(int Baud,int SET);
~~~

就是常规的配置，提供波特率可定义；SET是使能，传给UARTA的时钟使能、UART对应的GPIO（如果不使能串口1，对应GPIO设置成模拟输入）。

~~~c
void Uart1_Init(int Baud,int SET)
{
    //xx
    FunctionalState temp;
    if(SET)
        temp = ENABLE;
    else
        temp = DISABLE;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,temp);

    if(temp)
    {
        //正常初始化GPIO、UART
        //TXD	AF(复用-输出)	
        //RXD	AF(复用-开漏)
    }
    else
    {
        //将GPIO设置成模拟输入
    }
    //xx
}

~~~



### 标志位获取/清除

以什么标志位为准

~~~c
#define RXD_Falg    USART_IT_RXNE		//  接收标志
#define TXD_Falg    USART_FLAG_TC		//  【USART_FLAG_TXE】这个只是说明，数据被cpu取走,【USART_FLAG_TC】这是完全发送完成
~~~



​	**RXD**

~~~c
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
~~~



​	**TXD**

不需要提供出去，他被集成在了发送里面。



### 发送/接收

​	**发送**

这个函数提供发送通道、数据即可

~~~c
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
~~~

​	

​	**接收**

~~~c
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
~~~



**printf**

~~~c
int fputc(int ch, FILE *f)      //printf
{
    USART_SendData(USART1,(uint8_t)ch);
    while (!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
    return (ch);
}
~~~

