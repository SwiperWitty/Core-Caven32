#ifndef _MP3_H_
#define _MP3_H_

//	可以跨芯片移植	需要大改底层(UART)

#include "uart.h"

#define	JQ8900	1
#define	JQ6500	2
#define Voice_Baud 9600

#define HLLO 1
#define END  2

void Voice_Init(void);
void Voice(char Model,char Num);


#endif
