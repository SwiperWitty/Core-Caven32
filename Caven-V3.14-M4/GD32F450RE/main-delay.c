
#if 0

#include <stdio.h>
#include "gd32f4xx.h"
#include "gd32f450i_eval.h"
#include "systick.h"
#include "SYS_Time.h"

int uart1_init(int baud,int set)
{
    int retval = 0;
    if(set)
    {
        if(baud % 9600)
        {
            retval = 1;
            return retval;
        }
        rcu_periph_clock_enable(RCU_GPIOA);
        /* enable USART clock */
        rcu_periph_clock_enable(RCU_USART0);
        
        gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_9 | GPIO_PIN_10);
        gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPIO_PIN_9);
        gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_9);

        /* configure USART Rx as alternate function push-pull */
        gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPIO_PIN_10);
        gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_10);
        
        usart_deinit(USART0);
        usart_baudrate_set(USART0,baud);
        usart_parity_config(USART0, USART_PM_NONE);
        usart_word_length_set(USART0, USART_WL_8BIT);
        usart_stop_bit_set(USART0, USART_STB_1BIT);
        
        usart_receive_config(USART0, USART_RECEIVE_ENABLE);
        usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
        
        usart_interrupt_enable(USART0, USART_INT_RBNE);
        usart_enable(USART0);
        
        nvic_irq_enable(USART0_IRQn, 0, 0);
    }
    else
    {
        rcu_periph_clock_disable(RCU_USART0);
        nvic_irq_disable(USART0_IRQn);
    }
    return retval;
}

void UART0_Send_Data(const void * data,int size)
{
    int temp = 0;
    if(data == NULL || size <= 0)
    {
        return;
    }
    while(temp < size)
    {
        temp++;
        usart_data_transmit(USART0, *((char*)data + temp));
        while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
    }
}

int main(void)
{
    Sys_Time_Init(ENABLE);
    uart1_init(115200,ENABLE);
    
    SYS_Delay_ms(100);
    printf("p: a usart transmit test example! \n");
    UART0_Send_Data("hello, world ! \r\n",sizeof("hello, world ! \r\n"));
    int num = 0;
    int temp;
    curtime_t time_a;
    
    while(1)
    {
        cpu_get_lltimer(&time_a);
        temp = time_a.s_tenth;
        SYS_Delay_ms(1000);
        temp = cpu_diff_lltimer(time_a);
        printf("p: time diff is %d (%d)\n",temp,1);
        temp = cpu_diff_ms(time_a);
        printf("p: time diff is %d ms (%d)\n",temp,2);
        temp = cpu_diff_us(time_a);
        printf("p: time diff is %d us (%d)\n ",temp,3);
        num++;
    }
}

int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    return ch;
}

#endif
