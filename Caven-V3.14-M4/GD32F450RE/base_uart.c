
#include "base_uart.h"

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
//        usart_halfduplex_disable(USART0);
        
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

void USART0_IRQHandler(void)
{
    if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE))
    {
        uint16_t temp;
        /* clear IDLE flag */
        temp = usart_data_receive(USART0);
        usart_data_transmit(USART0, temp);
        while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));

    }
}

