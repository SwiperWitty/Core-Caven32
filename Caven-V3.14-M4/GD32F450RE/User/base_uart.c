
#include "base_uart.h"

#include "timetick.h"
//#include "readerCfg.h"
//#include "errorwarntype.h"




#define USART0_DATA_ADDRESS    ((uint32_t)&USART_DATA(USART0))

void DMA_UART_Send(int buff_addr,int size)
{
    dma_single_data_parameter_struct dma_init_struct;
    rcu_periph_clock_enable(RCU_DMA1);

    dma_deinit(DMA1, DMA_CH7);

    dma_init_struct.direction = DMA_MEMORY_TO_PERIPH;
    dma_init_struct.memory0_addr = buff_addr;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
    dma_init_struct.number = size;
    dma_init_struct.periph_addr = USART0_DATA_ADDRESS;
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_single_data_mode_init(DMA1, DMA_CH7, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA1, DMA_CH7);
    dma_channel_subperipheral_select(DMA1, DMA_CH7, DMA_SUBPERI4);
    /* enable DMA channel7 */
    dma_channel_enable(DMA1, DMA_CH7);
    
    /* USART DMA enable for transmission */
    usart_dma_transmit_config(USART0, USART_TRANSMIT_DMA_ENABLE);
    
    /* wait DMA channel transfer complete */
    while(RESET == dma_flag_get(DMA1, DMA_CH7, DMA_FLAG_FTF));
}

int uart0_init(int baud,int set)
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
        usart_halfduplex_disable(USART0);
        
        usart_receive_config(USART0, USART_RECEIVE_ENABLE);
        usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
        
        usart_interrupt_enable(USART0, USART_INT_RBNE);
        usart_enable(USART0);
        
        nvic_irq_enable(USART0_IRQn, 0, 1);
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
    u16 tx_data;
    if(data == NULL || size <= 0)
    {
        return;
    }
    while(temp < size)
    {
        tx_data = *((char*)data + temp);
        while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
        usart_data_transmit(USART0, tx_data);
        temp++;
    }
}

char array_tx[200];
void DMA_UART0_Send_Data(const void * data,int size)
{
  int time = size / sizeof(array_tx);
  int temp = size;
  int run = 0;
  if(data == NULL || size <= 0)
  {
      return;
  }
  do{
    if(time > 0) {temp = sizeof(array_tx);}
    else {temp = size % sizeof(array_tx);}
    
    memcpy(array_tx,(char *)data+run,temp);
    DMA_UART_Send((int)array_tx,temp);
    
    run += temp;
    time--;
  }while(time > 0);
}

//void USART0_IRQHandler(void)
//{
//    if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE))
//    {
//        uint16_t temp;
//        /* clear IDLE flag */
//        temp = usart_data_receive(USART0);
//        usart_data_transmit(USART0, temp);
//        while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
//
//    }
//}

void USART0_IRQHandler(void){
    
    if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE))
    {
      usart_data_receive(USART0);

    }
}

void  Ser_Printf (u8 *format, ...)
{
#if 0
   static  U8   buffer[80 + 1];
           va_list      vArgs;
  
   va_start(vArgs, format);
   vsnprintf((char *)buffer, sizeof(buffer), (char const *)format, vArgs);
   va_end(vArgs);

   Ser_WrStr((U8*) buffer);
   
#endif  
}

