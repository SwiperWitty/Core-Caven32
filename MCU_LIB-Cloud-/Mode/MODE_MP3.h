#ifndef _MP3_H_
#define _MP3_H_

//	可以跨芯片移植	需要大改底层(UART)

#include "Base.h"
#include "Caven.h"

#define	JQ8900	1
#define	JQ6500	2
#define Voice_Baud 9600

#define HLLO 1
#define END  2

struct MP3_
{
    void (*Voice) (char Model,char Num);
};

void Voice_Init(int Set);
void Voice(char Model,char Num);


#endif
