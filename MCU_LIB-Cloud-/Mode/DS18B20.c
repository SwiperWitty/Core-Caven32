#include "DS18B20.h"

unsigned int Templ,Temp2,Temperature;  //Templ低八位，Temp2高八位
unsigned char Minus_Flag=0;  //负温度标志位


int DS18B20_Time = 0;
int DS18B20_Exist_Flag = 0;
static void Write_Byte (char Data);

static void DS18B20_Delay (int Num)
{
#ifdef Exist_DS18B20
    for (Num *= 5; Num > 0; Num--)
    {
        NOP();NOP();NOP();NOP();NOP();
		NOP();NOP();NOP();NOP();NOP();
		NOP();NOP();NOP();NOP();NOP();
		NOP();NOP();NOP();NOP();NOP();
		NOP();NOP();NOP();
    }
#endif
}

char DS18B20_Start (void)
{
	char temp = 0;
#ifdef Exist_DS18B20
	DS18B20_IO_Config(WRITE_Config);
	DS18B20_IO_H();DS18B20_Delay (50);
	DS18B20_IO_L();DS18B20_Delay (500);
	
	DS18B20_IO_H();DS18B20_Delay (5);
    DS18B20_IO_Config(READ_Config);
	DS18B20_Delay (50);
    int time = 0;
    do{
        DS18B20_Delay (50);
        time++;
        if(time > 6)
        {
//            DS18B20_IO_Config(READ_Config);
            DS18B20_Exist_Flag = 0;
            return temp;                    //启动失败了
        }
    }while(DS18B20_IO_R() == 1);
	DS18B20_IO_Config(WRITE_Config);
	DS18B20_IO_H();DS18B20_Delay (5);
	temp = 1;
#endif
	return temp;
}

char DS18B20_Init (int Set)
{
	char temp = 0;
#ifdef Exist_DS18B20
    DS18B20_IO_Config(WRITE_Config);
    DS18B20_Delay (1);
    DS18B20_Time = (MCU_SYS_Freq/6000000);
	DS18B20_Delay (500);
	if(DS18B20_Start () == 1)
	{
		DS18B20_Exist_Flag = 1;             //成功
		temp = 1;
        DS18B20_Delay (50);
        DS18B20_Get_Temp();
	}
#endif
    
	return temp; 
}

static void Write_Byte (char Data)
{
#ifdef Exist_DS18B20
    char Temp = Data;
    DS18B20_IO_Config(WRITE_Config);
    DS18B20_IO_H();
    DS18B20_Delay (10);
	
    for (char i = 8; i > 0; i--)
    {
        DS18B20_IO_L();
        DS18B20_Delay (8);

        if(Temp & 0x01)             //与1按位与运算，Data最低位为1时DQ总线为1，Data最低位为0时DQ总线为0
        {
            DS18B20_IO_H();
            DS18B20_Delay (30);
        }
        else
        {
            DS18B20_Delay (40);
        }
        Temp >>= 1;
        DS18B20_IO_H();
        DS18B20_Delay (10);

    }
    DS18B20_Delay (50);
#endif
}

static char Read_Byte (void)
{
    char Data = 0;
#ifdef Exist_DS18B20
    for (char i = 0; i < 8; i++)
    {
        DS18B20_IO_Config(WRITE_Config);
        DS18B20_IO_H();
        DS18B20_Delay (8);
        DS18B20_IO_L();         //低脉冲
		DS18B20_Delay (1);
        DS18B20_IO_Config(READ_Config);		//弱拉高

        DS18B20_Delay (20);
		Data >>= 1;
		if(DS18B20_IO_R())
		{ Data |= 0x80; }
		//else {0}
		DS18B20_Delay (40);
    }

    DS18B20_IO_Config(WRITE_Config);
	DS18B20_IO_H();
#endif
    return Data;
}

float DS18B20_Get_Temp(void)
{
    float Temp = 0;
	if(DS18B20_Start () == 0)
	{ 
//        return 0;
        DS18B20_Exist_Flag = 1;
    } 
	
    if(DS18B20_Exist_Flag)      //  在DB18B20存在的情况下
    {
        int Data;
        unsigned char Temp_H,Temp_L;
		char t = 0;

        Write_Byte(0xcc);	// skip rom		1100 1100
        Write_Byte(0x44);	// convert		0100 0100

		DS18B20_Start();
        
        Write_Byte(0xcc);	// skip rom
        Write_Byte(0xbe);	// convert		10111110

        Temp_L = Read_Byte ();
        Temp_H = Read_Byte ();
		
		if(Temp_H > 7)
		{
			Temp_H = ~Temp_H;
			Temp_L = ~Temp_L;
			t = 1;
		}
        Data = Temp_H;
        Data <<= 8;
        Data += Temp_L;

		Temp = Data;
		Temp *= 0.0625;	
		if(t)
		{Temp = -Temp;}
    }
    return Temp;
}


