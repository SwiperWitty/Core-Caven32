#ifndef _BZZ__H_
#define _BZZ__H_

#include "Base.h"
#include "Caven.h"

/*
        【MODE】      ————C语言环境即可运行



*/

struct BZZ_
{
    void (*BZZ_SET)(int SET);
};

void BZZ_Init(int SET);

void BZZ_SET(int SET);

#endif
