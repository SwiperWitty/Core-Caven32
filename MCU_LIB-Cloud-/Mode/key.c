#include "key.h"

void KEY_Init (char Channel,int Set)
{
#ifdef Exist_KEY
    switch (Channel) {
        case 0:
            break;
        case 1:
            KEY_GPIO_Init(Set);
            break;
        case 2:

            break;
        default:
            break;
    }
#endif
}

U8 KEY_State (char Channel)
{
    U8 temp = 1;
	#ifdef Exist_KEY
    switch (Channel) {
        case 0:
            break;
        case 1:
            temp = KEY_R();
            break;
        default:
            break;
    }
	#endif
    return temp;
}
