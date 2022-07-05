#include "API.h"

struct API_Data_ API;


void API_Index(void)
{
    API.Get_CRC16 = CRC16_CCITT_CalculateBuf;

    API.DATA_Time.Hourly_to_Seconds = Hourly_to_Seconds;
    API.DATA_Time.Seconds_to_Hourly = Seconds_to_Hourly;
    API.DATA_Time.Over_Time = Over_Time;
}
