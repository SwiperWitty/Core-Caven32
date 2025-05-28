#ifndef _POWER__APP__H_
#define _POWER__APP__H_

#include "Mode.h"
#include "API.h"

typedef struct
{
    char out_mode;
	char out_witch;
	char PD_val;
	char line_max;
	char line_end;
	
	int set_out_sync;
	
	char set_pd;
	float set_out_vol;	//voltage
	float set_out_temp;
	float set_ele_val;	//value
	float set_ele_temp;
	
	float IN_vol;
	float OUT_vol;
	float ELE_val;
	float TEM_val;
}Power_Control_Type;


int Power_app_init (int Set);
int Power_app (Caven_App_Type * message);

#endif
