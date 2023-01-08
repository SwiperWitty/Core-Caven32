#ifndef _MODE_H__
#define _MODE_H__
#include "Base.h"
#include "Caven.h"

#ifdef Exist_SYS_TIME
    #include "MODE_Time.h"
#endif

#ifdef Exist_LCD
    #include "LCD.h"            //显示输出
#endif
#ifdef Exist_OLED
    #include "OLED.h"            //显示输出
#endif

#ifdef Exist_BZZ
    #include "BZZ.h"            //BZZ输出
#endif
#ifdef Exist_LED
    #include "LED.h"            //LED输出
#endif

#ifdef Exist_UART
    #include "MODE_UART.h"
#endif

#ifdef Exist_HC595
    #include "HC595.h"			//驱动输出
#endif
#ifdef Exist_HC138
    #include "HC138.h"
#endif

#ifdef Exist_DS18B20
    #include "DS18B20.h"
#endif

#ifdef Exist_Voice
    #include "MODE_MP3.h"
#endif
#ifdef Exist_Steering_Engine    //动力输出
    #include "Steering_engine.h"
#endif
#ifdef Exist_Motor
    #include "MODE_motor.h"
#endif

#ifdef Exist_KEY
    #include "KEY.h"            //按键输入
#endif

#ifdef Exist_FindLine
    #include "Tracing.h"		//外部需要模块才能输入的条件
#endif

#ifdef Exist_Ultrasonic
    #include "MODE_Ultrasonic.h"
#endif

#ifdef Exist_MLX90614
    #include "mlx90614.h"
#endif



/*
    SDK->Items->GPIO(Exist_GPIO)->BASE->
                                         \\
                                          -->[XXX]->MODE
                                         //
                    C(Lib)->Caven->API->
*/

extern struct _Mode_Init Mode_Init;
extern struct _Mode_User Mode_User;


struct _Mode_Init
{
    char empty;
#ifdef Exist_LCD
	void (*LCD)(int SET);
#endif
#ifdef Exist_SYS_TIME
	void (*Sys_Clock)(int SET);
#endif

#ifdef Exist_UART
	void (*UART)(char Channel,int Baud,int SET);
#endif
#ifdef Exist_LED
	void (*LED)(int SET);
#endif
#ifdef Exist_BZZ
    void (*BZZ)(int SET);
#endif
#ifdef Exist_HC595
    void (*HC_595)(int SET);
#endif
#ifdef Exist_HC138
    void (*HC_138)(int SET);
#endif

#ifdef Exist_DS18B20
    char (*DS18B20)(int SET);
#endif

#ifdef Exist_KEY
    void (*KEY)(char Channel,int SET);
#endif
#ifdef Exist_Ultrasonic
    void (*Ultrasonic)(int SET);
#endif

#ifdef Exist_Voice
    void (*Voice) (int Set);
#endif
#ifdef Exist_Steering_Engine
    void (*Steering_Engine) (int Set);
#endif

#ifdef Exist_ADC
    void (*User_ADC) (int Set);
#endif


};

struct _Mode_User
{
    void (*Debug_Out)(const char *String);          //提倡写一个Debug
#ifdef Exist_LCD
    struct LCD_ LCD;
#endif
#ifdef Exist_SYS_TIME
    struct Delay_ Delay;
	struct TIME_ Sys_Clock;
#endif
#ifdef Exist_UART
    struct _Uart_ UART;                             //面向对象
#endif
#ifdef Exist_LED
    struct LED_ LED;
#endif
    #ifdef Exist_BZZ
    struct BZZ_ BZZ;
#endif
#ifdef Exist_HC595
    struct HC595_ HC595;
#endif
#ifdef Exist_HC138
    struct HC138_ HC138;
#endif
#ifdef Exist_DS18B20
    struct DS18B20_ DS18B20;
#endif

#ifdef Exist_KEY
    struct KEY_ KEY;
#endif
#ifdef Exist_Ultrasonic
    struct Ultrasonic_ Ultrasonic;
#endif

#ifdef Exist_Voice
    struct MP3_ MP3;
#endif
#ifdef Exist_Steering_Engine
    struct Steering_Engine_ Steering_Engine;
#endif

#ifdef Exist_ADC
    struct _USER_ADC_ USER_ADC;
#endif
};

void Mode_Index(void); //初始化Mode函数索引,真正功能的初始化请调用结构体中函数指针(Mode_Init)
void Debug_Out(const char *String);


//以下是勘误，如果你已完成配置请将其注释！

#if END_Data != NO_END
//    #warning "Attention that UART requires end Data !!! File: ./Base./<Base_UART.h> "
#endif





#endif
