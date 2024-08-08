#include "Mode.h"
#include "API.h"
#include "pic.h" //图片



char temp_array[100];
void Main_Init(void);


int main(void)
{
	Main_Init();

	while (1)
	{
    
        
	}
}

void Main_Init(void)
{
    int reverse = 0;
	// 直接使用外部时钟
	Mode_Index();
    reverse |= Mode_Init.TIME(ENABLE);
    
	//
    
}
