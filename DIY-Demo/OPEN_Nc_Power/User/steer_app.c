#include "steer_app.h"


int Steer_app_init (int Set)
{
    int retval = 0;
    Mode_Init.Steering_Engine(ENABLE);
	Mode_Use.Steering_Engine.Set_Angle(1,0);
	Mode_Use.Steering_Engine.Set_Angle(2,90);
    return retval;
}


int Steer_app (Caven_App_Type * message)
{
    int retval = 0;
    int run_num = 0;
    static int first = 1;
    Caven_Control_Type control;

    if (message != NULL)
    {
        memcpy(&control,message->p_Data,sizeof(control));
        if(first)
        {
            message->str_switch = 1;
            message->cursor = 0;
            message->layer = first;
            first = 0;
        }
        if(control.Control_botton == 1)
        {
            switch (message->cursor)
            {
                case (2):
                    break;

                default:
                    message->app_ID = 1;        // 返回home
                    first = 1;
                    break;
            }
        }
        //
        if(control.Control_y > 0)
        {
            if(message->cursor >= (10 - 1))
            {
                message->cursor = 2;
            }
            else
            {
                message->cursor ++;
            }
            if(message->cursor > 2)        //!!!! 
            {
                message->cursor = (10 - 1);
            }
        }
        else if(control.Control_y < 0)
        {
            if(message->cursor <= 2)
            {
                message->cursor = (10 - 1);
            }
            else if(message->cursor == (10 - 1))
            {
                message->cursor = 2;
            }
            else
            {
                message->cursor --;
            }
        }
        message->cursor = MAX(message->cursor,2);       // 第一个可选是2

        //
        sprintf(message->string,"Caven Open power");            // 0
        run_num += sizeof("Caven Open power");
        if(message->layer == 1)
        {
            sprintf(message->string + run_num," ");             // 1
            run_num += sizeof(" ");
            sprintf(message->string + run_num,"this Steer !");  // 2
            run_num += sizeof("this Steer !");
            sprintf(message->string + run_num,"\nBack [user/steer]");    // end(x)
            
        }

    }

    return retval;
}

