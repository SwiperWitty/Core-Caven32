#include "MODE.h"
#include "API.h"

int led;
char Free_array[100];
void Main_Init(void);
int main (void)
{
    Main_Init();
    
    while(1)
    {

        SYS_Delay_ms (5);
//        SPI_SET_Addr_SendData(1,led++,0XAA);
        memset(Free_array,0xf1,10);
    }
}

void Main_Init(void)
{
    Mode_Index();

}
