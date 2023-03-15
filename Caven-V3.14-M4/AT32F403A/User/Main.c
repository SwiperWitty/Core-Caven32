#include "Mode.h"
#include "API.h"

#include "pic.h"
void Uart2_Init(int Baud,int Set);

void Main_Init(void);
int main (void)
{
    Main_Init();
    while(1)
    {
        
    }
}

void Main_Init(void)
{
    system_clock_config();
    Mode_Index();
    API_Index();
    
//    Mode_Init.LCD(ENABLE);
    Uart2_Init(115200,ENABLE);
    
//    Mode_User.LCD.Show_Picture(180,0,60,60,gImage_am_60);
    printf("holle world ! \r\n");
    
}

static usart_type * Temp;
void Uart2_Init(int Baud,int Set)
{    

    confirm_state set = FALSE;
    Temp = USART2;
    usart_reset(Temp);
    if (Set)
        set = TRUE;

    crm_periph_clock_enable(CRM_USART2_PERIPH_CLOCK, set);
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);

    gpio_init_struct.gpio_pins = GPIO_PINS_2;                           //Tx
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOA, &gpio_init_struct);

    gpio_init_struct.gpio_pins = GPIO_PINS_3;                           //Rx
    gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
    gpio_init_struct.gpio_pull = GPIO_PULL_UP;
    gpio_init(GPIOA, &gpio_init_struct);

    nvic_priority_group_config(NVIC_PRIORITY_GROUP_0);
    nvic_irq_enable(USART2_IRQn, 0, 2);

    usart_init(Temp, Baud, USART_DATA_8BITS, USART_STOP_1_BIT);   //波特率、位数、停止位
    usart_transmitter_enable(Temp, TRUE);         //发送使能
    usart_receiver_enable(Temp, TRUE);            //接收使能

    usart_parity_selection_config(Temp,USART_PARITY_NONE);    //无奇偶校验
    usart_interrupt_enable(Temp, USART_RDBF_INT, TRUE);
    usart_enable(Temp, TRUE);

}


int fputc(int ch, FILE *f)      //printf
{
#ifdef DEBUG_OUT

    while (usart_flag_get(Temp, USART_TDC_FLAG) == RESET);  
    usart_data_transmit(Temp, (uint8_t)ch);
#endif // DEBUG
    return (ch);
}
