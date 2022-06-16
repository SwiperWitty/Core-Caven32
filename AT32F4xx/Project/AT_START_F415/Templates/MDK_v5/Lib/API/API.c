#include "API.h"

struct _API_Data API;


void API_Index(void)
{
    API.Get_CRC16 = CRC16_CCITT_CalculateBuf;
    API.Agreement.Find_Data  = Find_Data;
    API.Agreement.Pick = Pick_Agreement;
    API.Agreement.Send = Send_Agreement;
}
